#include "stdafx.h"
#include "LogicCenter.h"
#include "Help.h"
#include "UtilPath.h"
#include "UtilLog.h"

#define MAX_SECTION   (256)
#define MAX_FILE_SIZE (10 * 1024 * 1024)

LogicCenter::LogicCenter() : m_seq(0), m_MainHtml_TaskID(0)
{
	m_strConfFilePath = Util::Path::GetDTmpFolder() + _T("\\FileConfig.ini");

	m_strMainHtmlFile = Util::Path::GetDTmpFolder() + _T("\\meten-tv_check.html");
	Util::FileTrans::HttpDownLoad::DownLoad(_T("http://coursetest.miamusic.com/meten-tv.html"), m_strMainHtmlFile, Util::FileTrans::FILE_TYPE_NORMAL, this, &m_MainHtml_TaskID);

	LoadConf();
}

LogicCenter::~LogicCenter()
{
	//SaveConf();
}

///
void	LogicCenter::GetFilePathList(map<CString, CString>& list)
{
	list.clear();
	for (map<CString, FileItem>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		list[it->first] = it->second.filepath;
	}
}
///

void	LogicCenter::OnJsEvent_GetList()
{
	map<CString, CString> list;
	GetFilePathList(list);
	CallJs_LocalList(list);
}

void	LogicCenter::OnJsEvent_SyncList(const map<CString, CString>& list)	//id->url
{
	m_seq++;
	for (map<CString, CString>::const_iterator it = list.begin(); it != list.end(); it++)
	{
		map<CString, FileItem>::iterator it2 = m_items.find(it->first);
		if (it2 == m_items.end())
		{
			FileItem	item;
			item.url = it->second;
			item.seq = m_seq;

			m_items[it->first] = item;
		}
		else
		{
			it2->second.url = it->second;	
			it2->second.seq = m_seq;		
			if (it2->second.state == FIE_DISCARD)
			{
				it2->second.state = FIE_NONE;
			}
		}
	}
	for (map<CString, FileItem>::iterator it = m_items.begin(); it != m_items.end();)
	{
		if (it->second.seq != m_seq)
		{
			if (it->second.state == FIE_DOWNING)
			{
				Util::FileTrans::HttpDownLoad::Stop(it->second.task_id);
				it->second.state = FIE_DISCARD;
			}
			m_items.erase(it++);
		}
		else
		{
			if (it->second.state == FIE_NONE)
			{
				it->second.filepath = Util::Path::GetDTmpFolder() + _T("\\") + it->first + _T(".mp4");
				Util::FileTrans::HttpDownLoad::DownLoad(it->second.url, it->second.filepath, Util::FileTrans::FILE_TYPE_NORMAL, this, &it->second.task_id);
				it->second.state = FIE_DOWNING;
			}
			it++;
		}
	}	

	SaveConf();
}

void	LogicCenter::OnJsEvent_EnterFullScreen()
{
	GetSafeChannel()->EnterFullScreen_Helper();
}

void	LogicCenter::OnJsEvent_ExitFullScreen()
{
	GetSafeChannel()->ExitFullScreen_Helper();
}

void	LogicCenter::OnJsEvent_Upgrade(const CString& ver, const CString& url, const CString& md5)
{
	CString strText;
	strText.Format(_T("ver: %s\nurl: %s\nmd5:%s"), ver, url, md5);
	MessageBox(0, strText, _T("��ʾ"), MB_OK);
}

BOOL LogicCenter::GetSections(vector<CString>& vSections)
{
	vSections.clear();

	/*
	������������
	GetPrivateProfileSectionNames - �� ini �ļ��л�� Section ������
	��� ini �������� Section: [sec1] �� [sec2]���򷵻ص��� 'sec1',0,'sec2',0,0 �����㲻֪��
	ini ������Щ section ��ʱ���������� api ����ȡ����
	*/
	int i;
	int iPos = 0;
	int iMaxCount;
	TCHAR* chSectionNames = new TCHAR[MAX_FILE_SIZE]; //�ܵ���������ַ���
	memset(chSectionNames, 0, MAX_FILE_SIZE * sizeof(TCHAR));

	TCHAR chSection[MAX_SECTION] = { 0 }; //���һ��������
	GetPrivateProfileSectionNames(chSectionNames, MAX_FILE_SIZE, m_strConfFilePath);

	//����ѭ�����ضϵ�����������0
	for (i = 0; i < MAX_FILE_SIZE - 1; i++)
	{
		if (chSectionNames[i] == 0 && chSectionNames[i + 1] == 0)
		{
			break;
		}		
	}

	iMaxCount = i + 1; //Ҫ��һ��0��Ԫ�ء����ҳ�ȫ���ַ����Ľ������֡�
	for (i = 0; i < iMaxCount; i++)
	{
		if (iPos + 1 >= MAX_SECTION)
		{
			break;
		}
		chSection[iPos++] = chSectionNames[i];
		if (iPos > 1 && chSectionNames[i] == 0)
		{
			vSections.push_back(chSection);
			memset(chSection, 0, i);
			iPos = 0;
		}
	}

	return TRUE;
}

CString LogicCenter::GetStateStrByCode(const FILE_ITEM_STATE& state)
{
	switch (state)
	{
	case FIE_NONE:
		return _T("None");
	case FIE_DOWNING:
		return _T("Downing");
	case FIE_AVAILABLE:
		return _T("Available");
	case FIE_DISCARD:
		return _T("Discard");
	default:
		ATLASSERT(FALSE);
		return _T("Error");
	}
}

LogicCenter::FILE_ITEM_STATE LogicCenter::GetStateCodeByStr(const CString& str)
{
	if (!str.CompareNoCase(_T("None")))
	{
		return FIE_NONE;
	}
	else if (!str.CompareNoCase(_T("Downing")))
	{
		return FIE_DOWNING;
	}
	else if (!str.CompareNoCase(_T("Available")))
	{
		return FIE_AVAILABLE;
	}
	else if (!str.CompareNoCase(_T("Discard")))
	{
		return FIE_DISCARD;
	}
	else
	{
		ATLASSERT(FALSE);
		return FIE_NONE;
	}
}

void	LogicCenter::LoadConf()
{
	//�������ļ�����10M������0��ʼ
	if (Util::Path::GetFileLength(m_strConfFilePath) > MAX_FILE_SIZE)
	{
		return;
	}

	//С�ģ������ڿ�ͷload��������load����bug
	m_items.clear();

	vector<CString> vSections;
	GetSections(vSections);

	for (vector<CString>::iterator it = vSections.begin(); it != vSections.end(); it++)
	{
		FileItem	fi;

		TCHAR szTemp[1024] = {0};
		GetPrivateProfileString(*it, _T("FilePath"), _T(""), szTemp, sizeof(szTemp)-1, m_strConfFilePath);
		fi.filepath = szTemp;
		GetPrivateProfileString(*it, _T("State"), _T(""), szTemp, sizeof(szTemp)-1, m_strConfFilePath);
		fi.state = GetStateCodeByStr(szTemp);

		m_items[*it] = fi;
	}
}

void	LogicCenter::SaveConf(const CString& id/* = _T("")*/)
{
	if (id.IsEmpty())
	{
		for (map<CString, FileItem>::iterator it = m_items.begin(); it != m_items.end(); it++)
		{
			WritePrivateProfileString(it->first, _T("FilePath"), it->second.filepath, m_strConfFilePath);
			WritePrivateProfileString(it->first, _T("State"), GetStateStrByCode(it->second.state), m_strConfFilePath);
		}
	}
	else
	{
		map<CString, FileItem>::iterator it = m_items.find(id);
		if (it != m_items.end())
		{
			WritePrivateProfileString(it->first, _T("FilePath"), it->second.filepath, m_strConfFilePath);
			WritePrivateProfileString(it->first, _T("State"), GetStateStrByCode(it->second.state), m_strConfFilePath);
		}
	}
}

void	LogicCenter::CallJs_LocalList(const map<CString, CString>& list)	//id->filepath
{
	CStringA json_text;
	if (Encode_Cpp2Js_FileList(list, json_text))
	{
		GetSafeChannel()->Cpp2Js(_T("fileList"), json_text);
	}
}

void	LogicCenter::CallJs_DownCompleted(const CString& id, const CString& filepath)
{
	CStringA json_text;
	if (Encode_Cpp2Js_DownComplete(id, filepath, json_text))
	{
		GetSafeChannel()->Cpp2Js(_T("notifyDownComplete"), json_text);
	}
}

void	LogicCenter::CallJs_MessageBox(const CString& text)
{
	CStringA json_text;
	if (Encode_Cpp2Js_MessageBox(text, json_text))
	{
		GetSafeChannel()->Cpp2Js(_T("notifyMessageBox"), json_text);
	}
}

void LogicCenter::OnHttpDownProgress(UINT64 TaskID, UINT64 pos, UINT64 range, UINT64 speed, UINT32 remain_time)
{
}

void LogicCenter::OnHttpDownResult(UINT64 TaskID, UINT32 code)
{
	if (TaskID == m_MainHtml_TaskID)
	{
		if (Util::FileTrans::c_suc == code)
		{
			Util::Log::Info(_T("TVCenter"), _T("mainhtml suc id: %llu, path: %s"), TaskID, m_strMainHtmlFile);
		}
		else
		{
			Util::Log::Info(_T("TVCenter"), _T("mainhtml down fail id: %llu, path: %s, err: %u"), TaskID, m_strMainHtmlFile, code);
		}
		m_MainHtml_TaskID = 0;

		return;
	}

	for (map<CString, FileItem>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		if (it->second.task_id == TaskID)
		{
			if (Util::FileTrans::c_suc == code)
			{
				it->second.state = FIE_AVAILABLE;
				it->second.task_id = 0;

				SaveConf(it->first);
				CallJs_DownCompleted(it->first, it->second.filepath);
				Util::Log::Info(_T("TVCenter"), _T("down suc id: %s, path: %s, url: %s"), it->first, it->second.filepath, it->second.url);
			}
			else
			{
				CString strText;
				strText.Format(_T("����diΪ %s ���ļ� %s ʧ��"), it->first, it->second.filepath);
				CallJs_MessageBox(strText);

				Util::Log::Info(_T("TVCenter"), _T("down fail id: %s, path: %s, url: %s, err: %u"), it->first, it->second.filepath, it->second.url, code);
			}
			
			break;
		}
	}
}
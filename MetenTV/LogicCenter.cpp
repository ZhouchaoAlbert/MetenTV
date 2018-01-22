#include "stdafx.h"
#include "LogicCenter.h"
#include "Help.h"
#include "UtilPath.h"
#include "UtilLog.h"

#define MAX_SECTION   (256)
#define MAX_FILE_SIZE (10 * 1024 * 1024)

LogicCenter::LogicCenter() : m_seq(0), m_MainHtml_TaskID(0), m_UpgradeFile_TaskID(0)
{
	m_strConfFilePath = Util::Path::GetDTmpFolder() + _T("\\FileConfig.ini");

	m_strMainHtmlFile = Util::Path::GetDTmpFolder() + _T("\\meten-tv_check.html");
	DeleteFile(m_strMainHtmlFile);
	Util::Log::Info(_T("TVCenter"), _T("delete mainfile_bak: %s, hr: 0x%x"), m_strMainHtmlFile, GetLastError());
	Util::FileTrans::HttpDownLoad::DownLoad(_T("http://coursetest.miamusic.com/meten-tv.html"), m_strMainHtmlFile, Util::FileTrans::FILE_TYPE_NORMAL, this, &m_MainHtml_TaskID, FALSE, TRUE);

	LoadConf();

//	OnJsEvent_Upgrade(_T("2"), _T("http://coursetest.miamusic.com/meten-tv.html"), _T("02353456346346"));
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
	CString strFolder = Util::Path::GetDTmpFolder() + _T("\\");
	for (map<CString, CString>::const_iterator it = list.begin(); it != list.end(); it++)
	{
		Util::Log::Info(_T("TVCenter"), _T("sync id(.mp4 name): %s, url: %s"), it->first, it->second);

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

			//安全校验，避免外部修改配置文件导致误操作
			if (it->second.filepath.Find(strFolder, 0) == 0 && Util::Path::IsFileExist(it->second.filepath))
			{
				DeleteFile(it->second.filepath);
				Util::Log::Info(_T("TVCenter"), _T("delete file: %s, hr: 0x%x"), it->second.filepath, GetLastError());
			}

			m_items.erase(it++);
		}
		else
		{
			if (it->second.state == FIE_NONE)
			{
				it->second.filepath = strFolder + it->first + _T(".mp4");
				it->second.state = FIE_DOWNING;
			}
			if (it->second.state == FIE_DOWNING)	
			{
				UINT32 ret = Util::FileTrans::HttpDownLoad::DownLoad(it->second.url, it->second.filepath, Util::FileTrans::FILE_TYPE_NORMAL, this, &it->second.task_id);
				if (Util::FileTrans::c_file_repeat == ret)
				{
					it->second.task_id = 0;
					it->second.state = FIE_AVAILABLE;
					CallJs_DownCompleted(it->first, it->second.filepath);
					Util::Log::Info(_T("TVCenter"), _T("down exist id: %s, path: %s, url: %s"), it->first, it->second.filepath, it->second.url);
				}
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

void	LogicCenter::OnJsEvent_QueryVersion()
{
	CStringA json_text;
	if (Encode_Cpp2Js_ReportVersion(PROG_VER, json_text))
	{
		GetSafeChannel()->Cpp2Js(_T("reportVersion"), json_text);
	}
}

void	LogicCenter::OnJsEvent_Upgrade(const CString& ver, const CString& url, const CString& md5)
{
	UINT32	u32AskVer = _tcstoul((LPCTSTR)ver, NULL, 10);
	if (u32AskVer <= PROG_VER)
	{
		Util::Log::Info(_T("TVCenter"), _T("stop upgrade, ask_ver: %u, local_ver: %u"), u32AskVer, PROG_VER);
		return;
	}

	CString strUpgradeConf = Util::Path::GetDUpgradeFolder(TRUE) + _T("\\UpgradeConf.ini");

	//是否正在进行
	TCHAR szTemp[1024] = { 0 };
	GetPrivateProfileString(_T("Main"), _T("Version"), _T(""), szTemp, sizeof(szTemp)-1, strUpgradeConf);
	UINT32	u32Ver = _tcstoul(szTemp, NULL, 10);
	Util::Log::Info(_T("TVCenter"), _T("stop upgrade, ask_ver: %u, upgrading_ver: %u, m_UpgradeFile_TaskID: %u"), u32AskVer, u32Ver, m_UpgradeFile_TaskID);
	if (u32AskVer < u32Ver)
	{
		return;
	}
	else if (u32AskVer == u32Ver)
	{
		//可能升级被手工中止，需要check
		if (m_UpgradeFile_TaskID)
		{
			return;
		}

		GetPrivateProfileString(_T("Main"), _T("State"), _T(""), szTemp, sizeof(szTemp)-1, strUpgradeConf);
		CString	strState = szTemp;
		if (!strState.CompareNoCase(_T("Finish")))
		{
			return;
		}
	}
	if (m_UpgradeFile_TaskID)
	{
		Util::FileTrans::HttpDownLoad::Stop(m_UpgradeFile_TaskID);
		m_UpgradeFile_TaskID = 0;
	}
	
	CString	strFilePath;
	strFilePath.Format(_T("%s\\ver_%u.exe"), Util::Path::GetDUpgradeFolder(TRUE), u32AskVer);
	WritePrivateProfileString(_T("Main"), _T("FilePath"), strFilePath, strUpgradeConf);
	WritePrivateProfileString(_T("Main"), _T("Md5"), md5, strUpgradeConf);
	WritePrivateProfileString(_T("Main"), _T("State"), _T("Downing"), strUpgradeConf);
	WritePrivateProfileString(_T("Main"), _T("Version"), ver, strUpgradeConf);

	DeleteFile(strFilePath);
	Util::Log::Info(_T("TVCenter"), _T("url: %s, del upgrad temp file: %s, hr: 0x%x"), url, strFilePath, GetLastError());
	Util::FileTrans::HttpDownLoad::DownLoad(url, strFilePath, Util::FileTrans::FILE_TYPE_NORMAL, this, &m_UpgradeFile_TaskID, FALSE, TRUE);
}

BOOL LogicCenter::GetSections(vector<CString>& vSections)
{
	vSections.clear();

	/*
	本函数基础：
	GetPrivateProfileSectionNames - 从 ini 文件中获得 Section 的名称
	如果 ini 中有两个 Section: [sec1] 和 [sec2]，则返回的是 'sec1',0,'sec2',0,0 ，当你不知道
	ini 中有哪些 section 的时候可以用这个 api 来获取名称
	*/
	int i;
	int iPos = 0;
	int iMaxCount;
	TCHAR* chSectionNames = new TCHAR[MAX_FILE_SIZE]; //总的提出来的字符串
	memset(chSectionNames, 0, MAX_FILE_SIZE * sizeof(TCHAR));

	TCHAR chSection[MAX_SECTION] = { 0 }; //存放一个段名。
	GetPrivateProfileSectionNames(chSectionNames, MAX_FILE_SIZE, m_strConfFilePath);

	//以下循环，截断到两个连续的0
	for (i = 0; i < MAX_FILE_SIZE - 1; i++)
	{
		if (chSectionNames[i] == 0 && chSectionNames[i + 1] == 0)
		{
			break;
		}		
	}

	iMaxCount = i + 1; //要多一个0号元素。即找出全部字符串的结束部分。
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
	//如配置文件大于10M，将从0开始
	if (Util::Path::GetFileLength(m_strConfFilePath) > MAX_FILE_SIZE)
	{
		return;
	}

	//小心，仅仅在开头load，运行中load会有bug
	m_items.clear();

	vector<CString> vSections;
	GetSections(vSections);

	for (vector<CString>::iterator it = vSections.begin(); it != vSections.end(); it++)
	{
		TCHAR szTemp[1024] = {0};
		GetPrivateProfileString(*it, _T("State"), _T(""), szTemp, sizeof(szTemp)-1, m_strConfFilePath);
		FILE_ITEM_STATE fis = GetStateCodeByStr(szTemp);
		if (FIE_DISCARD != fis)
		{
			FileItem	fi;
			GetPrivateProfileString(*it, _T("FilePath"), _T(""), szTemp, sizeof(szTemp)-1, m_strConfFilePath);
			fi.filepath = szTemp;
			fi.state = fis;

			m_items[*it] = fi;
		}
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
	else if (TaskID == m_UpgradeFile_TaskID)
	{
		CString strUpgradeConf = Util::Path::GetDUpgradeFolder(TRUE) + _T("\\UpgradeConf.ini");
		WritePrivateProfileString(_T("Main"), _T("State"), _T("Finish"), strUpgradeConf);

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
				strText.Format(_T("下载di为 %s 的文件 %s 失败"), it->first, it->second.filepath);
				CallJs_MessageBox(strText);

				Util::Log::Info(_T("TVCenter"), _T("down fail id: %s, path: %s, url: %s, err: %u"), it->first, it->second.filepath, it->second.url, code);
			}
			
			break;
		}
	}
}
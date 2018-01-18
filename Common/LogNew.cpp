#include "stdafx.h"
#include "LogNew.h"
#include "UtilPath.h"

#define MAX_LOG_FILE_SIZE		(200 * 1024 * 1025)

CLogNew::CLogNew()
	: m_hFile(INVALID_HANDLE_VALUE)
	, m_hThread(NULL)
	, m_bRun(false)
	, m_u32UserID(0)
	, m_eLogLevel(Util::Log::E_RLL_NONE)
{
	::InitializeCriticalSection(&m_cs);
}

CLogNew::~CLogNew()
{
	UnInit();
	::DeleteCriticalSection(&m_cs);
}

// ��������
CLogNew * CLogNew::GetObject()
{
	static CLogNew obj;
	return &obj;
}

// ��ʼ����־��
bool CLogNew::Init()
{
	if (m_hThread)
		return true;

	UINT uThreadID = 0;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, (void*)this, 0, &uThreadID);
	if (m_hThread)
	{
		m_bRun = true;
	}
	return m_hThread != NULL;
}

// ж����־��
void CLogNew::UnInit()
{
	m_bRun = false;
	if (m_hThread)
	{
		::WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	Writes();
	CloseFile();
}

// ������־�ȼ�
void CLogNew::SetLevel(Util::Log::EnumRawLogLevel eLogLevel)
{
	m_eLogLevel = eLogLevel;
}

// �ı���־·����Ϣ
BOOL CLogNew::Change(LPCTSTR szFilePath, UINT32 u32UID)
{
	m_u32UserID = u32UID;
	if (NULL == szFilePath)
	{
		TCHAR szDir[MAX_PATH + 1] = { 0 };
		GetModuleFileName(NULL, szDir, MAX_PATH);
		m_strFilePath = Util::Path::GetLogFolder() + _T("\\") + Util::Path::GetFileName(szDir) + _T(".log");
	}
	else
	{
		m_strFilePath = szFilePath;
	}
	Util::Path::ValidFilePath(m_strFilePath);
	return TRUE;
}

// ƴ��־�ַ���
void CLogNew::Write(Util::Log::EnumRawLogLevel eLogLevel, LPCTSTR pchKeyword, LPCTSTR pchFmt, va_list va)
{
	if (m_strFilePath.IsEmpty())
	{
		ATLASSERT(0);
	}
	if (eLogLevel < m_eLogLevel)
	{
		return;
	}
	CString str;
	GetText(str, pchKeyword, pchFmt, va);
	Add(str);
}

//////////////////////////////////////////////////////////////////////////
//
// ��������
//
// �����־���ݵ�����
void CLogNew::Add(const CString & strText)
{
	::EnterCriticalSection(&m_cs);
	m_Logs.push_back(strText);
	::LeaveCriticalSection(&m_cs);
}

// ��־ͷ
void CLogNew::GetText(CString& strText, LPCTSTR pchKeyword, LPCTSTR pchFmt, va_list va)
{
	CString str;
	str.FormatV(pchFmt, va);

	SYSTEMTIME t;
	::GetLocalTime(&t);
	strText.Format(_T("\n%04d-%02d-%02d %02d:%02d:%02d.%03d [%u|%s] %s"),
		t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, m_u32UserID, pchKeyword, str);
}

// ��־�ļ�д��
void CLogNew::Writes()
{
	list<CString> logs;

	// ��ֹ�ļ������ã���־���ݱ�������
	if (!OpenFile(m_strFilePath))
		return;

	::EnterCriticalSection(&m_cs);
	if (!m_Logs.empty())
	{
		logs = m_Logs;
		list<CString>().swap(m_Logs);
	}
	::LeaveCriticalSection(&m_cs);

	list<CString>::iterator it = logs.begin();
	while (it != logs.end())
	{
		// �ļ��Ƿ�̫��
		if (IsTooLarge())
		{
			Backup();
			OpenFile(m_strFilePath);
		}

		// ����д��
		WriteFile(*it);
		it++;
	}

	CloseFile();
}

// ���ļ�
bool CLogNew::OpenFile(LPCTSTR pchFilePath)
{
	if (m_hFile != INVALID_HANDLE_VALUE)
		return true;

	m_strFilePath = pchFilePath;
	if (m_strFilePath.IsEmpty())
		return false;
	m_hFile = CreateFile(m_strFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		::SetFilePointer(m_hFile, 0, NULL, FILE_END);
	}
	return INVALID_HANDLE_VALUE != m_hFile;
}

// �ر��ļ�
void CLogNew::CloseFile()
{
	if (INVALID_HANDLE_VALUE != m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

// д�ļ�
void CLogNew::WriteFile(const CString & strBuffer)
{
	DWORD dwWritted = 0;
	if (INVALID_HANDLE_VALUE != m_hFile)
		::WriteFile(m_hFile, strBuffer.GetString(), strBuffer.GetLength() * sizeof(TCHAR), &dwWritted, NULL);
}

// �ļ��Ƿ����
bool CLogNew::IsTooLarge()
{
	if (m_hFile)
	{
		LARGE_INTEGER stFileSize;
		BOOL bRet = GetFileSizeEx(m_hFile, &stFileSize);
		if (!bRet)
			return false;
		if (stFileSize.QuadPart > MAX_LOG_FILE_SIZE)
			return true;
	}
	return false;
}

// ����������
void CLogNew::Backup()
{
	CloseFile();

	CString strTemp = m_strFilePath;
	FileAppendNumber(strTemp);
	MoveFile(m_strFilePath, strTemp);
}

// �����ļ�������
void CLogNew::FileAppendNumber(CString & strFilePath)
{
	CString strNewPath = strFilePath;
	CString strFolder = Util::Path::GetParentDirPath(strNewPath);
	CString strExt = Util::Path::GetFileExt(strNewPath);
	CString strShort = Util::Path::GetFileName(strNewPath);

	for (int nIndex = 1; nIndex != 0; nIndex++)
	{
		if (!::PathFileExists(strNewPath)){
			strFilePath = strNewPath;
			break;
		}
		strNewPath.Format(_T("%s\\%s(%d)%s"), strFolder, strShort, nIndex, strExt);
	}
}

// ��־�߳�
UINT WINAPI  CLogNew::ThreadProc(void* pVoid)
{
	DWORD dwTickCount = ::GetTickCount();
	CLogNew *pThis = (CLogNew *)pVoid;
	while (pThis->m_bRun)
	{
		DWORD dwCurrentTick = ::GetTickCount();
		DWORD dwCount = dwCurrentTick - dwTickCount;
		if (dwCount > 60 * 1000)
		{
			dwTickCount = dwCurrentTick;
			pThis->Writes();
		}
		Sleep(1);
	}
	return 0;
}
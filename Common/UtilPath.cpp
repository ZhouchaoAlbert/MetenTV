#include "stdafx.h"
#include "UtilPath.h"
#include "UtilTime.h"
#include <ShlObj.h>
#include "UtilLog.h"
#include "KtmW32.h"

static CString	s_strExePath;

BOOL	Util::Path::SetExeFolder(CString strFilePath)
{
	s_strExePath = StdFilePath(strFilePath);
	
	return TRUE;
}

// 获取安装目录
CString Util::Path::GetInstallFolder()
{
	ATLASSERT(!s_strExePath.IsEmpty());

	int ix = s_strExePath.ReverseFind(_T('\\'));
	if (ix != -1)
	{
		return s_strExePath.Mid(0, ix);
	}

	ATLASSERT(FALSE);
	return _T("");
}

// 获取主Exe所在目录
CString Util::Path::GetExeFolder()
{
	ATLASSERT(!s_strExePath.IsEmpty());

	return s_strExePath;
}

CString Util::Path::GetCfgFolder()
{
	return GetInstallFolder() + _T("\\Config");
}

CString Util::Path::GetLogFolder(BOOL bCreateIfNotExist/* = FALSE*/)
{
	CString strDir = GetDataFolder() + _T("\\Log");

	if (FALSE != bCreateIfNotExist)
	{
		ValidDirPath(strDir);
	}

	return strDir;
}

// 获取主Res所在目录
CString Util::Path::GetResFolder()
{
	return GetInstallFolder() + _T("\\Resource");
}

//获取声音文件目录
CString Util::Path::GetResSoundFolder()
{
	return GetResFolder() + _T("\\sound");
}

// 获取主Resource::XML所在目录
CString Util::Path::GetResXmlFolder()
{
	return GetResFolder() + _T("\\xml");
}

// 获取主Resource::Material所在目录
CString Util::Path::GetResMatFolder()
{
	return GetResFolder() + _T("\\Material");
}
CString Util::Path::GetResImageFolder()
{
	return GetResFolder() + _T("\\images");
}
CString Util::Path::GetResEmojiFolder()
{
	return GetResFolder() + _T("\\emoji\\unicode");
}
// 获取资源图片所在目录代码中用到的图片
CString Util::Path::GetResToolsFolder()
{
	return GetResFolder() + _T("\\Tools");
}

// 获取数据根目录
CString Util::Path::GetDataFolder(BOOL bCreateIfNotExist/* = FALSE*/)
{
	TCHAR szDocuments[MAX_PATH] = { 0 };
	MyGetFolderPath(CSIDL_PERSONAL, szDocuments);
	CString strDir;
	strDir.Format(_T("%s\\MetenTV"), szDocuments);

	if (FALSE != bCreateIfNotExist)
	{
		ValidDirPath(strDir);
	}

	return strDir;
}

// 获取Tmp数据根目录
COMMON_API CString Util::Path::GetDTmpFolder(BOOL bCreateIfNotExist/* = FALSE*/)
{
//	ATLASSERT(0 != s_uid);

	CString strDir;
	strDir.Format(_T("%s\\Temp"), GetDataFolder());

	if (FALSE != bCreateIfNotExist)
	{
		ValidDirPath(strDir);
	}

	return  strDir;
}

// 获取Url解析的缩略图片数据根目录
COMMON_API CString Util::Path::GetUrlThumbFolder(BOOL bCreateIfNotExist/* = FALSE*/)
{
	//	ATLASSERT(0 != s_uid);

	CString strDir;
	strDir.Format(_T("%s\\UrlThumb"), GetDTmpFolder());

	if (FALSE != bCreateIfNotExist)
	{
		ValidDirPath(strDir);
	}

	return  strDir;
}


//
CString Util::Path::GetCefCachePath(BOOL bCreateIfNotExist)
{
	CString strDir = GetDataFolder() + L"\\CefCachePath";
	if (FALSE != bCreateIfNotExist)
	{
		ValidDirPath(strDir);
	}
	return strDir;
}

CString Util::Path::GetCefUserPath(BOOL bCreateIfNotExist)
{
	CString strDir = GetDataFolder() + L"\\CefUserPath";
	if (FALSE != bCreateIfNotExist)
	{
		ValidDirPath(strDir);
	}
	return strDir;
}

size_t Util::Path::SplitString(LPCTSTR pszPath, LPCTSTR pszToken, std::vector<CString>& aFileNames)
{
	aFileNames.clear();

	int len = _tcslen(pszToken);
	if (0 == len)
	{
		ATLASSERT(FALSE);
		return 0;
	}

	CString strPath = pszPath;
	size_t length	=	strPath.GetLength();
	if (0 == length)
	{
		return 0;
	}

	int pos = -1;

	for(size_t i = 0; i < length; i++)
	{   
		pos = strPath.Find(pszToken, i);
		if (-1 != pos)
		{
			CString s = strPath.Mid(i, pos - i);
			if (0 != s.GetLength() && s != pszToken)
			{
				aFileNames.push_back(s);
			}
			i = pos + len - 1;
		}
		else
		{
			CString s = strPath.Mid(i, length - i);
			if (0 != s.GetLength() && s != pszToken)
			{
				aFileNames.push_back(s);
			}

			break;
		}
	}

	return aFileNames.size();
}

CString Util::Path::StandardFilePath(LPCTSTR pszFilePath)
{
	CString strPath = pszFilePath;
	if (strPath.GetLength() > 0)
	{
		if (_T('\\') == strPath[strPath.GetLength() - 1])
		{
			strPath = strPath.Left(strPath.GetLength() - 1);
		}
	}

	return strPath;
}

BOOL Util::Path::ExtractCurrentPath(LPCTSTR pszFilePath, CString& strCurFilePath, CString& strSubFilePath)
{
	strCurFilePath.Empty();
	strSubFilePath.Empty();

	CString strPath = pszFilePath;
	int pos = strPath.Find(_T('\\'));
	if (-1 != pos)
	{
		strCurFilePath = strPath.Mid(0, pos);

		if (pos + 1 < strPath.GetLength())
		{
			strSubFilePath = strPath.Mid(pos + 1, strPath.GetLength() - pos - 1);
		}
	}
	else
	{
		strCurFilePath = pszFilePath;
	}

	return TRUE;
}

CString Util::Path::GetParentDirPath(CString strFilePath)
{
	int ix = strFilePath.ReverseFind(_T('\\'));
	if (-1 != ix)
	{
		return strFilePath.Mid(0, ix);
	}

	return _T("");
}
CString Util::Path::GetParentDirName(CString strFilePath)
{
	int ix = strFilePath.ReverseFind(_T('\\'));
	if (-1 != ix)
	{
		return strFilePath.Mid(ix, strFilePath.GetLength());
	}
	return _T("");
}

BOOL Util::Path::ValidDirPath(CString strDirPath)
{
	CString strParentPath = GetParentDirPath(strDirPath);

	if (strParentPath.IsEmpty())
	{
		return FALSE;
	}
	if (!IsFileExist(strParentPath))
	{
		BOOL bResult = ValidDirPath(strParentPath);
		if (FALSE == bResult)
		{
			return FALSE;
		}
	}
	
	if (!IsFileExist(strDirPath))
	{
		BOOL bResult = CreateDirectory(strDirPath, NULL);
		if (FALSE == bResult)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL Util::Path::ValidFilePath(CString strFilePath)
{
	CString strDirPath = GetParentDirPath(strFilePath);
	
	return ValidDirPath(strDirPath);
}

BOOL Util::Path::IsFileExist(CString strPath)
{
	return PathFileExists(strPath);
}
BOOL Util::Path::IsFilePath(LPCTSTR strPath)
{
	if (!IsFileExist(strPath))
	{
		return FALSE;
	}
	if (PathIsDirectory(strPath))
	{
		return FALSE;
	}
	return TRUE;
}
BOOL Util::Path::GetFileTime(LPCTSTR szFilePath, SYSTEMTIME &stCreateTime, SYSTEMTIME &stModifyTime, SYSTEMTIME &stVisitTime)
{
	if (!IsFilePath(szFilePath))
	{
		return FALSE;
	}

	WIN32_FILE_ATTRIBUTE_DATA win32FileAttributeData;
	if (!GetFileAttributesEx(szFilePath, GetFileExInfoStandard, &win32FileAttributeData))
	{
		return FALSE;
	}

	//创建时间；
	FILETIME ftCreateLocalTime;
	FileTimeToLocalFileTime(&win32FileAttributeData.ftCreationTime, &ftCreateLocalTime);
	//修改时间；
	FILETIME ftModifyLocalTime;
	FileTimeToLocalFileTime(&win32FileAttributeData.ftLastWriteTime, &ftModifyLocalTime);
	//访问时间；
	FILETIME ftVisitLocalTime;
	FileTimeToLocalFileTime(&win32FileAttributeData.ftLastAccessTime, &ftVisitLocalTime);

	//创建时间转换成系统格式时间；
	FileTimeToSystemTime(&ftCreateLocalTime, &stCreateTime);
	//修改时间转换系统格式时间；
	FileTimeToSystemTime(&ftModifyLocalTime, &stModifyTime);
	//访问时间转换系统格式时间；
	FileTimeToSystemTime(&ftVisitLocalTime, &stVisitTime);
	return TRUE;
}

BOOL Util::Path::GetMyFilePath(CString strPath, vector<CString>&vecPath, vector<CString>&vecFolder, FileScanFlag flag /* = FileScanFlag::MULTIFOLDER */)
{
	if (strPath.IsEmpty())
		return FALSE;
	CString strFolder = strPath;
	if (IsFilePath(strFolder))
	{
		return FALSE;
	}
	strFolder += _T("\\*.*");
	WIN32_FIND_DATAW FindData;
	HANDLE hFind = FindFirstFile(strFolder, &FindData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	while (::FindNextFile(hFind, &FindData))
	{
		if (_tcsicmp(FindData.cFileName, _T(".")) == 0
			|| _tcsicmp(FindData.cFileName, _T("..")) == 0)
		{
			continue;
		}
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			CString strFileName(FindData.cFileName);
			CString strFolder = strPath + _T("\\") + strFileName;
			vecFolder.push_back(strFolder);
			if (SINGLEFOLDER == flag)
			{
				continue;
			}
			GetMyFilePath(strFolder, vecPath, vecFolder, flag);
		}
		else if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
		{
			CString strFilePath(FindData.cFileName);
			strFilePath = strPath + _T("\\") + strFilePath;
			vecPath.push_back(strFilePath);
		}
	}
	FindClose(hFind);
	return TRUE;
}

UINT64 Util::Path::GetFileLength(LPCTSTR szFileName)
{
	WIN32_FIND_DATA		rFileInfo = { 0 };
	LARGE_INTEGER		rFileSize = { 0 };
	
	HANDLE hFind = FindFirstFile(szFileName, &rFileInfo);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		rFileSize.LowPart = rFileInfo.nFileSizeLow;
		rFileSize.HighPart = rFileInfo.nFileSizeHigh;
		FindClose(hFind);
	}

	return rFileSize.QuadPart;
}

CString Util::Path::StdFilePath(CString strPath)
{
	// '/'替换为'\'
	strPath.Replace(_T('/'), _T('\\'));

	// 替换 'A\B\.\'的 ‘\B\’
	strPath.Replace(_T("\\.\\"), _T("\\"));

	// 去掉多余的'\'
	strPath.Replace(_T("\\\\"), _T("\\"));

	// 去掉 'A\B\..\'的 ‘\B\..’
	for (;;)
	{
		int iPos = strPath.Find(_T("\\..\\"));
		if (0 > iPos)
		{
			break;
		}

		//死循环?
		CString strSub = strPath.Left(iPos);
		int iPrev = strSub.ReverseFind(_T('\\'));
		if (iPrev < 0)
		{
			break;
		}

		strPath.Delete(iPrev, iPos - iPrev + _tcslen(_T("\\..")));
	}

	return strPath;
}

CString Util::Path::GetFileName(LPCTSTR szPath)
{
	CString strPath = szPath;
	int ix = strPath.ReverseFind(_T('\\'));
	if (ix != -1)
	{
		strPath = strPath.Right(strPath.GetLength() - ix - 1);
		ix = strPath.ReverseFind(_T('.'));
		if (ix != -1)
		{
			return strPath.Mid(0, ix);
		}
		return strPath.Right(strPath.GetLength() - ix);
	}
	ATLASSERT(FALSE);
	return _T("");
}
CString Util::Path::GetFileNameWithExt(LPCTSTR szPath)
{
	CString strPath = szPath;
	int ix = strPath.ReverseFind(_T('\\'));
	if (ix != -1)
	{
		strPath = strPath.Right(strPath.GetLength() - ix - 1);
		return strPath;
	}

	ATLASSERT(FALSE);
	return _T("");
}
CString Util::Path::GetFileExt(LPCTSTR szPath)
{
	CString strPath = szPath;
	int ix = strPath.ReverseFind(_T('\\'));   //找到最后一个
	if (ix != -1)
	{
		strPath = strPath.Right(strPath.GetLength() - ix - 1);
		ix = strPath.ReverseFind(_T('.'));
		if (-1 != ix)
			strPath = strPath.Right(strPath.GetLength() - ix);   //获取后缀
		else
			strPath = L"";             //没有后缀
		return strPath;
	}

	ATLASSERT(FALSE);
	return _T("");
}

UINT64 Util::Path::OpenFile(LPCTSTR strPath, BOOL bIsFolder)
{
	CString strParameters;
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	if (bIsFolder)
	{
		strParameters.Format(_T(" /select, %s"), strPath);
		ShExecInfo.lpFile = _T("explorer");
	}
	else
	{
		ShExecInfo.lpFile = strPath;
	}
	ShExecInfo.lpParameters = strParameters;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	if (ShellExecuteEx(&ShExecInfo))
	{
		return 0;
	}
	else
	{
		return GetLastError();
	}
}

// 打开目录
UINT64 Util::Path::OpenFolder(LPCTSTR strFolder)
{
	CString strParameters;
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;

	ShExecInfo.lpFile = _T("explorer");
	ShExecInfo.lpParameters = strFolder;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	if (ShellExecuteEx(&ShExecInfo))
	{
		return 0;
	}
	else
	{
		return GetLastError();
	}
}

//打开文件 或压缩包并选中
BOOL  Util::Path::OpenFileSelect(LPCTSTR strPath)
{
	if (IsDirectory(strPath))
	{
		return FALSE;
	}
	CString strFile(strPath);
	strFile = _T("/e,/select,") + strFile;

	HINSTANCE hResult = ShellExecute(NULL, _T("open"), _T("explorer.exe"), strFile, NULL, SW_SHOWNORMAL);
	if ((INT32)hResult <= 32)
	{
		return FALSE;
	}
	return TRUE;
}

//是否是目录
BOOL Util::Path::IsDirectory(LPCTSTR lpszPath)
{
	if (NULL == lpszPath || NULL == *lpszPath)
	{
		return FALSE;
	}

	DWORD dwAttr = ::GetFileAttributes(lpszPath);
	return (((dwAttr != 0xFFFFFFFF) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY)) ? TRUE : FALSE);
}


#define TIME_WAIT_PROCESS_EXIT	3000
BOOL Util::Path::RegisterLocalServer(LPCTSTR pszExePath)
{
	//		ECHub.exe /regserver
	//		regsvr32 Common.dll
	//		regsvr32 Service.dll
	TCHAR szCmd[1024];
	_sntprintf_s(szCmd, sizeof(szCmd), _T("\"%s\" /RegServer"), StdFilePath(pszExePath));

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	BOOL bRetVal = TRUE;
	if (CreateProcess(
		NULL,
		szCmd,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi))
	{
		CloseHandle(pi.hThread);
		//等待ECHub.exe注册自己完毕
		WaitForSingleObject(pi.hProcess, TIME_WAIT_PROCESS_EXIT);
		CloseHandle(pi.hProcess);
	}
	else
	{
		HRESULT hr = GetLastError();
		bRetVal = FALSE;
	}

	return bRetVal;
}

BOOL Util::Path::DllRegister(LPCTSTR pszDllName)
{
    BOOL bRetVal = FALSE;

    CString strPath = Util::Path::GetExeFolder() + _T("\\") + pszDllName;
    HINSTANCE hLib = LoadLibrary(strPath);
    if (hLib >= (HINSTANCE)HINSTANCE_ERROR)
    {
        FARPROC pfnDllRegisterServer = GetProcAddress(hLib, "DllRegisterServer");
        if (NULL != pfnDllRegisterServer)
        {
            HRESULT hr = (*pfnDllRegisterServer)();

            bRetVal = SUCCEEDED(hr);
        }
        else
        {
            ATLASSERT(FALSE);
        }
            FreeLibrary(hLib);
    }

    return bRetVal;
}

BOOL Util::Path::FixedTimeMsgLoop(UINT32 ms)
{
	DWORD  dwBegin = GetTickCount();

	MSG msg;
	while (TRUE)
	{
		DWORD  dwEnd = GetTickCount();
		if (dwBegin + ms <= dwEnd)
		{
			break;
		}

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				return FALSE;		
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Sleep(10);
		}
	}

	return TRUE;
}
//注意使用该函数获取命令行参数时，创建进程传的参数值和这里pszName字母大小要一致
BOOL Util::Path::GetCmdLineValue(vector<CString>& v, LPCTSTR pszName, CString& strValue)
{
	CString strName = pszName;  
	//strName.MakeLower();
	for (vector<CString>::iterator it = v.begin(); it != v.end(); it++) 
	{
		//it->MakeLower();
		int iPos = it->Find(strName);
		if (iPos >= 0)
		{
			iPos += strName.GetLength();
			if (iPos == it->GetLength())
			{
				strValue.Empty();		
			}
			else
			{
				strValue = it->Mid(iPos, it->GetLength() - iPos);
			}

			return TRUE;
		}
	}

	return FALSE;
}

HRESULT Util::Path::CreateObjectFromDllFile(LPCTSTR pszLib, IUnknown* pUnkOuter, REFCLSID rclsid, REFIID riid, LPVOID * ppv)
{
	*ppv = NULL;
	HRESULT hr = E_FAIL;
	HINSTANCE hInst = CoLoadLibrary((LPOLESTR)pszLib, TRUE);
    typedef HRESULT(__stdcall *GETCLASS_PROC)(REFCLSID, REFIID, LPVOID*);
	GETCLASS_PROC proc = (GETCLASS_PROC)GetProcAddress(hInst, "DllGetClassObject");
	if (proc)
	{
		IClassFactory* pfact;
		hr = proc(rclsid, IID_IClassFactory, (void**)&pfact);
		if (pfact)
		{
			hr = pfact->CreateInstance(pUnkOuter, riid, ppv);
			pfact->Release();
		}
	}
	return hr;
}

COMMON_API UINT64 Util::Path::GetFileModifyTime(LPCTSTR strPath)
{
	UINT64 uTime = 0;
	if (FALSE == IsFileExist(strPath))
	{
		return uTime;
	}
	HANDLE hFile = CreateFile(strPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return uTime;
	}

	FILETIME ftModify;
	if (FALSE != GetFileTime(hFile, NULL, NULL, &ftModify))
	{
		uTime = Util::TimeConvert::FileTimeToTime(ftModify);
	}
	CloseHandle(hFile);
	return uTime;
}

COMMON_API UINT64 Util::Path::GetFileCreateTime(LPCTSTR strPath)
{
	UINT64 uTime = 0;
	if (FALSE == IsFileExist(strPath))
	{
		return uTime;
	}
	HANDLE hFile = CreateFile(strPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return uTime;
	}

	FILETIME ftCreate;
	if (FALSE != GetFileTime(hFile, &ftCreate, NULL, NULL))
	{
		uTime = Util::TimeConvert::FileTimeToTime(ftCreate);
	}
	CloseHandle(hFile);
	return uTime;
}

COMMON_API UINT64 Util::Path::GetFileAccessTime(LPCTSTR strPath)
{
	UINT64 uTime = 0;
	if (FALSE == IsFileExist(strPath))
	{
		return uTime;
	}
	HANDLE hFile = CreateFile(strPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return uTime;
	}

	FILETIME ftAccess;
	if (FALSE != GetFileTime(hFile, NULL, &ftAccess, NULL))
	{
		uTime = Util::TimeConvert::FileTimeToTime(ftAccess);
	}
	CloseHandle(hFile);
	return uTime;
}

COMMON_API CString Util::Path::GetAppReportFolder(BOOL bCreateIfNotExist /*= FALSE*/)
{

	CString strDir = GetDTmpFolder(bCreateIfNotExist) + _T("\\Report");

	if(FALSE != bCreateIfNotExist)
	{
		ValidDirPath(strDir);
	}

	return strDir;
}


//获取指定系统目录
BOOL Util::Path::MyGetFolderPath(int csidl, LPTSTR pszPath)
{
	BOOL bRet = SHGetSpecialFolderPath(NULL, pszPath, csidl, FALSE);
	//Util::Log::Info(_T("MyGetFolderPath"), _T("[Start] Path=[%s] ret=%d\r\n"), pszPath, bRet);
	if (_tcsstr(pszPath, _T("systemprofile")) != NULL)
	{
		HKEY hKey = NULL;
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), 0, KEY_READ, &hKey))
		{
			DWORD dwData = MAX_PATH - 1;
			DWORD dwType = REG_SZ;
			TCHAR szPath[MAX_PATH] = { 0 };
			if (CSIDL_PERSONAL == csidl)
			{
				if (ERROR_SUCCESS == RegQueryValueEx(hKey, _T("Personal"), NULL, &dwType, (LPBYTE)szPath, &dwData))
				{
					//Util::Log::Info(_T("MyGetFolderPath"), _T("[RegQueryValueEx] Path=[%s]\r\n"), szPath);
					lstrcpy(pszPath, szPath);
					bRet = TRUE;
				}
				else
				{
					//Util::Log::Info(_T("MyGetFolderPath"), _T("[RegQueryValueEx] LastError=%ld\r\n"), GetLastError());
				}
			}
			RegCloseKey(hKey);
		}
		else
		{
			//Util::Log::Info(_T("MyGetFolderPath"), _T("[RegOpenKeyEx] LastError=%ld\r\n"), GetLastError());
		}
	}
	return bRet;
}

//删除文件或者文件夹(文件夹下所有文件都会删除)
BOOL Util::Path::DeleteFolder(LPCTSTR pszPath, BOOL bRecycle)
{
	int				  nRet = 0;
	int				  nLen = 0;
	int				  nBufLen = 0;
	TCHAR *			  szBuffer = NULL;
	SHFILEOPSTRUCT	  rFileOp = { 0 };


	ATLASSERT(pszPath != NULL);
	nLen = _tcslen(pszPath);
	nBufLen = sizeof(TCHAR)* (nLen + 2);
	szBuffer = (TCHAR *)malloc(nBufLen);
	if (szBuffer == NULL) return false;
	_tcscpy_s(szBuffer, nLen + 2, pszPath);
	szBuffer[nLen] = '\0';
	szBuffer[nLen + 1] = '\0';//必须要加两个'\0'不然SHFileOperation会报错

    if (TRUE == bRecycle) rFileOp.fFlags = FOF_ALLOWUNDO;
	//不出现确认对话框  //不出现错误对话框 // 不显示进度条提示框
	rFileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	rFileOp.pFrom = szBuffer;
	rFileOp.pTo = NULL;      //一定要是NULL
	rFileOp.wFunc = FO_DELETE;    //删除操作
	nRet = SHFileOperation(&rFileOp);
    free(szBuffer);
	ATLASSERT(nRet == 0);
	return nRet == 0;
}


CString Util::Path::GetCefResPath()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString strPath(szPath);
	INT32 iFind = strPath.ReverseFind(_T('\\'));
	if (iFind < 0)
	{
		return _T("");
	}
	strPath = strPath.Left(iFind);
	ATLASSERT(!strPath.IsEmpty());

	INT32 ix = strPath.ReverseFind(_T('\\'));
	if (ix != -1)
	{
		return strPath.Mid(0, ix) + _T("\\CefRes");
	}
	ATLASSERT(FALSE);
	return _T("");
}


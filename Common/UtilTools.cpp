#include "stdafx.h"
#include "UtilTools.h"
#include  <httpext.h>  
#include  <windef.h>  
#include  <Nb30.h>
#include <time.h>
#include <Psapi.h>
#include <shlobj.h>   
#include <conio.h> 
#include <tlhelp32.h> 
#include "UtilLog.h"
#include "UtilPath.h"
#include "UtilString.h"
#include "MD5Helper.h"

#pragma comment(lib,"Netapi32.lib")

INT32 Util::Tools::GetMAC(UINT64 *mac)
{
	NCB ncb;
	typedef struct _ASTAT_
	{
		ADAPTER_STATUS  adapt;
		NAME_BUFFER  NameBuff[30];
	}ASTAT, *PASTAT;
	ASTAT Adapter;

	typedef struct _LANA_ENUM
	{  //  le 
		UCHAR  length;
		UCHAR  lana[MAX_LANA];
	}LANA_ENUM;
	LANA_ENUM lana_enum;

	UCHAR uRetCode;
	memset(&ncb, 0, sizeof(ncb));
	memset(&lana_enum, 0, sizeof(lana_enum));

	ncb.ncb_command = NCBENUM;
	ncb.ncb_buffer = (unsigned char *)&lana_enum;
	ncb.ncb_length = sizeof(LANA_ENUM);
	uRetCode = Netbios(&ncb);
	if (uRetCode != NRC_GOODRET)
		return uRetCode;

	for (int lana = 0; lana < lana_enum.length; lana++)
	{
		ncb.ncb_command = NCBRESET;
		ncb.ncb_lana_num = lana_enum.lana[lana];
		uRetCode = Netbios(&ncb);
		if (uRetCode == NRC_GOODRET)
			break;
	}
	if (uRetCode != NRC_GOODRET)
		return uRetCode;

	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = lana_enum.lana[0];
	strcpy_s((char*)ncb.ncb_callname, 2, "*");
	ncb.ncb_buffer = (unsigned char *)&Adapter;
	ncb.ncb_length = sizeof(Adapter);
	uRetCode = Netbios(&ncb);
	if (uRetCode != NRC_GOODRET)
		return uRetCode;
	unsigned char *p = (unsigned char *)mac;
	*p = Adapter.adapt.adapter_address[0];
	*p++ = Adapter.adapt.adapter_address[1];
	*p++ = Adapter.adapt.adapter_address[2];
	*p++ = Adapter.adapt.adapter_address[3];
	*p++ = Adapter.adapt.adapter_address[4];
	*p++ = Adapter.adapt.adapter_address[5];
	return 0;
}

INT32 Util::Tools::GetMAC(char *pMac)
{
	NCB ncb;
	typedef struct _ASTAT_
	{
		ADAPTER_STATUS  adapt;
		NAME_BUFFER  NameBuff[30];
	}ASTAT, *PASTAT;
	ASTAT Adapter;

	typedef struct _LANA_ENUM
	{  //  le 
		UCHAR  length;
		UCHAR  lana[MAX_LANA];
	}LANA_ENUM;
	LANA_ENUM lana_enum;

	UCHAR uRetCode;
	memset(&ncb, 0, sizeof(ncb));
	memset(&lana_enum, 0, sizeof(lana_enum));

	ncb.ncb_command = NCBENUM;
	ncb.ncb_buffer = (unsigned char *)&lana_enum;
	ncb.ncb_length = sizeof(LANA_ENUM);
	uRetCode = Netbios(&ncb);
	if (uRetCode != NRC_GOODRET)
		return uRetCode;

	for (int lana = 0; lana < lana_enum.length; lana++)
	{
		ncb.ncb_command = NCBRESET;
		ncb.ncb_lana_num = lana_enum.lana[lana];
		uRetCode = Netbios(&ncb);
		if (uRetCode == NRC_GOODRET)
			break;
	}
	if (uRetCode != NRC_GOODRET)
		return uRetCode;

	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = lana_enum.lana[0];
	strcpy_s((char*)ncb.ncb_callname, 2, "*");
	ncb.ncb_buffer = (unsigned char *)&Adapter;
	ncb.ncb_length = sizeof(Adapter);
	uRetCode = Netbios(&ncb);
	if (uRetCode != NRC_GOODRET)
		return uRetCode;
	sprintf_s(pMac,  18, "%02X-%02X-%02X-%02X-%02X-%02X",
		Adapter.adapt.adapter_address[0],
		Adapter.adapt.adapter_address[1],
		Adapter.adapt.adapter_address[2],
		Adapter.adapt.adapter_address[3],
		Adapter.adapt.adapter_address[4],
		Adapter.adapt.adapter_address[5]
		);
	return 0;
}

UINT64 Util::Tools::GetDiskSpaceTotalSize(CString strDisk)
{
	UINT64 uSectorsPerCluster = 0;//每簇中扇区数
	UINT64 uBytesPerSector = 0;//每扇区中字节数
	UINT64 uFreeClusters = 0;//剩余簇数
	UINT64 uTotalClusters = 0;//总簇数
	UINT64 uDiskSize = 0;
	if (GetDiskFreeSpace(strDisk, (LPDWORD)&uSectorsPerCluster, (LPDWORD)&uBytesPerSector,
		(LPDWORD)&uFreeClusters, (LPDWORD)&uTotalClusters))
	{
		uDiskSize = uTotalClusters*uSectorsPerCluster*uBytesPerSector;
		return uDiskSize;
	}
	return 0;
}

UINT64 Util::Tools::GetDiskSpaceFreeSize(CString strDisk)
{
	UINT64 uSectorsPerCluster = 0;//每簇中扇区数
	UINT64 uBytesPerSector = 0;//每扇区中字节数
	UINT64 uFreeClusters = 0;//剩余簇数
	UINT64 uTotalClusters = 0;//总簇数
	UINT64 uDiskSize = 0;
	if (GetDiskFreeSpace(strDisk, (LPDWORD)&uSectorsPerCluster, (LPDWORD)&uBytesPerSector,
		(LPDWORD)&uFreeClusters, (LPDWORD)&uTotalClusters))
	{
		uDiskSize = uFreeClusters*uSectorsPerCluster*uBytesPerSector;
		return uDiskSize;
	}
	return 0;
}

void Util::Tools::SystemTimeToStringTime(SYSTEMTIME st, CString &strTime)
{
	strTime.Format(_T("%4d年-%2d月-%2d日 %2d:%2d:%2d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

INT64 Util::Tools::SystemTimeToI64Time(SYSTEMTIME st)
{
	struct tm tm_ = { st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth - 1, st.wYear - 1900, st.wDayOfWeek, 0, 0 };
	return mktime(&tm_);
}

time_t Util::Tools::StringToDatetime(const TCHAR *strTime)
{
	if (NULL == strTime)
	{
		return 0;
	}

	CStringA straTime;
	Util::String::W_2_Utf8(strTime, straTime);
	tm tm_;
	int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
	sscanf_s(straTime.GetString(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	tm_.tm_year = year - 1900;
	tm_.tm_mon = month - 1;
	tm_.tm_mday = day;
	tm_.tm_hour = hour;
	tm_.tm_min = minute;
	tm_.tm_sec = second;
	tm_.tm_isdst = 0;
	time_t t_ = mktime(&tm_); //已经减了8个时区       return t_; //秒时间
	return t_;
}

// 系统是否是vista或以上
BOOL Util::Tools::IsVistaOrLater()
{
	OSVERSIONINFOEX version = { sizeof(OSVERSIONINFOEX) };
	if (!GetVersionEx((LPOSVERSIONINFO)&version))
	{
		version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx((LPOSVERSIONINFO)&version))
		{
			return FALSE;
		}
	}
	return (version.dwMajorVersion >= 6);
}

CString Util::Tools::DataUnitToStr(UINT64 u32Size)
{
	CString strSize;
	if (0 < u32Size / 1024 / 1024 / 1024)
	{
		double dDiv = (double)u32Size / 1024 / 1024 / 1024;
		strSize.Format(_T("%.2f GB"), dDiv);
	}
	else if (0 < u32Size / 1024 / 1024)
	{
		double dDiv = (double)u32Size / 1024 / 1024;
		strSize.Format(_T("%.2f MB"), dDiv);
	}
	else if (0 < u32Size / 1024)
	{
		double dDiv = (double)u32Size / 1024;
		strSize.Format(_T("%.2f KB"), dDiv);
	}
	else
	{
		strSize.Format(_T("%u B"), u32Size);
	}
	return strSize;
}

CString  Util::Tools::GetScreenResolution()
{
	CString strRestlt;
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	strRestlt.Format(_T("%d*%d"), width, height);
	return strRestlt;
}

CString Util::Tools::GetSystemVer()
{
	CString vname;
	SYSTEM_INFO info;                //用SYSTEM_INFO结构判断64位AMD处理器
	GetSystemInfo(&info);            //调用GetSystemInfo函数填充结构
	OSVERSIONINFOEX  os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (FALSE == GetVersionEx((OSVERSIONINFO*)&os))
		return _T("");
	switch (os.dwMajorVersion)
	{                        //判断主版本号
		case 4:
			switch (os.dwMinorVersion)
			{                //判断次版本号
			case 0:
				if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
					vname = _T("Microsoft Windows NT 4.0");  //1996年7月发布
				else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
					vname = _T("Microsoft Windows 95");
				break;
			case 10:
				vname = _T("Microsoft Windows 98");
				break;
			case 90:
				vname = _T("Microsoft Windows Me");
				break;
			}
			break;
		case 5:
			switch (os.dwMinorVersion)
			{              
			case 0:
				vname = _T("Microsoft Windows 2000");    //1999年12月发布
				break;
			case 1:
				vname = _T("Microsoft Windows XP");      //2001年8月发布
				break;
			case 2:
				if (os.wProductType == VER_NT_WORKSTATION &&
					info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					vname = _T("Microsoft Windows XP Professional x64 Edition");
				else if (GetSystemMetrics(SM_SERVERR2) == 0)
					vname = _T("Microsoft Windows Server 2003");   //2003年3月发布
				else if (GetSystemMetrics(SM_SERVERR2) != 0)
					vname = _T("Microsoft Windows Server 2003 R2");
				break;
			}
			break;
		case 6:
			switch (os.dwMinorVersion)
			{
			case 0:
				if (os.wProductType == VER_NT_WORKSTATION)
					vname = _T("Microsoft Windows Vista");
				else
					vname = _T("Microsoft Windows Server 2008");   //服务器版本
				break;
			case 1:
				if (os.wProductType == VER_NT_WORKSTATION)
					vname = _T("Microsoft Windows 7");
				else
					vname = _T("Microsoft Windows Server 2008 R2");
				break;
			case 2:
				vname = _T("Microsoft Windows 8");
				break;
			}
			break;
		default:
			vname = _T("");
		}
	return vname;
}
CString Util::Tools::GetSystemMemory()
{
	MEMORYSTATUSEX memStatex;
	memStatex.dwLength = sizeof(memStatex);
	if (GlobalMemoryStatusEx(&memStatex))
	{
		UINT64 uMemorySize = memStatex.ullTotalPhys;
		return DataUnitToStr(uMemorySize);
	}
	return _T("");
}

CString Util::Tools::GetCurrentIme()
{
	CString strRes;
	return strRes;
}
UINT32 Util::Tools::GetECUsedMemory()
{
	UINT32 uSize = 0;
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	if(TRUE==GetProcessMemoryInfo(handle, &pmc, sizeof(pmc)))    //依赖Psapi.lib  库
		uSize= pmc.WorkingSetSize;
	return uSize;
}

UINT32 Util::Tools::GetUsedGDICount()
{
	UINT32 uCount = 0;
	HANDLE ecHandle = GetCurrentProcess();
	uCount = GetGuiResources(ecHandle, GR_GDIOBJECTS);
	return uCount;
}

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;
INT32 Util::Tools::GetSystemBits()
{
	INT32 n32Rsult = -1;
	BOOL bIsWow64 = FALSE;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			return n32Rsult;
		}
	}
	if (TRUE == bIsWow64)
	{
		n32Rsult = 64;
	}
	else
	{
		n32Rsult = 32;
	}
	return n32Rsult;
}

CString Util::Tools::GetHidePhoneNumber(const CString& strNumber)
{
	CString strHideNum = strNumber;
	int nSize = strHideNum.GetLength();

	for(int i = 0; i < nSize; i++)
	{
		if(i >= 3 && i <= 6)
		{
			strHideNum.SetAt(i, _T('*'));
		}
	}

	return strHideNum;
}

/*
函数功	能：进程是否存在
函数参	数：csProcessName:进程名
函数返回值：存在返回进程ID,不存在返回0
*/
DWORD Util::Process::IsProcessExist(CString csProcessName)
{
	DWORD dwProcessID = 0;
	PROCESSENTRY32 processInfo = { 0 };
    csProcessName.MakeLower();
	do
	{
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hSnapShot)
		{
			break;
		}

		processInfo.dwSize = sizeof(PROCESSENTRY32);			 //??ProcessInfo??? 
		BOOL Status = Process32First(hSnapShot, &processInfo);   //????????????? 
		while (Status)
		{
            CString strExeFile = processInfo.szExeFile;
            strExeFile.MakeLower();
            if (0 == lstrcmpi(strExeFile, csProcessName))
			{
				dwProcessID = processInfo.th32ProcessID;
				break;
			}
			Status = Process32Next(hSnapShot, &processInfo);
		}

		CloseHandle(hSnapShot);
	} while (FALSE);

	return dwProcessID;
}

/*
函数功	能：创建进程
函数参	数：csProcessPath:可执行文件路径
函数返回值：创建成功返回TRUE,否则返回FALSE
*/
BOOL Util::Process::CreateProcessEx(CString csProcessPath)
{
	STARTUPINFO sinfo = { sizeof(sinfo) };
	PROCESS_INFORMATION pinfo = { 0 };
	sinfo.dwFlags = STARTF_USESHOWWINDOW;
	sinfo.wShowWindow = SW_SHOWNORMAL;

	if (CreateProcess(
		NULL,
		(LPTSTR)(LPCTSTR)csProcessPath, //在Unicode版本中此参数不能为常量字符串，因为此参数会被修改 
		NULL,
		NULL,
		FALSE,
		NULL,
		NULL,
		NULL,
		&sinfo,
		&pinfo))
	{
		CloseHandle(pinfo.hProcess);
		CloseHandle(pinfo.hThread);
		return TRUE;
	}
	return FALSE;
}

/*
函数功	能：进程是否存在
函数参	数：csProcessName:进程名
函数返回值：存在返回进程ID,不存在返回0
*/
BOOL Util::Process::KillProcess(DWORD pID)
{
	HANDLE hProcess;
	hProcess = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, 0, pID);
	if (hProcess)
	{
		return TerminateProcess(hProcess, 0);
	}
	return FALSE;
}

static BOOL OpenURLEx(CString strURL)
{
    SHELLEXECUTEINFO shelli = {0};
    TCHAR szIexplore[MAX_PATH] = { 0 };


    ::SHGetSpecialFolderPath(NULL, szIexplore, CSIDL_PROGRAM_FILES, FALSE);
    ::PathStripToRoot(szIexplore);
    lstrcat(szIexplore, _T("Program Files\\Internet Explorer\\iexplore.exe"));

    shelli.cbSize = sizeof(SHELLEXECUTEINFO);
    shelli.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
    shelli.hwnd = NULL;
    shelli.lpVerb = _T("open");
    shelli.lpFile = szIexplore;
    shelli.lpParameters = strURL;
    shelli.lpDirectory = NULL;
    shelli.nShow = SW_SHOWNORMAL;
    if (ShellExecuteEx(&shelli))
    {
        return TRUE;
    }
    return FALSE;
}

void Util::Process::OpenPage(CString strURL)
{
    UINT32 u32StartTimer = GetTickCount();
	HINSTANCE  hResult = ShellExecute(NULL, _T("open"), strURL, NULL, NULL, SW_SHOWNORMAL);
	if ((INT32)hResult <= 32)
	{
		Util::Log::Info(_T("Util::Process::OpenPage[Fail][ShellExecute]"), _T(":strURL =%s"), strURL);
		if (FALSE == OpenURLEx(strURL))
		{
			CString strError;
			strError.Format(_T("%u"), GetLastError());
			Util::Log::Info(_T("Util::Process::OpenPage[Fail][OpenURLEx]"), _T(":GetLastError = %s,strURL =%s"), strError, strURL);
		}
	}
    UINT32 u32EndTimer = GetTickCount();
    Util::Log::Info(_T("Util::Process::OpenPage "), _T("Run Timer = %u start = %u end = %u"), u32EndTimer - u32StartTimer, u32StartTimer, u32EndTimer);
}

//获取当前进程内存占用情况（单位KB）
UINT32 Util::Process::GetCurProcMemUser()
{
    HANDLE hProcess = ::GetCurrentProcess();
    if (hProcess == NULL)
    {
        return 0;
    }

    PROCESS_MEMORY_COUNTERS pmc = { 0 };
    BOOL bRet = ::GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));
    if (!bRet)
    {
        return 0;
    }
    return pmc.WorkingSetSize >> 10;
}

HWND Util::Process::GetWindowsHandleByPid(DWORD pID, LPCTSTR pStrName)
{
	HWND hfindWindow = NULL;
	HWND hNextWindow = NULL;
	DWORD dwProcessId;
	while (TRUE)
	{
		hfindWindow = ::FindWindowEx(NULL, hfindWindow, NULL, pStrName);
		if (!hfindWindow)
		{
			break;
		}

		GetWindowThreadProcessId(hfindWindow, &dwProcessId);
		if (dwProcessId == pID)
		{
			return hfindWindow;
		}
	}
	return NULL;
}


UINT32 Util::Hash::Hash(const TCHAR* str)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;
	while (*str)
	{
		hash = hash * seed + (*str++);
	}
	return (hash & 0x7FFFFFFF);
}
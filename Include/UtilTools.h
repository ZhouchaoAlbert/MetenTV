#pragma once

#include "BaseDefine.h"
#include <atlstr.h>
using namespace ATL;
namespace Util
{
	namespace Tools
	{
		//获取Mac地址；
		COMMON_API INT32 GetMAC(UINT64 *mac);
		COMMON_API INT32 GetMAC(char *pMac);
		//获取磁盘空间大小(返回字节数)
		COMMON_API UINT64 GetDiskSpaceTotalSize(CString strDisk);
		COMMON_API UINT64 GetDiskSpaceFreeSize(CString strDisk);
		//系统时间转成字符串时间(xxxx年-xx月-xx日 xx:xx:xx） (xx:xx:xx  时 分 秒)
		COMMON_API void SystemTimeToStringTime(SYSTEMTIME st, CString &strTime);
		//系统时间转成整型;
		COMMON_API INT64 SystemTimeToI64Time(SYSTEMTIME st);
		//字节数转字符串(带单位)
		COMMON_API CString DataUnitToStr(UINT64 u32Size);
		//字符串日期转换成时间戳
		COMMON_API time_t StringToDatetime(const TCHAR *strTime);
		//获取屏幕的分辨率
		COMMON_API CString  GetScreenResolution();

		//获取操作系统的版本号
		COMMON_API CString GetSystemVer();

		//获取当前操作系统的 当前内存 (注意这里是操作系统识别出来内存大小)
		COMMON_API  CString GetSystemMemory();

		//获取EC 当前的内存使用量
		COMMON_API  UINT32 GetECUsedMemory();

		//获取当前系统使用的输入法
		COMMON_API CString GetCurrentIme();

		//获取EC当前使用的GDI数量
		COMMON_API UINT32 GetUsedGDICount();

		//获取系统是32bit或64bit，返回32表示32bit,返回64表示64bit,返回-1，表示失败；
		COMMON_API INT32 GetSystemBits();

		COMMON_API CString GetHidePhoneNumber(const CString& strNumber);
	
		//时间转成整数 (xx-xx-xx xx:xx:xx)
		COMMON_API time_t StringToDatetime(const TCHAR *strTime);

		// 系统是否是vista或以上
		COMMON_API BOOL IsVistaOrLater();
	}
	namespace Process
	{
		//判断进程是否存在；
		COMMON_API DWORD IsProcessExist(CString csProcessName);
		//创建进程
		COMMON_API BOOL CreateProcessEx(CString csProcessPath);
		//通过进程ID结束进程
		COMMON_API BOOL KillProcess(DWORD pID);
		//打开URL
		COMMON_API void  OpenPage(CString strURL); 
        //获取当前进程内存占用情况（单位KB）
        COMMON_API UINT32 GetCurProcMemUser();
		//通过进程ID获取窗口句柄
		COMMON_API HWND GetWindowsHandleByPid(DWORD pID, LPCTSTR pStrName);
	}

	namespace Hash
	{
		COMMON_API UINT32 Hash(const TCHAR* str);
	}
}
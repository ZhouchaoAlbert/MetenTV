#include "stdafx.h"
#include "UtilLog.h"
#include "atlstr.h"
#include "Singleton.h"
#include "LogNew.h"

using namespace ATL;

//如果多线程同步，严格的说应该在此作入口判断，但一般我们在进程开始就进行了log初始化，也就是对象肯定存在，所以我们退回到logmgr做同步

void Util::Log::SetLevel(EnumRawLogLevel eLevel)
{
	CLogNew::GetObject()->SetLevel(eLevel);
}

void Util::Log::Debug(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...)
{
	va_list va = (va_list)(&pchFmt + 1);
	CLogNew::GetObject()->Write(E_RLL_DEBUG, pchKeyword, pchFmt, va);
}

void Util::Log::Info(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...)
{
	va_list va = (va_list)(&pchFmt + 1);
	CLogNew::GetObject()->Write(E_RLL_INFO, pchKeyword, pchFmt, va);
}

void Util::Log::Warn(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...)
{
	va_list va = (va_list)(&pchFmt + 1);
	CLogNew::GetObject()->Write(E_RLL_WARN, pchKeyword, pchFmt, va);
}

void Util::Log::Error(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...)
{
	va_list va = (va_list)(&pchFmt + 1);
	CLogNew::GetObject()->Write(E_RLL_ERROR, pchKeyword, pchFmt, va);
}

void Util::Log::Fatal(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...)
{
	va_list va = (va_list)(&pchFmt + 1);
	CLogNew::GetObject()->Write(E_RLL_FATAL, pchKeyword, pchFmt, va);
}

BOOL Util::Log::Change(LPCTSTR pchFilePath, UINT32 u32UID)
{
	return CLogNew::GetObject()->Change(pchFilePath, u32UID);
}

BOOL Util::Log::Init()
{
	return CLogNew::GetObject()->Init();
}

void Util::Log::UnInit()
{
	CLogNew::GetObject()->UnInit();
}

UINT32 Util::LogConfig::GetLogLevel(LPCTSTR pchFilePath, LPCTSTR pchAppName, LPCTSTR pchKeyName)
{
	//获取本地配置文件中LogLevel；
	UINT32 u32Level = 0;
	TCHAR szLogLevel[64];
	GetPrivateProfileString(pchAppName, pchKeyName, _T(""), szLogLevel, sizeof(szLogLevel), pchFilePath);
	CString strLogLevel(szLogLevel);
	if (0 == strLogLevel.CompareNoCase(_T("DEBUG")))
	{
		u32Level = Util::Log::E_RLL_DEBUG;
	}
	else if (0 == strLogLevel.CompareNoCase(_T("INFO")))
	{
		u32Level = Util::Log::E_RLL_INFO;
	}
	else if (0 == strLogLevel.CompareNoCase(_T("WARN")))
	{
		u32Level = Util::Log::E_RLL_WARN;
	}
	else if (0 == strLogLevel.CompareNoCase(_T("ERROR")))
	{
		u32Level = Util::Log::E_RLL_ERROR;
	}
	else if (0 == strLogLevel.CompareNoCase(_T("FATAL")))
	{
		u32Level = Util::Log::E_RLL_FATAL;
	}
	else if (0 == strLogLevel.CompareNoCase(_T("SYSTEM")))
	{
		u32Level = Util::Log::E_RLL_SYSTEM;
	}
	else
	{
		u32Level = Util::Log::E_RLL_INFO;
	}
	return u32Level;
}

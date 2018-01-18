#include "StdAfx.h"
#include "UtilTime.h"

SYSTEMTIME Util::TimeConvert::TimeToSystemTime(UINT64 uTime)
{
	FILETIME fileTime;
	SYSTEMTIME sysTime;
	INT64 nTmp = Int32x32To64(uTime, 10000000) + 116444736000000000;
	fileTime.dwLowDateTime = (DWORD)nTmp;
	fileTime.dwHighDateTime = (DWORD)(nTmp >> 32);
	FileTimeToSystemTime(&fileTime, &sysTime);
	return sysTime;
}

UINT64 Util::TimeConvert::SystemTimeToTime(SYSTEMTIME sysTime)
{
	FILETIME fileTime;
	SystemTimeToFileTime(&sysTime, &fileTime);

	return FileTimeToTime(fileTime);
}

UINT64 Util::TimeConvert::FileTimeToTime(FILETIME fileTime)
{
	INT64 nTmp = 0;
	ULARGE_INTEGER ui;
	ui.LowPart = fileTime.dwLowDateTime;
	ui.HighPart = fileTime.dwHighDateTime;
	nTmp = fileTime.dwHighDateTime;
	nTmp = (nTmp << 32) + fileTime.dwLowDateTime;
//	nTmp = (INT64)(fileTime.dwHighDateTime << 32) + fileTime.dwLowDateTime;
	return (UINT64)((INT64)(ui.QuadPart - 116444736000000000) / 10000000);
}

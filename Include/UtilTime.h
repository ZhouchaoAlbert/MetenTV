#pragma once
#include "BaseDefine.h"

namespace Util
{
	namespace TimeConvert
	{
		COMMON_API SYSTEMTIME TimeToSystemTime(UINT64 uTime);

		COMMON_API UINT64 SystemTimeToTime(SYSTEMTIME sysTime);

		COMMON_API UINT64 FileTimeToTime(FILETIME fileTime);
	}
}
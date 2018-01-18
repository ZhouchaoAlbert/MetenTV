#pragma once

#include "BaseDefine.h"

//请每个进程注意设置Change文件路径和UID，默认将会使用当前EXE的上一级目录的Log子目录保存日志

namespace Util
{
	//仅仅支持主线程调用，请注意：主线程最好使用这种接口，因为效率高
	namespace Log
	{
		enum EnumRawLogLevel
		{
			E_RLL_NONE      = 0,
			E_RLL_DEBUG     = 1,
			E_RLL_INFO      = 2,
			E_RLL_WARN      = 3,
			E_RLL_ERROR     = 4,
			E_RLL_FATAL     = 5,
			E_RLL_SYSTEM    = 6,	//log本身需要的记录级别
		};
		//设置等级	
		COMMON_API void SetLevel(EnumRawLogLevel eLevel);
		COMMON_API void Debug(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...);
		COMMON_API void Info(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...);
		COMMON_API void Warn(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...);
		COMMON_API void Error(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...);
		COMMON_API void Fatal(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...);
		//pszFilePath可以为空
		COMMON_API BOOL Change(LPCTSTR pchFilePath, UINT32 u32UID);
		COMMON_API BOOL Init();
		COMMON_API void UnInit();
	}
	namespace LogConfig
	{
		//从配置文件中获取日志打印等级
		COMMON_API UINT32 GetLogLevel(LPCTSTR pchFilePath, LPCTSTR pchAppName, LPCTSTR pchKeyName);
	}
}



#pragma once

#include "BaseDefine.h"

//��ÿ������ע������Change�ļ�·����UID��Ĭ�Ͻ���ʹ�õ�ǰEXE����һ��Ŀ¼��Log��Ŀ¼������־

namespace Util
{
	//����֧�����̵߳��ã���ע�⣺���߳����ʹ�����ֽӿڣ���ΪЧ�ʸ�
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
			E_RLL_SYSTEM    = 6,	//log������Ҫ�ļ�¼����
		};
		//���õȼ�	
		COMMON_API void SetLevel(EnumRawLogLevel eLevel);
		COMMON_API void Debug(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...);
		COMMON_API void Info(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...);
		COMMON_API void Warn(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...);
		COMMON_API void Error(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...);
		COMMON_API void Fatal(LPCTSTR pchKeyword, LPCTSTR pchFmt, ...);
		//pszFilePath����Ϊ��
		COMMON_API BOOL Change(LPCTSTR pchFilePath, UINT32 u32UID);
		COMMON_API BOOL Init();
		COMMON_API void UnInit();
	}
	namespace LogConfig
	{
		//�������ļ��л�ȡ��־��ӡ�ȼ�
		COMMON_API UINT32 GetLogLevel(LPCTSTR pchFilePath, LPCTSTR pchAppName, LPCTSTR pchKeyName);
	}
}



#pragma once
#include "BaseDefine.h"
#include <atlstr.h>

#define ATTACHPASSWORDSTRING _T("jhdtybvxweimjhytrdgjkopqazmnlpoi")

using namespace ATL;

namespace Util
{
	namespace MD5
	{
		//param:strFile 需要获取的文件全路径   strMD5Value如果获取成功将存储文件的MD5
		//return:TRUE 获取成功   FALSE 获取失败
		COMMON_API BOOL GetFileMD5Value(const CString &strFile, CString &strMD5Value);

		//缓存区转32位md5字符串
		COMMON_API CString BufferToMD5(const BYTE * buffer, UINT nLen);

		COMMON_API BOOL BufferToMD5(const BYTE * buffer, UINT nLen, UINT8 *buf, UINT16 buflen);
	}

	namespace Storage
	{
		COMMON_API CString CreateDBPassWord(LPCTSTR pNormalStr, LPCTSTR pAttchStr = ATTACHPASSWORDSTRING);
	}
}


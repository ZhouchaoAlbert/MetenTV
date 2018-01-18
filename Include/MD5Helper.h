#pragma once
#include "BaseDefine.h"
#include <atlstr.h>

#define ATTACHPASSWORDSTRING _T("jhdtybvxweimjhytrdgjkopqazmnlpoi")

using namespace ATL;

namespace Util
{
	namespace MD5
	{
		//param:strFile ��Ҫ��ȡ���ļ�ȫ·��   strMD5Value�����ȡ�ɹ����洢�ļ���MD5
		//return:TRUE ��ȡ�ɹ�   FALSE ��ȡʧ��
		COMMON_API BOOL GetFileMD5Value(const CString &strFile, CString &strMD5Value);

		//������ת32λmd5�ַ���
		COMMON_API CString BufferToMD5(const BYTE * buffer, UINT nLen);

		COMMON_API BOOL BufferToMD5(const BYTE * buffer, UINT nLen, UINT8 *buf, UINT16 buflen);
	}

	namespace Storage
	{
		COMMON_API CString CreateDBPassWord(LPCTSTR pNormalStr, LPCTSTR pAttchStr = ATTACHPASSWORDSTRING);
	}
}


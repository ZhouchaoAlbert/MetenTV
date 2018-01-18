#include "stdafx.h"
#include "DownLoadHelper.h"

BOOL IsSignedUrl(CString strUrl, CString& strObject, CString& strBucket)
{
	return FALSE;   //屏蔽群文件签名操作
	CString strTemp=strUrl;
	CString strHost(_T("ecspace.oss-cn-hangzhou.aliyuncs.com"));
	int nLocation = strTemp.Find(strHost);      //群文件 目录 需要授权访问
	if (-1 != nLocation)
	{
		strBucket = L"ecspace";
		strObject = L"";
		strTemp=strTemp.Right(strTemp.GetLength()-nLocation-strHost.GetLength()-1);
		int nIndex = strTemp.Find('?');
		if (-1 != nIndex)
		{
			strObject = strTemp.Left(nIndex);
			return TRUE;
		}
	}
	return FALSE;
}

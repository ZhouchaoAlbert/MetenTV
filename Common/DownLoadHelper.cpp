#include "stdafx.h"
#include "DownLoadHelper.h"

BOOL IsSignedUrl(CString strUrl, CString& strObject, CString& strBucket)
{
	return FALSE;   //����Ⱥ�ļ�ǩ������
	CString strTemp=strUrl;
	CString strHost(_T("ecspace.oss-cn-hangzhou.aliyuncs.com"));
	int nLocation = strTemp.Find(strHost);      //Ⱥ�ļ� Ŀ¼ ��Ҫ��Ȩ����
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

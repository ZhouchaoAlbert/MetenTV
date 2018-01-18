#include "stdafx.h"
#include "UtilString.h"

BOOL Util::String::A_2_W(LPCSTR pszSrc, CString& strDst)
{
	try
	{
		CA2W pszW(pszSrc);
		if (NULL == pszW)
		{
			return FALSE;
		}
		strDst = pszW;
	}
	catch (CAtlException &e)
	{
		e;
		return FALSE;
	}

	return TRUE;
}

BOOL Util::String::W_2_A(LPCWSTR pszSrc, CStringA& strDst)
{
	try
	{
		CW2A pszA(pszSrc); 
		if (NULL == pszA)
		{
			return FALSE;
		}
		strDst = pszA;
	}
	catch (CAtlException &e)
	{
		e;
		return FALSE;
	}

	return TRUE;
}


//Utf8 Unicode
BOOL Util::String::Utf8_2_W(LPCSTR pszSrc, CString& strDst)
{
	try
	{
		CA2W pszW(pszSrc, CP_UTF8);
		if (NULL == pszW)
		{
			return FALSE;
		}
		strDst = pszW;
	}
	catch (CAtlException &e)
	{
		e;
		return FALSE;
	}

	return TRUE;
}

BOOL Util::String::W_2_Utf8(LPCWSTR pszSrc, CStringA& strDst)
{
	try
	{
		CW2A pszU8(pszSrc, CP_UTF8);
		if (NULL == pszU8)
		{
			return FALSE;
		}
		strDst = pszU8;
	}
	catch (CAtlException &e)
	{
		e;
		return FALSE;
	}

	return TRUE;
}

 BOOL A_2_Utf8(LPCSTR pszSrc, CStringA& strDst)
 {
	try
	{
		CW2A pszU8(CA2W(pszSrc), CP_UTF8);
		strDst = pszU8;
	}
	catch(CAtlException &e)
	{
		e;
		return FALSE;
	}

	return TRUE;
 }

 BOOL Util::String::Utf8_2_A(LPCSTR pszSrc, CStringA& strDst)
 {
	try
	{
		CW2A pszA(CA2W(pszSrc, CP_UTF8)); 
		strDst = pszA;
	}
	catch(CAtlException &e)
	{
		e;
		return FALSE;
	}
	return TRUE;
 }

BOOL Util::String::A_2_W(LPCSTR pszSrc, CString& strDst, UINT32 uCodePage)
{
	try
	{
		CA2W pszW(pszSrc, uCodePage);
		if (NULL == pszW)
		{
			return FALSE;
		}
		strDst = pszW;
	}
	catch (CAtlException &e)
	{
		e;
		return FALSE;
	}

	return TRUE;
}

BOOL Util::String::W_2_A(LPCWSTR pszSrc, CStringA& strDst, UINT32 uCodePage)
{
	try
	{
		CW2A pszU8(pszSrc, uCodePage);
		if (NULL == pszU8)
		{
			return FALSE;
		}
		strDst = pszU8;
	}
	catch (CAtlException &e)
	{
		e;
		return FALSE;
	}

	return TRUE;
}
//
//INT32 Util::String::string2wstring(LPCSTR lpSrc, LPWSTR *lppDes, UINT32 uCodePage)
//{
//	if (0 == lpSrc)
//	{
//		return 0;
//	}
//
//	UINT32 uDesLen = MultiByteToWideChar(uCodePage, NULL, lpSrc, -1, 0, 0);
//	*lppDes = new wchar_t[uDesLen + 1];
//	memset(*lppDes, 0, (uDesLen + 1)*sizeof(wchar_t));
//	return MultiByteToWideChar(uCodePage, 0, lpSrc, -1, *lppDes, uDesLen);
//}
//
//INT32 Util::String::string2wstring(LPCSTR lpSrc, LPWSTR lpDes, UINT32 uDesSize, UINT32 uCodePage)
//{
//	if (0 == lpSrc || 0 == lpDes || 0 == uDesSize)
//	{
//		return 0;
//	}
//
//	return MultiByteToWideChar(uCodePage, 0, lpSrc, -1, lpDes, uDesSize);
//}
//
//INT32 Util::String::string2wstring(LPCSTR lpSrc, ATL::CString &strDes, UINT32 uCodePage)
//{
//	if (0 == lpSrc)
//	{
//		return 0;
//	}
//	strDes = L"";
//	UINT32 uDesLen = MultiByteToWideChar(uCodePage, NULL, lpSrc, -1, 0, 0);
//	wchar_t *pDes = new wchar_t[uDesLen + 1];
//	memset(pDes, 0, (uDesLen + 1)*sizeof(wchar_t));
//	MultiByteToWideChar(uCodePage, 0, lpSrc, -1, pDes, uDesLen);
//	strDes.Format(L"%s", pDes);
//	delete[] pDes;
//	return strDes.GetLength();
//}

// INT32 Util::String::wstring2string(LPCWSTR lpSrc, LPSTR *lppDes, UINT32 uCodePage)
// {
// 	if (0 == lpSrc || 0 == lppDes)
// 	{
// 		return 0;
// 	}
// 
// 	UINT32 uDesLen = WideCharToMultiByte(uCodePage, NULL, lpSrc, -1, NULL, 0, NULL, FALSE);
// 
// 	*lppDes = new char[uDesLen + 1];
// 	memset(*lppDes, 0, uDesLen + 1);
// 
// 	return WideCharToMultiByte(uCodePage, NULL, lpSrc, -1, *lppDes, uDesLen, NULL, FALSE);
// }
// 
 INT32 Util::String::wstring2string(LPCWSTR lpSrc, LPSTR lpDes, UINT32 uDesSize, UINT32 uCodePage)
 {
 	if (0 == lpSrc || 0 == lpDes)
 	{
 		return 0;
 	}
 
 	return WideCharToMultiByte(uCodePage, NULL, lpSrc, -1, lpDes, uDesSize - 1, NULL, FALSE);
 }
// 
// INT32 Util::String::wstring2string(LPCWSTR lpSrc, ATL::CStringA &strDes, UINT32 uCodePage)
// {
// 	if (0 == lpSrc)
// 	{
// 		return 0;
// 	}
// 
// 	strDes = "";
// 
// 	UINT32 uDesLen = WideCharToMultiByte(uCodePage, NULL, lpSrc, -1, NULL, 0, NULL, FALSE);
// 
// 	char *pDes = new char[uDesLen + 1];
// 	memset(pDes, 0, uDesLen + 1);
// 	WideCharToMultiByte(uCodePage, NULL, lpSrc, -1, pDes, uDesLen, NULL, FALSE);
// 	strDes.Format("%s", pDes);
// 	delete[] pDes;
// 	return strDes.GetLength();
// }

void Util::String::Guid2WString(const GUID& guid, ATL::CString& strRet)
{
	strRet.Format(_T("{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"), 
				guid.Data1, guid.Data2, guid.Data3, 
				guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], 
				guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);	
}

void Util::String::StringSplit(CString strSource, CString strSplit, vector<CString>& vecSplit)
{
	vecSplit.clear();
	if (strSource.IsEmpty())
		return;
	if (strSplit.IsEmpty())
		vecSplit.push_back(strSource);
	else
	{
		CString strT = L"";
		int nIndex = 0;
		while (!strSource.IsEmpty())
		{
			nIndex = strSource.Find(strSplit);
			if (nIndex >= 0)
			{
				strT = strSource.Left(nIndex);
				strSource.Delete(0, strT.GetLength() + strSplit.GetLength());
			}
			else
			{
				strT = strSource;
				strSource = L"";
			}
			vecSplit.push_back(strT);
		}
	}
}

int Util::String::StrCmp(TCHAR* p1, TCHAR* p2)
{
	if (!p1)
	{
		if (!p2)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		if (!p2)
		{
			return -1;
		}
		else
		{
			return _tcscmp(p1, p2);
		}
	}
}
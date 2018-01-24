#pragma once
#include "BaseDefine.h"
#include <atlstr.h>
#include <vector>
#include <string>
using namespace std;
using namespace ATL;

//http://blog.csdn.net/akof1314/article/details/7397206
//1.Ansi¡úUnicode
//CA2W pszW(pszA); 
//2.Ansi¡úUtf - 8
//CW2A pszU8(CA2W(pszA), CP_UTF8);  
//3.Unicode¡úAnsi
//CW2A pszA(pszW); 
//4.Unicode¡úUtf - 8
//CW2A pszU8(pszW, CP_UTF8); 
//5.Utf - 8¡úAnsi
//CW2A pszA(CA2W(pszU8, CP_UTF8)); 
//6.Utf - 8¡úUnicode
//CA2W pszW(pszU8, CP_UTF8);
namespace Util
{
	namespace Convert
	{
		COMMON_API BOOL W_2_Utf8(LPCWSTR pszSrc, std::string& str);
		COMMON_API BOOL W_2_Utf8(LPCWSTR pszSrc, CStringA& strDst);
	}
	namespace String
	{
		//Ansci - Unicode
		COMMON_API BOOL A_2_W(LPCSTR pszSrc, CString& strDst);
		COMMON_API BOOL W_2_A(LPCWSTR pszSrc, CStringA& strDst);

		//Utf8  - Unicode
		COMMON_API BOOL Utf8_2_W(LPCSTR pszSrc, CString& strDst);
		COMMON_API BOOL W_2_Utf8(LPCWSTR pszSrc, CStringA& strDst);


		COMMON_API BOOL A_2_Utf8(LPCSTR pszSrc, CStringA& strDst);
		COMMON_API BOOL Utf8_2_A(LPCSTR pszSrc, CStringA& strDst);

		//user
		COMMON_API BOOL A_2_W(LPCSTR pszSrc, CString& strDst, UINT32 uCodePage);
		COMMON_API BOOL W_2_A(LPCWSTR pszSrc, CStringA& strDst, UINT32 uCodePage);

		
		//COMMON_API INT32 string2wstring(LPCSTR lpSrc, LPWSTR lpDes, UINT32 uDesSize, UINT32 uCodePage);

		//COMMON_API INT32 string2wstring(LPCSTR lpSrc, LPWSTR *lppDes, UINT32 uCodePage);
		//COMMON_API INT32 string2wstring(LPCSTR lpSrc, LPWSTR lpDes, UINT32 uDesSize, UINT32 uCodePage);
		//COMMON_API INT32 string2wstring(LPCSTR lpSrc, ATL::CString &strDes, UINT32 uCodePage);

		//COMMON_API INT32 wstring2string(LPCWSTR lpSrc, LPSTR *lppDes, UINT32 uCodePage);
		COMMON_API INT32 wstring2string(LPCWSTR lpSrc, LPSTR lpDes, UINT32 uDesSize, UINT32 uCodePage);
		//COMMON_API INT32 wstring2string(LPCWSTR lpSrc, ATL::CStringA &strDes, UINT32 uCodePage);

		COMMON_API void Guid2WString(const GUID& guid, ATL::CString& strRet);
		
		COMMON_API void StringSplit(CString strSource, CString strSplit, vector<CString>& vecSplit);

		COMMON_API int StrCmp(TCHAR* p1, TCHAR* p2);
	}
}


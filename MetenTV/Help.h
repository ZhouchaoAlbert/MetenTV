#pragma once
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include <assert.h>
#include <vector>
#include <map>
#include <atlstr.h>

using namespace ATL;
using namespace std;

//codec
BOOL Decode_Js2Cpp_SyncList(const std::string& json_text, map<CString, CString>& list);
BOOL Decode_Js2Cpp_Upgrade(const std::string& strContent, CString& ver, CString& url, CString& md5);

BOOL Encode_Cpp2Js_FileList(const map<CString, CString>& list, std::string& json_text);
BOOL Encode_Cpp2Js_DownComplete(const CString& id, const CString& filepath, std::string& json_text);
BOOL Encode_Cpp2Js_MessageBox(const CString& text, std::string& json_text);
BOOL Encode_Cpp2Js_ReportVersion(const UINT32& local_ver, std::string& json_text);

//Ö´ÐÐJS
void ExecJavaScript(CefRefPtr<CefBrowser>browser, LPCTSTR szCmd, LPCSTR szJsonData);
CString CombinePath(std::vector<CString>& aFileNames);

class ISafeChannel
{
public:
	ISafeChannel() {}
	virtual ~ISafeChannel(){}

	virtual LRESULT Js2Cpp(const CString& cmd, const CString& param) = 0;
	virtual LRESULT Cpp2Js(const std::wstring& cmd, const std::string& param) = 0;

	virtual LRESULT EnterFullScreen_Helper() = 0;
	virtual LRESULT ExitFullScreen_Helper() = 0;
};
ISafeChannel*	GetSafeChannel();
void	SetSafeChannel(ISafeChannel* channel);

#define PROG_VER (1004)


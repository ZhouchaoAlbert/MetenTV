// MeilianTV.cpp : WinMain 的实现
#include "stdafx.h"
#include "resource.h"
#include "MeilianTV_i.h"
#include "xdlldata.h"
#include "MainFrame.h"

#include "CefBrowserApp.h"
#include "include/cef_client.h"
#include "include/cef_app.h" 
#include "Help.h"
#include "UtilPath.h"
#include "UtilLog.h"

#pragma comment(lib, "libcef.lib")
#pragma comment(lib, "libcef_dll_wrapper.lib") 

using namespace ATL;

class CMeilianTVModule : public ATL::CAtlExeModuleT< CMeilianTVModule >
{
public :
	DECLARE_LIBID(LIBID_MeilianTVLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MEILIANTV, "{8D25AD50-5ED8-41BD-A26A-404D51CE5A38}")
};

CMeilianTVModule _AtlModule;

static CString Host_GetExeFolder()
{
	TCHAR szDir[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szDir, sizeof(szDir));

	CString	filepath = szDir;
	int ix = filepath.ReverseFind(_T('\\'));
	if (ix != -1)
	{
		return filepath.Mid(0, ix);
	}

	ATLASSERT(FALSE);
	return _T("");
}

//
extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, 
								LPTSTR lpCmdLine, int nShowCmd)
{
	CString strECPath = Host_GetExeFolder();
	Util::Path::SetExeFolder(strECPath);
	Util::Log::Init();
	Util::Log::SetLevel(Util::Log::E_RLL_INFO);
	Util::Log::Change(NULL, 0);

	CString strPerfLaunch = lpCmdLine;
	BOOL bPerfLaunch = strPerfLaunch.Find(_T("--PerfLaunch")) >= 0;
	Util::Log::Info(_T("MentenPV"), _T("CmdLine: %s"), strPerfLaunch);
	//唤起进程
	HWND hWnd = FindWindow(_T("MetenTV_Class"), _T("美联电视台"));
	HRESULT hr = GetLastError();
	Util::Log::Info(_T("Launch"), _T("Hwnd: 0x%x, IsWindow: %u, bPerfLaunch: %u, hr: %0xx"), hWnd, ::IsWindow(hWnd), bPerfLaunch, hr);
	if (hWnd && ::IsWindow(hWnd))
	{
		if (!::IsWindowVisible(hWnd))
		{
			if (!bPerfLaunch)
			{
				PostMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
			}
		}

		return 0;
	}
	
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CefMainArgs mainArgs(hInstance);

	CefSettings settings;
	settings.no_sandbox = true;
	settings.single_process = true;
	settings.multi_threaded_message_loop = true;
	CefString(&settings.cache_path) = Util::Path::GetCefCachePath(TRUE);
	CefString(&settings.log_file) = Util::Path::GetLogFolder(TRUE) + _T("\\LibCef.log");
	CefString(&settings.user_data_path) = Util::Path::GetCefUserPath(TRUE);

	CString strTemp1 = Util::Path::GetInstallFolder();
	strTemp1 += _T("\\CefRes");

	std::vector<CString> aFileNames;
	size_t size = Util::Path::SplitString(strTemp1, _T("\\"), aFileNames);
	strTemp1 = CombinePath(aFileNames);

	CefString(&settings.resources_dir_path) = strTemp1;
	CefString(&settings.locales_dir_path) = strTemp1 + _T("\\locales");

	CefRefPtr<CCefBrowserApp> objApp(new CCefBrowserApp());
	CefInitialize(mainArgs, settings, objApp.get() /*NULL*/, NULL);

	CMainFrame	wnd;
	wnd.CreateWnd(bPerfLaunch);

	int ret = _AtlModule.WinMain(nShowCmd);
	 
	CefShutdown();

	return ret;
}


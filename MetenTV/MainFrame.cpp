#include "stdafx.h"
#include "MainFrame.h"

#include "include/wrapper/cef_closure_task.h"
#include "include/base/cef_bind.h"
#include "CefBrowserEventHandler.h"
#include "Help.h"
#include "UtilPath.h"
#include "Singleton.h"
#include "LogicCenter.h"
#include "UtilString.h"
#include "MD5Helper.h"

CMainFrame::CMainFrame() : m_bFullScreen(FALSE)
{
	memset(&m_rcFullScreenRect, 0, sizeof(m_rcFullScreenRect));
	memset(&m_wpPrev, 0, sizeof(m_wpPrev));
}

CMainFrame::~CMainFrame()
{
	if (NULL != m_hWnd)
	{
		DestroyWindow();
		m_hWnd = NULL;
	}
}

// �첽���������
void CreateBrowserSync(
	CefRefPtr<CefBrowser> *pBrowser,
	CefRefPtr<CefClient> pClient,
	HWND hParentWnd,
	RECT rc,
	CefString url,
	bool bOffScreen,
	bool bTransparent)
{
	if (pBrowser == NULL) return;

	// CefBrowserHost::CreateBrowserSync ������cef�����ui�߳�ִ��
	if (!CefCurrentlyOn(TID_UI)) {
		CefPostTask(TID_UI,
			base::Bind(&CreateBrowserSync,
			pBrowser,
			pClient,
			hParentWnd,
			rc,
			url,
			bOffScreen,
			bTransparent));

		return;
	}

	CefWindowInfo info;
	info.x = rc.left;
	info.y = rc.top;
	info.width = rc.right - rc.left;
	info.height = rc.bottom - rc.top;

	CefBrowserSettings settings;
	
	if (bOffScreen) //������Ⱦ��ʽ
	{
		info.SetAsWindowless(hParentWnd, bTransparent);
	}
	else //�Ӵ��ڷ�ʽ
	{
		info.SetAsChild(hParentWnd, rc);
	}

	//���������
	(*pBrowser) = CefBrowserHost::CreateBrowserSync(info, pClient, url, settings, NULL);
}

BOOL CMainFrame::CreateWnd(BOOL bPerfLaunch)
{
	if (NULL == CWindowImpl::Create(NULL, CWindow::rcDefault, _T("��������̨"), 0, CMainFrame::GetWndExStyle(0) | WS_EX_TRANSPARENT))
	{
		return FALSE;
	}
	HICON hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDI_MAIN));
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	if (bPerfLaunch)
	{
		ShowWindow(SW_HIDE);
	}
	else
	{
		ShowWindow(SW_MAXIMIZE);
		UpdateWindow();
	}

	//SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);//���

	SetSafeChannel(this);

//	SetTimer(1, 5000, NULL);

	RECT rtClient;
	::GetClientRect(m_hWnd, &rtClient);
	CefRefPtr<CCefBrowserEventHandler> objEventHandler(new CCefBrowserEventHandler());

	//step0: check
	CString html_1 = Util::Path::GetDTmpFolder() + _T("\\meten-tv.html");
	CString html_2 = Util::Path::GetDTmpFolder() + _T("\\meten-tv_check.html");
	if (!Util::Path::IsFileExist(html_1))
	{
		if (!Util::Path::IsFileExist(html_2))
		{
			CString html_0 = Util::Path::GetExeFolder() + _T("\\meten-tv.html");

			if (!Util::Path::IsFileExist(html_0))
			{
				MessageBox(_T("���󣬰�װ���𻵣������ز���װ���µİ汾"), _T("����"), MB_OK | MB_ICONERROR);
				::PostQuitMessage(0);
				return TRUE;
			}

			Util::Path::ValidDirPath(Util::Path::GetDTmpFolder());
			if (0 == CopyFile(html_0, html_2, FALSE))
			{

			}
		}

		if (Util::Path::IsFileExist(html_2))
		{
			if (0 == ::MoveFile(html_2, html_1))
			{

			}
		}
		else
		{
			CString html_0 = Util::Path::GetExeFolder() + _T("\\meten-tv.html");

			Util::Path::DeleteFolder(Util::Path::GetDTmpFolder());
			Util::Path::ValidDirPath(Util::Path::GetDTmpFolder());
		}
	}
	if (Util::Path::IsFileExist(html_1))
	{
		if (Util::Path::IsFileExist(html_2))
		{
			//���Ƿ��ļ��Ƿ�ͬ
			CString hash_1;
			CString hash_2;
			Util::MD5::GetFileMD5Value(html_1, hash_1);
			Util::MD5::GetFileMD5Value(html_2, hash_2);

			if (hash_1.CompareNoCase(hash_2))
			{
				if (0 == ::MoveFile(html_2, html_1))
				{
				
				}
			}		
		}

		Util::Path::ValidDirPath(Util::Path::GetDTmpFolder());

		CString strUrl;
		strUrl.Format(_T("%s?_=%lld"), html_1, time(NULL));
		CreateBrowserSync(&m_pBrowser, objEventHandler.get(), m_hWnd, rtClient, CefString(strUrl), FALSE, FALSE);
	}
	else
	{
//		CString strUrl;
//		strUrl.Format(_T("http://coursetest.miamusic.com/meten-tv.html?_=%lld"), time(NULL));
//		CreateBrowserSync(&m_pBrowser, objEventHandler.get(), m_hWnd, rtClient, CefString(strUrl), FALSE, FALSE);

		MessageBox(_T("���󣬰�װ���𻵣������ز���װ���µİ汾"), _T("����"), MB_OK | MB_ICONERROR);
		::PostQuitMessage(0);
		return TRUE;
	}
	
	return TRUE;
}

LRESULT CMainFrame::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//	FullScreenView();
	return S_OK;
}

LRESULT CMainFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!::IsIconic(*this)) 
	{
		RECT rc;
		::GetClientRect(m_hWnd, &rc);

		if (m_pBrowser)
		{
			if (m_pBrowser.get())
			{
				CefRefPtr<CefBrowserHost> pBrowserHost = m_pBrowser->GetHost();
				if (pBrowserHost.get())
				{					
					CefWindowHandle hWnd = pBrowserHost->GetWindowHandle();
					if (::IsWindow(hWnd))
					{
						::MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
					}
				}
			}
		}
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CMainFrame::OnJS2CPP(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BSTR cmd = (BSTR)wParam;
	BSTR json = (BSTR)lParam;

	CString strCmd;
	CString strParam;

	USES_CONVERSION;
	strCmd = OLE2T(cmd);
	::SysFreeString(cmd);

	strParam = OLE2T(json);
	::SysFreeString(json);

	if (!strCmd.CompareNoCase(_T("getList")))
	{
		Singleton<LogicCenter>::Instance().OnJsEvent_GetList();
	}
	else if (!strCmd.CompareNoCase(_T("syncList")))
	{		
		CStringA strContent;
		Util::String::W_2_Utf8(strParam, strContent);

		map<CString, CString> list;
		if (Decode_Js2Cpp_SyncList(strContent, list))
		{
			Singleton<LogicCenter>::Instance().OnJsEvent_SyncList(list);
		}
	}
	else if (!strCmd.CompareNoCase(_T("userAction")))
	{
		if (!strParam.CompareNoCase(_T("enterFullScreen")))
		{
			Singleton<LogicCenter>::Instance().OnJsEvent_EnterFullScreen();
		}
		else if (!strParam.CompareNoCase(_T("exitFullScreen")))
		{
			Singleton<LogicCenter>::Instance().OnJsEvent_ExitFullScreen();
		}
	}
	else if (!strCmd.CompareNoCase(_T("upgrade")))
	{
		CStringA strContent;
		Util::String::W_2_Utf8(strParam, strContent);

		CString ver;
		CString url;
		CString md5;
		if (Decode_Js2Cpp_Upgrade(strContent, ver, url, md5))
		{
			Singleton<LogicCenter>::Instance().OnJsEvent_Upgrade(ver, url, md5);
		}

	}

	bHandled = TRUE;
	return 0;
}

LRESULT CMainFrame::Js2Cpp(const CString& cmd, const CStringA& param)
{
	PostMessage(WM_JS2CPP, (WPARAM)cmd.AllocSysString(), (LPARAM)param.AllocSysString());
	return 0L;
}

LRESULT CMainFrame::Cpp2Js(const CString& cmd, const CStringA& param)
{
	ExecJavaScript(m_pBrowser, cmd, param);
	return 0L;
}

LRESULT CMainFrame::EnterFullScreen_Helper()
{
	FullScreenView(TRUE);
	return 0L;
}

LRESULT CMainFrame::ExitFullScreen_Helper()
{
	FullScreenView(FALSE);
	return 0L;
}

void CMainFrame::FullScreenView(BOOL bFullScreen)
{
	if (bFullScreen == m_bFullScreen)
	{
		return;
	}

	HideTaskBar(bFullScreen);
	RECT rectDesktop;
	WINDOWPLACEMENT wpNew;
	if (!m_bFullScreen)
	{
		// We'll need these to restore the original state.
		GetWindowPlacement(&m_wpPrev);
		//Adjust RECT to new size of window
		::GetWindowRect(::GetDesktopWindow(), &rectDesktop);
		::AdjustWindowRectEx(&rectDesktop, GetStyle(), FALSE, GetExStyle());
		// Remember this for OnGetMinMaxInfo()
		m_rcFullScreenRect = rectDesktop;
		wpNew = m_wpPrev;
		wpNew.showCmd = SW_SHOWNORMAL;
		wpNew.rcNormalPosition = rectDesktop;
		m_bFullScreen = TRUE;
	}
	else
	{
		// �˳�ȫ��Ļʱ�ָ���ԭ���Ĵ���״̬
		m_bFullScreen = FALSE;
		wpNew = m_wpPrev;
	}
	SetWindowPlacement(&wpNew);
}

LRESULT CMainFrame::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MINMAXINFO* lpMMI = (MINMAXINFO *)lParam;

	if (m_bFullScreen)
	{
		lpMMI->ptMaxSize.y = m_rcFullScreenRect.bottom - m_rcFullScreenRect.top;
		lpMMI->ptMaxTrackSize.y = lpMMI->ptMaxSize.y;
		lpMMI->ptMaxSize.x = m_rcFullScreenRect.right - m_rcFullScreenRect.left;
		lpMMI->ptMaxTrackSize.x = lpMMI->ptMaxSize.x;
	}

	return 0;
}

void CMainFrame::HideTaskBar(BOOL bHide)
{
	int nCmdShow;
	LPARAM lParam;

	HWND hWnd = FindWindow(_T("Shell_TrayWnd"), NULL);
	if (bHide == TRUE)
	{
		nCmdShow = SW_HIDE;
		lParam = ABS_AUTOHIDE | ABS_ALWAYSONTOP;
	}
	else
	{
		nCmdShow = SW_SHOW;
		lParam = ABS_ALWAYSONTOP;
	}

	::ShowWindow(hWnd, nCmdShow);//����������


#ifndef ABM_SETSTATE 
#define ABM_SETSTATE 0x0000000a 
#endif

	APPBARDATA apBar;
	memset(&apBar, 0, sizeof(apBar));
	apBar.cbSize = sizeof(apBar);
	apBar.hWnd = hWnd;
	if (apBar.hWnd != NULL)
	{
		apBar.lParam = lParam;
		SHAppBarMessage(ABM_SETSTATE, &apBar); //�����������Զ�����
	}
}

LRESULT CMainFrame::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return TRUE;
}

LRESULT CMainFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	::BeginPaint(m_hWnd, &ps);
	::EndPaint(m_hWnd, &ps);

	bHandled = TRUE;
	return TRUE;
}
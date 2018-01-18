#include "StdAfx.h"
#include "DevTool.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/base/cef_bind.h"


CDevTool::CDevTool()
{
}


CDevTool::~CDevTool()
{
	
}

//关闭开发工具
void CDevTool::CloseDevTools(CefRefPtr<CefBrowser> browser)
{
	if (m_pDevTool.get())
	{
		CefRefPtr<CSimpleHandler> pDevTool = m_pDevTool;
		m_pDevTool = NULL;
		pDevTool->Close();
	}
}

//打开开发工具
bool CDevTool::ShowDevTools(CefRefPtr<CefBrowser> browser, const CefPoint& inspect_element_at)
{
	if (!browser.get())
		return false;
	CefRefPtr<CefBrowserHost> pHost = browser->GetHost();
	if (!pHost.get())
		return false;
	CefWindowHandle hWnd = pHost->GetWindowHandle();
	if (!m_pDevTool.get())
	{
		CefWindowInfo windowInfo;
		windowInfo.SetAsPopup(hWnd, "DevTools");
		CefBrowserSettings settings;
		m_pDevTool = new CSimpleHandler(this);
		pHost->ShowDevTools(windowInfo, m_pDevTool, settings, inspect_element_at);
		return true;
	}
	return false;
}

// net-internals
void CDevTool::SwitchNetInternalsPage()
{
	if (!CefCurrentlyOn(TID_UI)) {
		CefPostTask(TID_UI, base::Bind(&CDevTool::SwitchNetInternalsPage, this));
		return;
	}

	if (!m_pDevTool.get())
	{
		m_pDevTool = CefRefPtr<CSimpleHandler>(new CSimpleHandler(this));

		CefBrowserSettings browser_settings;
		browser_settings.background_color = 822;

		CefWindowInfo window_info;
		window_info.SetAsPopup(NULL, "net-internals");

		bool bRet = CefBrowserHost::CreateBrowser(window_info, m_pDevTool.get(), "chrome://net-internals", browser_settings, NULL);
		if (!bRet)
		{
			CefRefPtr<CSimpleHandler> pNetInternals = m_pDevTool;
			m_pDevTool = NULL;
			pNetInternals->Close();
		}
	}
	else
	{
		CefRefPtr<CSimpleHandler> pNetInternals = m_pDevTool;
		m_pDevTool = NULL;
		pNetInternals->Close();
	}
}

// 是否打开了调试工具
bool CDevTool::IsOpen()
{
	if (!m_pDevTool.get())
		return false;
	else
		return true;
}

// 响应关闭调试工具的通知
void CDevTool::OnDevToolClose()
{
	m_pDevTool = NULL;
}

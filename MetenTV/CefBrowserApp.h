#pragma once

#include "include/cef_client.h"
#include "include/cef_app.h" 
#include<atlstr.h>
using namespace ATL;

class CCefBrowserApp :
	public CefApp,
	public CefBrowserProcessHandler,
	public CefRenderProcessHandler
{
public:
	CCefBrowserApp();
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
		OVERRIDE{ return this; }

	virtual bool OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		NavigationType navigation_type,
		bool is_redirect) OVERRIDE;

	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) OVERRIDE;

	virtual void OnBeforeCommandLineProcessing(
		const CefString& process_type,
		CefRefPtr<CefCommandLine> command_line) OVERRIDE;

	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
		OVERRIDE{ return this; }

	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) OVERRIDE;
public:
	// ÃÌº”√¸¡Ó
	void AddCommandLine(LPCTSTR szSwitchName, LPCTSTR szSwitchValue);

	// …æ≥˝√¸¡Ó
	void RemoveCommandLine(LPCTSTR szSwitchName);

	// ÃÌº”√¸¡Ó÷µ
	void AddCommandValue(LPCTSTR szSwitchName, LPCTSTR szSwitchValue);

	// …æ≥˝√¸¡Ó÷µ
	void RemoveCommandValue(LPCTSTR szSwitchName);

	IMPLEMENT_REFCOUNTING(CCefBrowserApp);
protected:
	std::map<CString, CString>	m_mapCommandLine;
	std::map<CString, CString>	m_mapCommandValue;
};
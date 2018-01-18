#ifndef __DEV_TOOL_H__
#define __DEV_TOOL_H__

#include "SimpleHandler.h"

class CDevTool 
	: public IDevToolSink
	, public CefBase
{
public:
	CDevTool();
	~CDevTool();
public:
	//关闭开发工具
	void CloseDevTools(CefRefPtr<CefBrowser> browser);

	//打开开发工具
	bool ShowDevTools(CefRefPtr<CefBrowser> browser, const CefPoint& inspect_element_at);

	// net-internals
	void SwitchNetInternalsPage();

	// 是否打开了调试工具
	bool IsOpen();

	// 响应关闭调试工具的通知
	virtual void OnDevToolClose();

	IMPLEMENT_REFCOUNTING(CDevTool);
private:
	CefRefPtr<CSimpleHandler> m_pDevTool;
};

#endif


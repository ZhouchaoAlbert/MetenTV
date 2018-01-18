#pragma once

#include <list>
#include "include/cef_client.h"
#include "include/cef_app.h" 

using namespace std;

typedef std::list<CefRefPtr<CefBrowser>> BrowserList;

class CCefBrowserEventHandler
	: public CefClient
	, public CefDisplayHandler			// 显示变化事件
	, public CefLoadHandler			    // 加载错误事件
	, public CefLifeSpanHandler		    // 声明周期事件
	, public CefKeyboardHandler
	, public CefContextMenuHandler	// 上下文菜单事件
	//, public CefDialogHandler			// 对话框事件
	//, public CefDownloadHandler		// 下载事件
	//, public CefDragHandler			// 拖拽事件
	//, public CefFindHandler			// 查找事件
	//, public ...
{
public:
	CCefBrowserEventHandler(void);
	virtual ~CCefBrowserEventHandler(void);

public:
	// CefClient 事件处理器,如果没有对应处理器则默认使用内部处理器
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE;
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE;
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
	virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE;
	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE;
public:	
	// display handler method
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;

public:
	// load handler method
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) OVERRIDE;

	// CefKeyboardHandler 接口
	//
	virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
		const CefKeyEvent& event,
		CefEventHandle os_event,
		bool* is_keyboard_shortcut) OVERRIDE;

	virtual bool OnKeyEvent(CefRefPtr<CefBrowser> browser,
		const CefKeyEvent& event,
		CefEventHandle os_event)  OVERRIDE;

	//
	// CefContextMenuHandler 接口
	//
	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model) OVERRIDE;

	virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		int command_id,
		EventFlags event_flags) OVERRIDE;

public:
	// display handler meethod
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message);
public:
	// own method of cef browser event handler
	void CloseAllBrowser(bool bForceClose = true);

protected:
	BrowserList	m_browser_list;

	IMPLEMENT_REFCOUNTING(CCefBrowserEventHandler);
};

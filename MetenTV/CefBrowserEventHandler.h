#pragma once

#include <list>
#include "include/cef_client.h"
#include "include/cef_app.h" 

using namespace std;

typedef std::list<CefRefPtr<CefBrowser>> BrowserList;

class CCefBrowserEventHandler
	: public CefClient
	, public CefDisplayHandler			// ��ʾ�仯�¼�
	, public CefLoadHandler			    // ���ش����¼�
	, public CefLifeSpanHandler		    // ���������¼�
	, public CefKeyboardHandler
	, public CefContextMenuHandler	// �����Ĳ˵��¼�
	//, public CefDialogHandler			// �Ի����¼�
	//, public CefDownloadHandler		// �����¼�
	//, public CefDragHandler			// ��ק�¼�
	//, public CefFindHandler			// �����¼�
	//, public ...
{
public:
	CCefBrowserEventHandler(void);
	virtual ~CCefBrowserEventHandler(void);

public:
	// CefClient �¼�������,���û�ж�Ӧ��������Ĭ��ʹ���ڲ�������
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

	// CefKeyboardHandler �ӿ�
	//
	virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
		const CefKeyEvent& event,
		CefEventHandle os_event,
		bool* is_keyboard_shortcut) OVERRIDE;

	virtual bool OnKeyEvent(CefRefPtr<CefBrowser> browser,
		const CefKeyEvent& event,
		CefEventHandle os_event)  OVERRIDE;

	//
	// CefContextMenuHandler �ӿ�
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

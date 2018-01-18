// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_

#include "include/cef_client.h"
#include "DevToolSink.h"
#include <list>

typedef std::list<CefRefPtr<CefBrowser> > BrowserList;

class CSimpleHandler 
	: public CefClient
	, public CefDisplayHandler
	, public CefLifeSpanHandler
	, public CefContextMenuHandler
	, public CefLoadHandler 
{
 public:
	 CSimpleHandler(IDevToolSink *pSink);
	~CSimpleHandler();

	// CefClient methods:
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() { return this;}
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() { return this; }
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() { return this; }
	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler(){ return this; }
	// CefDisplayHandler methods:
	

	// CefLifeSpanHandler methods:
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser);
	virtual bool DoClose(CefRefPtr<CefBrowser> browser);
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser);

	// CefLoadHandler methods:
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl);

	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model);

	virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		int command_id,
		EventFlags event_flags);
public:
	//
	// 自定义函数
	//
	void Close();

	IMPLEMENT_REFCOUNTING(CSimpleHandler);
protected:
	IDevToolSink *m_pSink;
	CefRefPtr<CefBrowser> m_Browser;
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_

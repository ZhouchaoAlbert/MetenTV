// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include "StdAfx.h"
#include "SimpleHandler.h"
#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"


CSimpleHandler::CSimpleHandler(IDevToolSink *pSink)
	: m_pSink(pSink)
{
}

CSimpleHandler::~CSimpleHandler() {
	m_Browser = NULL;
	m_pSink = NULL;
}

void CSimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	if (!m_Browser.get())
	{
		m_Browser = browser;
	}
}

bool CSimpleHandler::DoClose(CefRefPtr<CefBrowser> browser) 
{
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed destription of this
	// process.
	return false;
}

void CSimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	if (browser->IsSame(m_Browser))
	{
		// Free the browser pointer so that the browser can be destroyed
		m_Browser = NULL;

		IDevToolSink *pSink = m_pSink;
		m_pSink = NULL;
		if (pSink)
		{
			pSink->OnDevToolClose();
		}
	}
}

void CSimpleHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl) 
{
  CEF_REQUIRE_UI_THREAD();

  // Don't display an error for downloaded files.
  if (errorCode == ERR_ABORTED)
    return;

  // Display a load error message.
  std::stringstream ss;
  ss << "<html><body bgcolor=\"white\">"
        "<h2>Failed to load URL " << std::string(failedUrl) <<
        " with error " << std::string(errorText) << " (" << errorCode <<
        ").</h2></body></html>";
  frame->LoadString(ss.str(), failedUrl);
}

void CSimpleHandler::Close()
{
	m_pSink = NULL;
	if (!m_Browser.get())
		return;
	CefRefPtr<CefBrowserHost> pHost = m_Browser->GetHost();
	if (!pHost.get())
		return;
	pHost->CloseBrowser(false);
}

void CSimpleHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefContextMenuParams> params,
	CefRefPtr<CefMenuModel> model)
{

}


bool CSimpleHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefContextMenuParams> params,
	int command_id,
	EventFlags event_flags)
{
	return false;
}
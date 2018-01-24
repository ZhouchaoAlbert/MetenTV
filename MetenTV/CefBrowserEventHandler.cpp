#include "StdAfx.h"
#include "CefBrowserEventHandler.h"

#include <sstream>
#include <string>

#include "include/base/cef_bind.h"
//#include "include/views/cef_browser_view.h"
//#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include<atlstr.h>

#include "DevTool.h"
#include "Help.h"
#include "UtilString.h"
#include "UtilLog.h"

#define CLIENT_ID_NOTIFY_DOWNCOMPLETED	1
#define CLIENT_ID_NOTIFY_MESSAGEBOX		2

CefRefPtr<CDevTool>		g_DevTool(new CDevTool());
CefRefPtr<CDevTool>		g_NetInternals(new CDevTool());

CCefBrowserEventHandler::CCefBrowserEventHandler(void)
{
}

CCefBrowserEventHandler::~CCefBrowserEventHandler(void)
{
}

CefRefPtr<CefDisplayHandler> CCefBrowserEventHandler::GetDisplayHandler() 
{
	return this;
}

CefRefPtr<CefLifeSpanHandler> CCefBrowserEventHandler::GetLifeSpanHandler() 
{
	return this;
}

CefRefPtr<CefLoadHandler> CCefBrowserEventHandler::GetLoadHandler() 
{
	return this;
}

CefRefPtr<CefKeyboardHandler> CCefBrowserEventHandler::GetKeyboardHandler() 
{ 
	return this; 
}
CefRefPtr<CefContextMenuHandler> CCefBrowserEventHandler::GetContextMenuHandler()
{
	return this;
}

void CCefBrowserEventHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) 
{
	CEF_REQUIRE_UI_THREAD();

	CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
	SetWindowText(hwnd, std::wstring(title).c_str());
}

void CCefBrowserEventHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, 
										  const CefString& errorText, const CefString& failedUrl) 
{
	CEF_REQUIRE_UI_THREAD();
	if (ERR_ABORTED == errorCode)
		return ;

	std::stringstream ss;
	ss <<	"<html><body bgcolor=\"white\">"
			"<h2>Failed to load URL " << std::string(failedUrl) <<
			" with error " << std::string(errorText) << " (" << errorCode <<
			").</h2></body></html>";
	frame->LoadString(ss.str(), failedUrl);
}

void CCefBrowserEventHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) 
{
	CEF_REQUIRE_UI_THREAD();	
	m_browser_list.push_back(browser);
}

bool CCefBrowserEventHandler::DoClose(CefRefPtr<CefBrowser> browser) 
{
	CEF_REQUIRE_UI_THREAD();
	if (1 == m_browser_list.size())
	{
	}
	return false;
}

void CCefBrowserEventHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) 
{
	CEF_REQUIRE_UI_THREAD();

	for (BrowserList::iterator bit = m_browser_list.begin(); bit != m_browser_list.end(); ++bit) 
	{
		if ((*bit)->IsSame(browser)) {
			m_browser_list.erase(bit);
			break;
		}
	}

	if (m_browser_list.empty()) 
	{
		CefQuitMessageLoop();
	}
}

void CCefBrowserEventHandler::CloseAllBrowser(bool force_close) 
{
	if (!CefCurrentlyOn(TID_UI)) 
	{
		CefPostTask(TID_UI,base::Bind(&CCefBrowserEventHandler::CloseAllBrowser, this, force_close));
		return;
	}

	if (m_browser_list.empty())
		return;

	BrowserList::const_iterator it = m_browser_list.begin();
	for (; it != m_browser_list.end(); ++it)
	{
		(*it)->GetHost()->CloseBrowser(force_close);
	}
}


bool CCefBrowserEventHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
	CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message)
{
	CEF_REQUIRE_UI_THREAD();

#ifdef _UNICODE
	ATL::CString message_name = message->GetName().ToWString().c_str();
#else
	ATL::CString message_name = message->GetName().ToString().c_str();
#endif
	if (message_name == _T("ipc_channel_js2cpp"))
	{
		CefRefPtr<CefListValue> argList = message->GetArgumentList();
		CefString strFuncName = argList->GetString(0);  //注册的函数名
		CefString cmd = argList->GetString(1);			//协议命令
		CefString strJsonData = argList->GetString(2);  //web传过来的Json数据

		ATL::CString strFuncName1((const wchar_t*)strFuncName.c_str(), strFuncName.length());
		ATL::CString scmd1((const wchar_t*)cmd.c_str(), cmd.length());
		ATL::CString strJsonData1((const wchar_t*)strJsonData.c_str(), strJsonData.length());
		
		CStringA strContent;
		Util::Convert::W_2_Utf8(strJsonData1, strContent);

		static UINT32 s_count = 0;
		Util::Log::Info(_T("MentenPV"), _T("Func:%s,JS:Cmd: %s,Json:%s"), strFuncName1, scmd1, strJsonData1);
		if (!scmd1.CompareNoCase(_T("SyncList")))
		{
			int k = 0;
		}

		GetSafeChannel()->Js2Cpp(scmd1, strJsonData1);

		//TCHAR szText[256];
		//_tprintf(_T("FuncName=%s, CMD=%s, JsonData=%s"), strFuncName1, scmd1, strJsonData1);
		//ATL::CString strTest;
		//strTest.Format(_T("FuncName=%s, CMD=%s, JsonData=%s"), strFuncName1, scmd1, strJsonData1);
		///MessageBox(NULL, strTest, _T("js2cpp"), MB_OK);
		return true;		
	}
	return false;
}

bool CCefBrowserEventHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
	const CefKeyEvent& event,
	CefEventHandle os_event,
	bool* is_keyboard_shortcut)
{
	CEF_REQUIRE_UI_THREAD();

	if (event.windows_key_code == VK_F2 && os_event != NULL && os_event->message == WM_KEYFIRST)
	{
		if (g_DevTool.get())
		{
			if (g_DevTool->IsOpen())
				g_DevTool->CloseDevTools(browser);
			else
				g_DevTool->ShowDevTools(browser, CefPoint());
		}
		return true;
	}
	if (event.windows_key_code == VK_F11 && os_event != NULL && os_event->message == WM_KEYFIRST)
	{
		if (g_NetInternals.get())
			g_NetInternals->SwitchNetInternalsPage();
		return true;
	}
	
	return false;
}

bool CCefBrowserEventHandler::OnKeyEvent(CefRefPtr<CefBrowser> browser,
	const CefKeyEvent& event,
	CefEventHandle os_event)  
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
// CefContextMenuHandler 接口
//
void CCefBrowserEventHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefContextMenuParams> params,
	CefRefPtr<CefMenuModel> model)
{
	CEF_REQUIRE_UI_THREAD();

//	if (!browser->IsSame(m_Browser)) return;
	if (params->GetTypeFlags() & CM_TYPEFLAG_EDITABLE)
		return;
	if (params->GetTypeFlags() & CM_TYPEFLAG_SELECTION)
	{
		CefString strCopy = model->GetLabel(MENU_ID_COPY);
		model->Clear();
		if (!strCopy.empty())
			model->AddItem(MENU_ID_COPY, strCopy);
		return;
	}
	model->Clear();

//	if ((params->GetTypeFlags() & (CM_TYPEFLAG_PAGE | CM_TYPEFLAG_FRAME)) != 0) {
		// Add a separator if the menu already has items.
//		if (model->GetCount() > 0)
//			model->AddSeparator();



		// Add a "Show DevTools" item to all context menus.
//		model->AddItem(CLIENT_ID_NOTIFY_DOWNCOMPLETED, "notifyDownloadCompleted");
//		model->AddItem(CLIENT_ID_NOTIFY_MESSAGEBOX, "notifyMessageBox");
		
		// Test context menu features.
	//	BuildTestMenu(model);
//	}
}

bool CCefBrowserEventHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefContextMenuParams> params,
	int command_id,
	EventFlags event_flags)
{
	switch (command_id)
	{
	case CLIENT_ID_NOTIFY_DOWNCOMPLETED:
		{
			ATL::CStringA strJson;

			int id = 13838438;
			ATL::CStringA filepath = "C:\\1.mp4";
			strJson.Format(("{\"id\" : \"%d\", \"filepath\" : \"%s\"}"), id, filepath);
			ExecJavaScript(browser, _T("notifyDownloadCompleted"), strJson);
		}
		break;
	case CLIENT_ID_NOTIFY_MESSAGEBOX:
		{
			ATL::CStringA strJson;

			int id = 13838438;
			ATL::CStringA strText = "hello world";
			strJson.Format(("{\"text\" : \"%s\"}"), strText);
			ExecJavaScript(browser, _T("notifyMessageBox"), strJson);
		}
		break;
	default:
		break;
	}

	return false;
}


#include "StdAfx.h"
#include "CefBrowserApp.h"
#include"V8JSHandler.h"

//#define CEF_2623 1
CCefBrowserApp::CCefBrowserApp()
{
	AddCommandLine(_T("disable-gpu"), _T("disable-gpu"));
	AddCommandLine(_T("process-per-site"), _T("process-per-site"));
	AddCommandLine(_T("enable-npapi"), _T("enable-npapi"));
	AddCommandLine(_T("allow-file-access-from-files"), _T("allow-file-access-from-files"));
	AddCommandLine(_T("disable-web-security"), _T("disable-web-security"));
	// 设置了web调试工具抓不到包，先注释
	// AddCommandLine(_T("no-proxy-server"), _T("no-proxy-server")); 

	// 设置所有的页面共用一个进程，以节省进程数(renderer-process-limit 设置会照常调试工具打开关闭有问题)
	// AddCommandValue(_T("renderer-process-limit"), _T("1")); 
}

bool CCefBrowserApp::OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	NavigationType navigation_type,
	bool is_redirect)
{
	return false;
}

//
bool CCefBrowserApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
	CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message)
{
	std::string message_name = message->GetName();
	if (message_name == "ipc_channel_cpp2js")//收到Browser进行传来的信息进行处理
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		CefString jsCode = args->GetString(0);
		CefRefPtr<CefV8Context> context = browser->GetMainFrame()->GetV8Context();

		CefRefPtr<CefV8Value> retval;
		CefRefPtr<CefV8Exception> exception;
#ifdef CEF_2623
		context->Eval(jsCode, retval, exception);
#else 
		context->Eval(jsCode, "", 0, retval, exception);
#endif
		return true;
	}
	return false;
}

void CCefBrowserApp::OnBeforeCommandLineProcessing(const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line)
{
	std::map<CString, CString>::iterator it = m_mapCommandLine.begin();
	while (it != m_mapCommandLine.end())
	{
		if (!command_line->HasSwitch(it->first.GetString()))
			command_line->AppendSwitch(it->second.GetString());
		it++;
	}

	std::map<CString, CString>::iterator it1 = m_mapCommandValue.begin();
	while (it1 != m_mapCommandValue.end())
	{
		if (!command_line->HasSwitch(it1->first.GetString()))
			command_line->AppendSwitchWithValue(it1->first.GetString(), it1->second.GetString());
		it1++;
	}
}

void CCefBrowserApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context)
{
	context->Enter();
	// The var type can accept all object or variable
	CefRefPtr<CefV8Value> window = context->GetGlobal();

	// bind value into window[or you can bind value into window sub node]
//	CefRefPtr<CefV8Value> strValue = CefV8Value::CreateString(_T("broadcast Test"));
//	window->SetValue(_T("Broadcast"), strValue, V8_PROPERTY_ATTRIBUTE_NONE);

	// bind function 
	CefRefPtr<CV8JsHandler> pJsHandler(new CV8JsHandler());
	CefRefPtr<CefV8Value> myFunc = CefV8Value::CreateFunction(_T("miaJs2CPP"), pJsHandler);
	window->SetValue(_T("miaJs2CPP"), myFunc, V8_PROPERTY_ATTRIBUTE_NONE);

	// bind function 
	context->Exit();
}

//////////////////////////////////////////////////////////////////////////
// 添加命令
void CCefBrowserApp::AddCommandLine(LPCTSTR szSwitchName, LPCTSTR szSwitchValue)
{
	m_mapCommandLine[szSwitchName] = szSwitchValue;
}

// 删除命令
void CCefBrowserApp::RemoveCommandLine(LPCTSTR szSwitchName)
{
	m_mapCommandLine.erase(szSwitchName);
}

// 添加命令值
void CCefBrowserApp::AddCommandValue(LPCTSTR szSwitchName, LPCTSTR szSwitchValue)
{
	m_mapCommandValue[szSwitchName] = szSwitchValue;
}

// 删除命令值
void CCefBrowserApp::RemoveCommandValue(LPCTSTR szSwitchName)
{
	m_mapCommandValue.erase(szSwitchName);
}
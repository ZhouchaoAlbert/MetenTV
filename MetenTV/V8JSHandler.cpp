#include"stdafx.h"
#include "V8JsHandler.h"
#include <ShellAPI.h>
#include <tchar.h>

CV8JsHandler::CV8JsHandler()
{
}

CV8JsHandler::~CV8JsHandler()
{
}

bool CV8JsHandler::Execute(const CefString& func_name,
							 CefRefPtr<CefV8Value> object,
							 const CefV8ValueList& arguments,
							 CefRefPtr<CefV8Value>& retval,
							 CefString& exception)
{
	// 当web中前端调用了window.miaJs2CPP自定义函数后，会触发到这里，从Render进程转发到Broswer进程
	// Broswer进程的ClientHandlerBrowser类在OnProcessMessageReceived接口中收到处理
	if (2 != arguments.size())
	{
		return false;
	}
	
	// send message to browser process that method began to execute  
	CefRefPtr<CefProcessMessage> objMsg = CefProcessMessage::Create(_T("ipc_channel_js2cpp"));
	CefRefPtr<CefListValue> arglist = objMsg->GetArgumentList();
	arglist->SetSize(3);
	
	arglist->SetString(0, func_name);
	arglist->SetString(1, arguments[0]->GetStringValue());	//cmd
	arglist->SetString(2, arguments[1]->GetStringValue());	//json

	CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, objMsg); 
	return true;
}
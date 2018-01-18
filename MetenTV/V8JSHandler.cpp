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
	// ��web��ǰ�˵�����window.miaJs2CPP�Զ��庯���󣬻ᴥ���������Render����ת����Broswer����
	// Broswer���̵�ClientHandlerBrowser����OnProcessMessageReceived�ӿ����յ�����
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
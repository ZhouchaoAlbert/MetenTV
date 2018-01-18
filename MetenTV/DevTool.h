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
	//�رտ�������
	void CloseDevTools(CefRefPtr<CefBrowser> browser);

	//�򿪿�������
	bool ShowDevTools(CefRefPtr<CefBrowser> browser, const CefPoint& inspect_element_at);

	// net-internals
	void SwitchNetInternalsPage();

	// �Ƿ���˵��Թ���
	bool IsOpen();

	// ��Ӧ�رյ��Թ��ߵ�֪ͨ
	virtual void OnDevToolClose();

	IMPLEMENT_REFCOUNTING(CDevTool);
private:
	CefRefPtr<CSimpleHandler> m_pDevTool;
};

#endif


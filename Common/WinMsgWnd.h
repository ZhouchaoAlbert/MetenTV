#pragma once

#include "EventSupport.h"
#include <atlwin.h>
using namespace ATL;

#include "UtilWinMsg.h"

//创建一个窗口
class CGeneralWindow : public CWindowImpl < CGeneralWindow, CWindow, CWinTraits<WS_OVERLAPPEDWINDOW, 0> >
{
public:
	DECLARE_WND_CLASS(_T("CECGeneralWindow"))

	CGeneralWindow(Util::WinMsg::CGeneralWinDestroyEvent* pContainer);
	virtual ~CGeneralWindow();

	HWND	GetGeneralMsgHWND();

	EVENT(evtOnGeneralMsg, (void* pObj, UINT32 msg_id, WPARAM wParam, LPARAM lParam, BOOL& bHandle, LRESULT& lResult), (pObj, msg_id, wParam, lParam, bHandle, lResult));

private:
	Util::WinMsg::CGeneralWinDestroyEvent* m_pContainer;
	void OnFinalMessage(HWND hwnd)
	{
		//不能调用退出进程，由外部决定
		//::PostQuitMessage(0);
	}

	BOOL	ProcessWindowMessage(
		_In_ HWND hWnd,
		_In_ UINT uMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam,
		_Inout_ LRESULT& lResult,
		_In_ DWORD dwMsgMapID);
};

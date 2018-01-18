#include "stdafx.h"

#include "WinMsgWnd.h"
#include "UtilLog.h"

CGeneralWindow::CGeneralWindow(Util::WinMsg::CGeneralWinDestroyEvent* pContainer)
{
	//pContainer¿ÉÒÔÎª¿Õ
	m_pContainer = pContainer;

	m_hWnd = Create(NULL);

	ATLASSERT(m_hWnd);
}

CGeneralWindow::~CGeneralWindow()
{
	if (m_pContainer)
	{
		m_pContainer->On_GeneralWinDestroyEvent(this);
		m_pContainer = NULL;
	}
	if (NULL != m_hWnd)
	{
		DestroyWindow();
		m_hWnd = NULL;
	}
}

HWND	CGeneralWindow::GetGeneralMsgHWND()
{
	return m_hWnd;
}

BOOL	CGeneralWindow::ProcessWindowMessage(
	_In_ HWND hWnd,
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam,
	_Inout_ LRESULT& lResult,
	_In_ DWORD dwMsgMapID)
{
	if (WM_CLOSE == uMsg || WM_DESTROY == uMsg || WM_NCDESTROY == uMsg)
	{
		Util::Log::Info(_T("GeneralWin"), _T("Destroy:	0x%x - %u"), m_hWnd, uMsg);
	}

	BOOL bHandle = FALSE;
	RaiseEvent(evtOnGeneralMsg(this, uMsg, wParam, lParam, bHandle, lResult));
	return bHandle;
}
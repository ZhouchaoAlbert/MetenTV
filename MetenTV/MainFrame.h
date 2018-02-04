#pragma once

#include <atlstr.h>
#include <atlwin.h>

#include "include/cef_client.h"
#include "include/cef_app.h" 
#include "Help.h"
#include "ShmObj.h"

using namespace ATL;

#define WM_JS2CPP (WM_USER + 501)

//创建一个窗口
class CMainFrame : public CWindowImpl < CMainFrame, CWindow, CWinTraits<WS_OVERLAPPEDWINDOW, 0> >, public ISafeChannel
{
public:
	CMainFrame();
	virtual ~CMainFrame();

	BOOL CreateWnd(BOOL bPerfLaunch);

	DECLARE_WND_CLASS(_T("MetenTV_Class"))
	BEGIN_MSG_MAP(CMainFrame)  //添加需要处理的消息的映射 · 声明消息处理函数、实现消息处理函数
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_JS2CPP, OnJS2CPP)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	virtual LRESULT Js2Cpp(const CString& cmd, const CString& param);
	virtual LRESULT Cpp2Js(const std::wstring& cmd, const std::string& param);
	virtual LRESULT EnterFullScreen_Helper();
	virtual LRESULT ExitFullScreen_Helper();

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnJS2CPP(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	CefRefPtr<CefBrowser> m_pBrowser;
	void OnFinalMessage(HWND hwnd)
	{
		::PostQuitMessage(0);
	}

	BOOL m_bFullScreen;
	RECT m_rcFullScreenRect;
	CShmObj m_shm;
	HANDLE  m_hMutex;
	WINDOWPLACEMENT m_wpPrev;
	void FullScreenView(BOOL bFullScreen);
	void HideTaskBar(BOOL bHide);
	BOOL HideTaskBarHelper(HWND hWnd, LPARAM lParam);
};


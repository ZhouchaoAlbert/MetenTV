#pragma once
#include "EventSupport.h"
#include "atlstr.h"
#include "UtilWinMsg.h"

using namespace ATL;

class   COMMON_API XTimerSinkHelper
{		
public:
	XTimerSinkHelper();
	~XTimerSinkHelper();
	EVENT(evtOnTimer, (), ());
	void OnTimer();

	void Start(LPCTSTR pszName, UINT32 millisecond);
	void Stop(LPCTSTR pszName);
	BOOL IsActive();

private:
	TCHAR m_szName[64];
};

//线程安全
class   COMMON_API XThreadTimerSinkHelper : public Util::WinMsg::CGeneralWinDestroyEvent
{		
public:
	XThreadTimerSinkHelper();
	~XThreadTimerSinkHelper();
	EVENT(evtOnThreadTimer, (), ());
	void OnThreadTimer();

	void Start(LPCTSTR pszName, UINT32 millisecond);
	void Stop(LPCTSTR pszName);
	BOOL IsActive();

private:
	DECLARE_EVENT_RECEIVER2(COMMON_API, XThreadTimerSinkHelper)
	TCHAR	m_szName[64];
	//避免导出
	void*	m_pWnd;
	BOOL	m_bActive;

	void	MsgHandle(void* pObj, UINT32 msg_id, WPARAM wParam, LPARAM lParam, BOOL& bHandle, LRESULT& lResult);
	virtual void	On_GeneralWinDestroyEvent(void* pVoid);
};
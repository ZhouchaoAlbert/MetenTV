#include "stdafx.h"
#include "XTimerMgr.h"
#include "Singleton.h"
#include "UtilLog.h"

XTimerMgr::XTimerMgr()
{
}

XTimerMgr::~XTimerMgr()
{
	m_mapForward.clear();
	m_mapInverte.clear();
}

BOOL XTimerMgr::Register(XTimerSinkHelper* pObj, UINT32 millisecond)
{
	ATLASSERT(NULL != pObj);
	map<XTimerSinkHelper*, UINT32>::iterator it = m_mapInverte.find(pObj);
	if (it != m_mapInverte.end())
	{
       ATLTRACE(_T("Waring Repeat Register Timer: %d\n"), millisecond);
        UnRegister(pObj);
	}

	UINT32 tid = GetFirstFreeTimerID();
	if (0 == tid)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	m_mapForward[tid] = pObj;
	m_mapInverte[pObj] = tid;

	HWND hWnd = GetGeneralMsgHWND();
	ATLASSERT(NULL != hWnd);
	INT nResult = ::SetTimer(hWnd, tid, millisecond, NULL);
	DWORD error = GetLastError();
	ATLASSERT(0 != nResult);
	return TRUE;
}

BOOL XTimerMgr::UnRegister(XTimerSinkHelper *pObj)
{
	ATLASSERT(NULL != pObj);
	map<XTimerSinkHelper*, UINT32>::iterator it = m_mapInverte.find(pObj);
	if (it == m_mapInverte.end())
	{
		return FALSE;
	}

	HWND hWnd = GetGeneralMsgHWND();
	if (::IsWindow(hWnd))
	{
		ATLASSERT(NULL != hWnd);
		::KillTimer(hWnd, it->second);
	}

	map<UINT32, XTimerSinkHelper*>::iterator it2 = m_mapForward.find(it->second);
	if (it2 != m_mapForward.end())
	{
		m_mapForward.erase(it2);
	}

	m_mapInverte.erase(it);

	return TRUE;
}

BOOL XTimerMgr::IsActive(XTimerSinkHelper *pObj)
{
	ATLASSERT(NULL != pObj);

	map<XTimerSinkHelper*, UINT32>::iterator it = m_mapInverte.find(pObj);
	if (it == m_mapInverte.end())
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

//
UINT32 XTimerMgr::GetFirstFreeTimerID()
{
	for (UINT32 i = 1; ; i++)
	{
		if (m_mapForward.find(i) == m_mapForward.end())
		{
			return i;
		}
	}

	return 0;
}

LRESULT XTimerMgr::OnGeneralMsgHandler(UINT32 msg_id, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{
	if (WM_TIMER == msg_id)
	{
		map<UINT32, XTimerSinkHelper*>::iterator it = m_mapForward.find(wParam);
		if (it != m_mapForward.end())
		{
			it->second->OnTimer();
			bHandle = TRUE;
		}
	}
	return 0;
}

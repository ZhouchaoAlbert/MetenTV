#include "stdafx.h"
#include "XTimerSinkHelper.h"
#include "XTimerMgr.h"
#include "Singleton.h"
#include "UtilLog.h"
#include <time.h>

XTimerSinkHelper::XTimerSinkHelper()
{
	m_szName[0] = 0;
}

XTimerSinkHelper::~XTimerSinkHelper()
{
	Singleton<XTimerMgr>::Instance().UnRegister(this);
}

void XTimerSinkHelper::OnTimer()
{
	time_t	lasttime = time(NULL);
//	Util::Log::Debug(_T("Timer"), _T("Log: %s - Now: %u"), m_szName, lasttime);

	evtOnTimer();

//	Util::Log::Debug(_T("Timer"), _T("Log: %s - Spend: %u"), m_szName, time(NULL) - lasttime);
}

void XTimerSinkHelper::Start(LPCTSTR pszName, UINT32 millisecond)
{
// 	if (!m_strName.IsEmpty())
// 	{
// 		if (m_strName.CompareNoCase(pszName))
// 		{
// 			Util::Log::Error(_T("Timer"), _T("Start NotMatch: %s - %s"), m_strName, pszName);
// 		}
// 	}
// 	m_strName = pszName;
	Singleton<XTimerMgr>::Instance().UnRegister(this);

	_sntprintf_s(m_szName, _countof(m_szName), _T("%s"), pszName);
	Singleton<XTimerMgr>::Instance().Register(this, millisecond);
}

void XTimerSinkHelper::Stop(LPCTSTR pszName)
{
//	EC_TRACE(_T("\nStoptTimer: %s 0x%x\n"), pszName, this);
	Singleton<XTimerMgr>::Instance().UnRegister(this);
}

BOOL XTimerSinkHelper::IsActive()
{
	return Singleton<XTimerMgr>::Instance().IsActive(this);
}

/////////////////
#include "WinMsgWnd.h"
XThreadTimerSinkHelper::XThreadTimerSinkHelper()
{
	m_szName[0] = 0;
	m_pWnd = NULL;
	m_bActive = FALSE;
}

XThreadTimerSinkHelper::~XThreadTimerSinkHelper()
{
	if (NULL != m_pWnd)
	{
		((CGeneralWindow*)m_pWnd)->UnhookEvent(evtOnGeneralMsg, this, &XThreadTimerSinkHelper::MsgHandle);
		delete (CGeneralWindow*)m_pWnd;
		m_pWnd = NULL;
	}
}

void XThreadTimerSinkHelper::OnThreadTimer()
{
	Util::Log::Debug(_T("Timer"), _T("Log: %s - %u"), m_szName, time(NULL));

	evtOnThreadTimer();
}

void XThreadTimerSinkHelper::Start(LPCTSTR pszName, UINT32 millisecond)
{
// 	if (!m_strName.IsEmpty())
// 	{
// 		if (m_strName.CompareNoCase(pszName))
// 		{
// 			Util::Log::Error(_T("Timer"), _T("Start NotMatch: %s - %s"), m_strName, pszName);
// 		}
// 	}
// 	m_strName = pszName;

	_sntprintf_s(m_szName, _countof(m_szName), _T("%s"), pszName);
	if (NULL == m_pWnd)
	{
		m_pWnd = new CGeneralWindow(this);
		if (NULL == m_pWnd)
		{
			ATLASSERT(FALSE);
			return;
		}
	}

	if (NULL != m_pWnd && !m_bActive)
	{
		((CGeneralWindow*)m_pWnd)->HookEvent(evtOnGeneralMsg, this, &XThreadTimerSinkHelper::MsgHandle);
		((CGeneralWindow*)m_pWnd)->SetTimer(1, millisecond, NULL);
		m_bActive = TRUE;
	}
}

void XThreadTimerSinkHelper::Stop(LPCTSTR pszName)
{
//	EC_TRACE(_T("\nStoptTimer: %s 0x%x\n"), pszName, this);
	if (NULL != m_pWnd && m_bActive)
	{
		m_bActive = FALSE;
		((CGeneralWindow*)m_pWnd)->KillTimer(1);
		((CGeneralWindow*)m_pWnd)->UnhookEvent(evtOnGeneralMsg, this, &XThreadTimerSinkHelper::MsgHandle);
	}
}

BOOL XThreadTimerSinkHelper::IsActive()
{
	return 	m_bActive;
}

void	XThreadTimerSinkHelper::MsgHandle(void* pObj, UINT32 msg_id, WPARAM wParam, LPARAM lParam, BOOL& bHandle, LRESULT& lResult)
{
	ATLASSERT(NULL != pObj);
	if (WM_TIMER == msg_id)
	{
		ATLASSERT(1 == wParam);

		OnThreadTimer();
	}
}

void XThreadTimerSinkHelper::On_GeneralWinDestroyEvent(void* pVoid)
{
	m_pWnd = NULL;
	m_bActive = FALSE;
}
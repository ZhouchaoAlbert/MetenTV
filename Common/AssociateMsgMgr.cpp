#include "stdafx.h"
#include "AssociateMsgMgr.h"
#include "Singleton.h"

CAssociateMsgMgr::CAssociateMsgMgr()
{
	m_pGeneralWindow = NULL;
}

CAssociateMsgMgr::~CAssociateMsgMgr()
{
	if (m_pGeneralWindow)
	{
		m_pGeneralWindow->UnhookEvent(evtOnGeneralMsg, this, &CAssociateMsgMgr::MsgHandle);
		delete m_pGeneralWindow;
	}

	m_mapForward.clear();
	m_mapInverte.clear();
}

UINT32 CAssociateMsgMgr::GetFirstFreeMsgID()
{
	for (UINT32 i = WM_USER + 0x100;; i++)
	{
		if (m_mapInverte.find(i) == m_mapInverte.end())
		{
			return i;
		}
	}

	return 0;
}

UINT32	CAssociateMsgMgr::RegAssociateMsg(Util::WinMsg::CAssociateMsgSinkHelper* pObj)
{
	ATLASSERT(NULL != pObj);

	map<Util::WinMsg::CAssociateMsgSinkHelper*, UINT32>::iterator it = m_mapForward.find(pObj);
	if (it != m_mapForward.end())
	{
		ATLASSERT(FALSE);
		return it->second;
	}

	UINT32 u32MsgID = GetFirstFreeMsgID();
	if (0 == u32MsgID)
	{
		ATLASSERT(FALSE);
		return 0;
	}

	//´¥·¢ÉúÐ§
	HWND hWnd = GetAssociateMsgHWND();
	if (!hWnd)
	{
		ATLASSERT(FALSE);
		return 0;
	}

	m_mapForward[pObj] = u32MsgID;
	m_mapInverte[u32MsgID] = pObj;
	
	return u32MsgID;
}
void	CAssociateMsgMgr::UnRegAssociateMsg(Util::WinMsg::CAssociateMsgSinkHelper* pObj)
{
	ATLASSERT(NULL != pObj);

	map<Util::WinMsg::CAssociateMsgSinkHelper*, UINT32>::iterator it = m_mapForward.find(pObj);
	if (it == m_mapForward.end())
	{
//		ATLASSERT(FALSE);
		return;
	}

	map<UINT32, Util::WinMsg::CAssociateMsgSinkHelper*>::iterator it2 = m_mapInverte.find(it->second);
	ATLASSERT(it2 != m_mapInverte.end());

	m_mapInverte.erase(it2);
	m_mapForward.erase(it);
}

HWND	CAssociateMsgMgr::GetAssociateMsgHWND()
{
	if (!m_pGeneralWindow)
	{
		m_pGeneralWindow = new CGeneralWindow(this);	
		ATLASSERT(m_pGeneralWindow);
		m_pGeneralWindow->HookEvent(evtOnGeneralMsg, this, &CAssociateMsgMgr::MsgHandle);
	}
	if (m_pGeneralWindow)
	{
		return m_pGeneralWindow->GetGeneralMsgHWND();
	}

	return NULL;
}

void CAssociateMsgMgr::On_GeneralWinDestroyEvent(void* pVoid)
{
	m_pGeneralWindow = NULL;
}

void	CAssociateMsgMgr::MsgHandle(void* pObj, UINT32 msg_id, WPARAM wParam, LPARAM lParam, BOOL& bHandle, LRESULT& lResult)
{
	CGeneralWindow*	p = (CGeneralWindow*)pObj;
	ATLASSERT(NULL != p);

	map<UINT32, Util::WinMsg::CAssociateMsgSinkHelper*>::iterator it2 = m_mapInverte.find(msg_id);
	if (it2 == m_mapInverte.end())
	{
//		ATLASSERT(FALSE);
		return;
	}
	
	lResult = it2->second->OnAssociateMsg(msg_id, wParam, lParam);
	bHandle = TRUE;
}

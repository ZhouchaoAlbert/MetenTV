#include "stdafx.h"
#include "WinMsgMgr.h"
#include "Singleton.h"
#include <vector>

CWinMsgMgr::CWinMsgMgr()
{

}

CWinMsgMgr::~CWinMsgMgr()
{
	vector<CGeneralWindow*>	vTemp;
	for (map<Util::WinMsg::CGeneralMsgBase*, CGeneralWindow*>::iterator it = m_mapForward.begin(); it != m_mapForward.end(); it++)
	{
		it->second->UnhookEvent(evtOnGeneralMsg, this, &CWinMsgMgr::MsgHandle);
		vTemp.push_back(it->second);
	}
	m_mapForward.clear();
	m_mapInverte.clear();
	for (vector<CGeneralWindow*>::iterator it = vTemp.begin(); it != vTemp.end(); it++)
	{
		delete *it;
	}
}

BOOL	CWinMsgMgr::RegGeneralMsg(Util::WinMsg::CGeneralMsgBase* pObj)
{
	ATLASSERT(NULL != pObj);

	map<Util::WinMsg::CGeneralMsgBase*, CGeneralWindow*>::iterator it = m_mapForward.find(pObj);
	if (it != m_mapForward.end())
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	CGeneralWindow* pWnd = new CGeneralWindow(this);
	if (NULL == pWnd)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}
	m_mapForward[pObj] = pWnd;
	m_mapInverte[pWnd] = pObj;

	//创建等消息不会Hook，因为New->Create在前
	pWnd->HookEvent(evtOnGeneralMsg, this, &CWinMsgMgr::MsgHandle);

	return TRUE;
}

BOOL	CWinMsgMgr::UnRegGeneralMsg(Util::WinMsg::CGeneralMsgBase* pObj)
{
	ATLASSERT(NULL != pObj);

	map<Util::WinMsg::CGeneralMsgBase*, CGeneralWindow*>::iterator it = m_mapForward.find(pObj);
	if (it == m_mapForward.end())
	{
//		ATLASSERT(FALSE);
		return FALSE;
	}

	CGeneralWindow*	pWin = it->second;
	
	map<CGeneralWindow*, Util::WinMsg::CGeneralMsgBase*>::iterator it2 = m_mapInverte.find(it->second);
	ATLASSERT(it2 != m_mapInverte.end());
	m_mapInverte.erase(it2);	
	m_mapForward.erase(it);

	pWin->UnhookEvent(evtOnGeneralMsg, this, &CWinMsgMgr::MsgHandle);
	delete pWin;
	
	return TRUE;
}

HWND	CWinMsgMgr::GetGeneralMsgHWND(Util::WinMsg::CGeneralMsgBase* pObj)
{
	ATLASSERT(NULL != pObj);

	map<Util::WinMsg::CGeneralMsgBase*, CGeneralWindow*>::iterator it = m_mapForward.find(pObj);
	if (it != m_mapForward.end())
	{
		return it->second->GetGeneralMsgHWND();
	}

	return NULL;
}

void CWinMsgMgr::On_GeneralWinDestroyEvent(void* pVoid)
{
	CGeneralWindow* pWnd = (CGeneralWindow*)pVoid;
	ATLASSERT(NULL != pWnd);

	map<CGeneralWindow*, Util::WinMsg::CGeneralMsgBase*>::iterator it2 = m_mapInverte.find(pWnd);
	if (it2 == m_mapInverte.end())
	{
		return;
	}

	map<Util::WinMsg::CGeneralMsgBase*, CGeneralWindow*>::iterator it = m_mapForward.find(it2->second);
	if (it == m_mapForward.end())
	{
		ATLASSERT(FALSE);
		return;
	}

	ATLASSERT(it != m_mapForward.end());
	m_mapInverte.erase(it2);
	m_mapForward.erase(it);
}

void	CWinMsgMgr::MsgHandle(void* pObj, UINT32 msg_id, WPARAM wParam, LPARAM lParam, BOOL& bHandle, LRESULT& lResult)
{
	CGeneralWindow*	p = (CGeneralWindow*)pObj;
	ATLASSERT(NULL != p);

	map<CGeneralWindow*, Util::WinMsg::CGeneralMsgBase*>::iterator it2 = m_mapInverte.find(p);
	if (it2 == m_mapInverte.end())
	{
		ATLASSERT(FALSE);
		return;
	}

	lResult = it2->second->OnGeneralMsgHandler(msg_id, wParam, lParam, bHandle);
}

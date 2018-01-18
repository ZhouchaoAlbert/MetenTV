#include "stdafx.h"
#include "UtilWinMsg.h"
#include "Singleton.h"
#include "WinMsgMgr.h"
#include "AssociateMsgMgr.h"

Util::WinMsg::CGeneralMsgBase::CGeneralMsgBase()
{
	Singleton<CWinMsgMgr>::Instance().RegGeneralMsg(this);
}

Util::WinMsg::CGeneralMsgBase::~CGeneralMsgBase()
{
	Singleton<CWinMsgMgr>::Instance().UnRegGeneralMsg(this);
}

HWND Util::WinMsg::CGeneralMsgBase::GetGeneralMsgHWND()
{
	return Singleton<CWinMsgMgr>::Instance().GetGeneralMsgHWND(this);
}

/////////////
Util::WinMsg::CAssociateMsgSinkHelper::CAssociateMsgSinkHelper()
{
	m_u32AssociateMsgID = 0;
}

Util::WinMsg::CAssociateMsgSinkHelper::~CAssociateMsgSinkHelper()
{
	Singleton<CAssociateMsgMgr>::Instance().UnRegAssociateMsg(this);
}

LRESULT Util::WinMsg::CAssociateMsgSinkHelper::OnAssociateMsg(UINT32 msg_id, WPARAM wParam, LPARAM lParam)//目前不考虑LRESULT返回值，现在用途仅仅是被动通知
{
	ATLASSERT(m_u32AssociateMsgID == msg_id);
	evtOnAssociateMsg(wParam, lParam);

	return 0;
}

BOOL Util::WinMsg::CAssociateMsgSinkHelper::Enable()
{
	if (!m_u32AssociateMsgID)
	{
		m_u32AssociateMsgID = Singleton<CAssociateMsgMgr>::Instance().RegAssociateMsg(this);
	}

	return IsEnable();
}

void Util::WinMsg::CAssociateMsgSinkHelper::Disble()
{
	Singleton<CAssociateMsgMgr>::Instance().UnRegAssociateMsg(this);
	m_u32AssociateMsgID = 0;
}

BOOL Util::WinMsg::CAssociateMsgSinkHelper::IsEnable()
{
	return 0 != m_u32AssociateMsgID;
}

HWND Util::WinMsg::CAssociateMsgSinkHelper::GetHwnd()
{
	Enable();
	if (IsEnable())
	{
		return Singleton<CAssociateMsgMgr>::Instance().GetAssociateMsgHWND();
	}
	return NULL;
}

UINT32 Util::WinMsg::CAssociateMsgSinkHelper::GetMsgID()
{
	Enable();
	if (IsEnable())
	{
		return m_u32AssociateMsgID;
	}
	return 0;
}
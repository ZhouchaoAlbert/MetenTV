#pragma once

#include "UtilWinMsg.h"
#include "WinMsgWnd.h"

#include <map>
using namespace ATL;
using namespace std;

class CAssociateMsgMgr : public Util::WinMsg::CGeneralWinDestroyEvent
{
public:
	DECLARE_EVENT_RECEIVER(CAssociateMsgMgr)

	CAssociateMsgMgr();
	virtual ~CAssociateMsgMgr();
			
	UINT32	RegAssociateMsg(Util::WinMsg::CAssociateMsgSinkHelper* pObj);
	void	UnRegAssociateMsg(Util::WinMsg::CAssociateMsgSinkHelper* pObj);
	HWND	GetAssociateMsgHWND();

	void	MsgHandle(void* pObj, UINT32 msg_id, WPARAM wParam, LPARAM lParam, BOOL& bHandle, LRESULT& lResult);
	virtual void On_GeneralWinDestroyEvent(void* pVoid);
private:	
	map<Util::WinMsg::CAssociateMsgSinkHelper*, UINT32>	m_mapForward;
	map<UINT32, Util::WinMsg::CAssociateMsgSinkHelper*>	m_mapInverte;

	CGeneralWindow*	m_pGeneralWindow;
	UINT32 GetFirstFreeMsgID();
};


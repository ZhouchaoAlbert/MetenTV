#pragma once


#include "UtilWinMsg.h"
#include "WinMsgWnd.h"

#include <map>

using namespace ATL;
using namespace std;

class CWinMsgMgr : public Util::WinMsg::CGeneralWinDestroyEvent
{
public:
	DECLARE_EVENT_RECEIVER(CWinMsgMgr)

	CWinMsgMgr();
	virtual ~CWinMsgMgr();
			
	BOOL	RegGeneralMsg(Util::WinMsg::CGeneralMsgBase* pObj);
	BOOL	UnRegGeneralMsg(Util::WinMsg::CGeneralMsgBase* pObj);
	HWND	GetGeneralMsgHWND(Util::WinMsg::CGeneralMsgBase* pObj);

	void	MsgHandle(void* pObj, UINT32 msg_id, WPARAM wParam, LPARAM lParam, BOOL& bHandle, LRESULT& lResult);

	virtual void On_GeneralWinDestroyEvent(void* pVoid);
private:	
	map<Util::WinMsg::CGeneralMsgBase*, CGeneralWindow*>	m_mapForward;
	map<CGeneralWindow*, Util::WinMsg::CGeneralMsgBase*>	m_mapInverte;
};


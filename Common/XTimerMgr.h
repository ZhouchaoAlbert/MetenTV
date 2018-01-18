#pragma once

#include "XTimerSinkHelper.h"
#include "UtilWinMsg.h"
#include <map>

using namespace std;

class XTimerMgr : public Util::WinMsg::CGeneralMsgBase
{
public:
	XTimerMgr();
	virtual ~XTimerMgr();

	//注册一个事件；
	BOOL Register(XTimerSinkHelper* pObj, UINT32 millisecond);
	//反注册一个指定的事件；
	BOOL UnRegister(XTimerSinkHelper *pObj);

	BOOL IsActive(XTimerSinkHelper *pObj);
	
private:
	virtual LRESULT OnGeneralMsgHandler(UINT32 msg_id, WPARAM wParam, LPARAM lParam, BOOL& bHandle);

	//获取事件ID号
	UINT32  GetFirstFreeTimerID();

	map<UINT32, XTimerSinkHelper*> m_mapForward;
	map<XTimerSinkHelper*, UINT32> m_mapInverte;
};


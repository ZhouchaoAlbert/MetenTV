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

	//ע��һ���¼���
	BOOL Register(XTimerSinkHelper* pObj, UINT32 millisecond);
	//��ע��һ��ָ�����¼���
	BOOL UnRegister(XTimerSinkHelper *pObj);

	BOOL IsActive(XTimerSinkHelper *pObj);
	
private:
	virtual LRESULT OnGeneralMsgHandler(UINT32 msg_id, WPARAM wParam, LPARAM lParam, BOOL& bHandle);

	//��ȡ�¼�ID��
	UINT32  GetFirstFreeTimerID();

	map<UINT32, XTimerSinkHelper*> m_mapForward;
	map<XTimerSinkHelper*, UINT32> m_mapInverte;
};


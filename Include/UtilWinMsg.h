#pragma once

#include "BaseDefine.h"
#include "EventSupport.h"
#include <atlwin.h>
using namespace ATL;

namespace Util
{
	namespace WinMsg
	{	
		//内部辅助
		class COMMON_API CGeneralWinDestroyEvent
		{
		public:
			CGeneralWinDestroyEvent(){}
			virtual ~CGeneralWinDestroyEvent(){}
			virtual void On_GeneralWinDestroyEvent(void* pVoid) = 0;
		};

		//创建一个窗口
		class COMMON_API CGeneralMsgBase
		{
		public:
			CGeneralMsgBase();
			virtual ~CGeneralMsgBase();

			HWND	GetGeneralMsgHWND();
			virtual LRESULT OnGeneralMsgHandler(UINT32 msg_id, WPARAM wParam, LPARAM lParam, BOOL& bHandle) = 0;
		};

		class  COMMON_API CAssociateMsgSinkHelper
		{
		public:
			CAssociateMsgSinkHelper();
			virtual ~CAssociateMsgSinkHelper();

			//伴生消息，本非WM_TIMER，而是申请的WM_USER以上消息，并保持全局唯一性
			EVENT(evtOnAssociateMsg, (WPARAM wParam, LPARAM lParam), (wParam, lParam));
			LRESULT OnAssociateMsg(UINT32 msg_id, WPARAM wParam, LPARAM lParam);//目前不考虑LRESULT返回值，现在用途仅仅是被动通知
			BOOL Enable();
			void Disble();
			BOOL IsEnable();
			HWND GetHwnd();
			UINT32 GetMsgID();
		private:
			UINT32	m_u32AssociateMsgID;//为0表示Disabled
		};
	}
}



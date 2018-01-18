#pragma once

#include "BaseDefine.h"
#include "EventSupport.h"
#include <atlwin.h>
using namespace ATL;

namespace Util
{
	namespace WinMsg
	{	
		//�ڲ�����
		class COMMON_API CGeneralWinDestroyEvent
		{
		public:
			CGeneralWinDestroyEvent(){}
			virtual ~CGeneralWinDestroyEvent(){}
			virtual void On_GeneralWinDestroyEvent(void* pVoid) = 0;
		};

		//����һ������
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

			//������Ϣ������WM_TIMER�����������WM_USER������Ϣ��������ȫ��Ψһ��
			EVENT(evtOnAssociateMsg, (WPARAM wParam, LPARAM lParam), (wParam, lParam));
			LRESULT OnAssociateMsg(UINT32 msg_id, WPARAM wParam, LPARAM lParam);//Ŀǰ������LRESULT����ֵ��������;�����Ǳ���֪ͨ
			BOOL Enable();
			void Disble();
			BOOL IsEnable();
			HWND GetHwnd();
			UINT32 GetMsgID();
		private:
			UINT32	m_u32AssociateMsgID;//Ϊ0��ʾDisabled
		};
	}
}



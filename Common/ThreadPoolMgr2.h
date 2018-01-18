#pragma once

#include <map>
#include "UtilThreadPool2.h"
#include "Utilkobj.h"
#include "UtilWinMsg.h"

using namespace Util::ThreadPool2;
using namespace std;

class CThreadPoolMgr2
{
public:
	CThreadPoolMgr2();
	virtual ~CThreadPoolMgr2();

	void		SetThreadNum(ThreadSlotClass cls, UINT8 high);
	UINT64		AddTask(ThreadSlotClass cls, IThreadObjSink* pISink, IThreadObjWork* pIWork, void* pThreadParam);
	void		DelTask(UINT64 id);
	void		PreDelAllTask();

	//Ϊ�˱������Ľ��������ڶ�Ӧ���ٽ��������ǳ��Ըĳ�static����
	static CCriticalSection	m_csLock;
	UINT32				m_highThreadNum;

	//�޸����⣬���������� ��ʱ��δ�����꣩  �ⲿ�ٴε��ýӿڵ�ʱ��  ���ܻᵼ�����´����߳� ���³������  ��� �ӽӿڿ�����
	static BOOL m_bValid;       //�����Ƿ����
	static BOOL IsObjectValid();
private:
	UINT64		m_u64Counter;
	struct TaskItem
	{
		ThreadSlotClass	cls;
		UINT64			id;
		IThreadObjSink*	pISink;
		IThreadObjWork*	pIWork;
		UINT32			dwThreadID;
		void*			pThreadParam;
		HWND		hNotifyDelWnd;
		UINT32		u32NotifyDelMsg;
	};
	map<IThreadObjSink*, TaskItem*>	m_mapSink;
	map<IThreadObjWork*, TaskItem*>	m_mapWork;
	map<UINT64, TaskItem*>			m_mapThread;

	DECLARE_EVENT_RECEIVER(CThreadPoolMgr2)
	Util::WinMsg::CAssociateMsgSinkHelper	m_shWinMsg;
	void OnNotifyDelTask(WPARAM wParam, LPARAM lParam);

	void		CheckThreadNum_InLock(ThreadSlotClass cls);//ǿ����ֹ�̺߳���Ҫ���������������������
	void		DelTask_InLock(TaskItem* pItem);	

	map<ThreadSlotClass, UINT8>	m_mapLimit;
	void	GetCalcByCls_InLock(ThreadSlotClass cls, UINT32* pu32LimitNum, UINT32* pu32ThreadNum, UINT32* pu32SinkNum);

	static UINT32 WINAPI WorkThreadProc(LPVOID pv);
};


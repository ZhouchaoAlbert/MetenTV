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

	//为了避免挂起的进程依赖于对应的临界区，我们尝试改成static试试
	static CCriticalSection	m_csLock;
	UINT32				m_highThreadNum;

	//修复问题，当对象析够 的时候（未析够完）  外部再次调用接口的时候  可能会导致重新创建线程 导致程序崩溃  这边 加接口控制下
	static BOOL m_bValid;       //对象是否可用
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

	void		CheckThreadNum_InLock(ThreadSlotClass cls);//强制终止线程后，需要调这个函数进行修正操作
	void		DelTask_InLock(TaskItem* pItem);	

	map<ThreadSlotClass, UINT8>	m_mapLimit;
	void	GetCalcByCls_InLock(ThreadSlotClass cls, UINT32* pu32LimitNum, UINT32* pu32ThreadNum, UINT32* pu32SinkNum);

	static UINT32 WINAPI WorkThreadProc(LPVOID pv);
};


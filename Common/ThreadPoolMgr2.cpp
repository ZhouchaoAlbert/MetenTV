#include "stdafx.h"
#include "ThreadPoolMgr2.h"
#include "Singleton.h"

#define THREADNUM_DEFAULT		5
#define SLEEPTHREAD_WAITTIME    (200)
#define SLEEPTHREAD_PROCEXIT    (1000)

CCriticalSection	CThreadPoolMgr2::m_csLock;
BOOL CThreadPoolMgr2::m_bValid = TRUE;

CThreadPoolMgr2::CThreadPoolMgr2()
{
	//m_bValid = TRUE;
	m_u64Counter = 0;

	m_shWinMsg.HookEvent(evtOnAssociateMsg, this, &CThreadPoolMgr2::OnNotifyDelTask);
}

CThreadPoolMgr2::~CThreadPoolMgr2()
{
	m_bValid = FALSE;

	m_shWinMsg.UnhookEvent(evtOnAssociateMsg, this, &CThreadPoolMgr2::OnNotifyDelTask);

	Util::Log::Info(_T("~CThreadPoolMgr2"), _T("Enter Left Thread Num: %u, Sink/Work Num: %u\n"), m_mapThread.size(), m_mapSink.size());
	ATLTRACE(_T("Enter Left Thread Num: %u, Sink/Work Num: %u\n"), m_mapThread.size(), m_mapSink.size());

	//ע��m_csLock�����򣬱�����౾����ɾ��
	//�˳�֮ǰ����������Ӧ���ϲ��ѽ��˳���
	m_mapLimit.clear();
	PreDelAllTask();

	//ɾ����ϵ
	do
	{
		Util::kObj::CAutoCriticalSection lock(&m_csLock);

		//Ϊ�˱���ͬ����ȫ������
		for (map<UINT64, TaskItem*>::iterator itThread = m_mapThread.begin(); itThread != m_mapThread.end(); itThread++)
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, LODWORD_UINT64(itThread->first));
			ATLASSERT(hThread);
			::SuspendThread(hThread);
			::CloseHandle(hThread);
			//�����ǹ�����Taskɾ�����
		}

		for (map<IThreadObjSink*, TaskItem*>::iterator itSink = m_mapSink.begin(); itSink != m_mapSink.end();)
		{
			ATLASSERT(itSink->first);

			TaskItem* pTaskItem = itSink->second;
			ATLASSERT(pTaskItem);

			pTaskItem->pISink->FetchAllMsg();
			if (!pTaskItem->dwThreadID && !pTaskItem->pIWork->CheckExit_InWorkThread())
			{
				map<IThreadObjWork*, TaskItem*>::iterator itWork = m_mapWork.find(pTaskItem->pIWork);
				ATLASSERT(itWork != m_mapWork.end());
				m_mapWork.erase(itWork);				
				itSink = m_mapSink.erase(itSink);
				
				if (pTaskItem->pThreadParam)
				{
					pTaskItem->pISink->OnFreeParam(pTaskItem->pThreadParam);
				}
				delete pTaskItem->pISink;
				delete pTaskItem->pIWork;
				delete pTaskItem;
			}
			else
			{
				itSink++;
			}
		}
	} while (0);

	//ATLTRACE(_T("Leave Left Thread Num: %u, Sink/Work Num: %u\n"), m_mapThread.size(), m_mapSink.size());
}

void		CThreadPoolMgr2::SetThreadNum(ThreadSlotClass cls, UINT8 high)
{
	Util::kObj::CAutoCriticalSection lock(&m_csLock);

	if (high >= 1)
	{
		m_mapLimit[cls] = high;

		CheckThreadNum_InLock(cls);
	}
	else
	{
		ATLASSERT(FALSE);
	}
}

UINT64		CThreadPoolMgr2::AddTask(ThreadSlotClass cls, IThreadObjSink* pISink, IThreadObjWork* pIWork, void* pThreadParam)	//ע��pParam�����ڣ������new����ָ�룬��������Ϊsink��һ����Ա����������������ʱ���ͷ�
{
	ATLASSERT(cls != TSC_NONE);
	ATLASSERT(pISink);
	ATLASSERT(pIWork);

	Util::kObj::CAutoCriticalSection lock(&m_csLock);

	//����Ƿ��ظ�

	map<IThreadObjSink*, TaskItem*>::iterator itSink = m_mapSink.find(pISink);
	if (itSink != m_mapSink.end())
	{
		ATLASSERT(FALSE);

		return 0;
	}
	map<IThreadObjWork*, TaskItem*>::iterator itWork = m_mapWork.find(pIWork);
	if (itWork != m_mapWork.end())
	{
		ATLASSERT(FALSE);

		return 0;
	}

	//Stub������u32ThousandSecondCheckInterval��tbd
	TaskItem*	t = new TaskItem;
	if (NULL == t)
	{
		ATLASSERT(FALSE);
		return 0;
	}
	
	t->cls = cls;
	t->id = (++m_u64Counter);
	t->pIWork = pIWork;
	t->pISink = pISink;
	t->dwThreadID = 0;
	t->pThreadParam = pThreadParam;
	t->hNotifyDelWnd = m_shWinMsg.GetHwnd();
	t->u32NotifyDelMsg = m_shWinMsg.GetMsgID();

	//Sink-Work������ϵ
	HWND	Hwnd = NULL;
	UINT32	MsgID = 0;
	void*	pC2W = NULL;
	void*	pW2C = NULL;
	UINT32	WaitSleep = 0;

	pISink->SetTaskID(t->id);
	pIWork->SetTaskID(t->id);
	if (!pISink->GetSinkMeta(&Hwnd, &MsgID, &WaitSleep, &pC2W, &pW2C))
	{
		delete t;
		ATLASSERT(FALSE);
		return 0;
	}
	if(!pIWork->SetSinkMeta(Hwnd, MsgID, WaitSleep, pC2W, pW2C))
	{
		delete t;
		ATLASSERT(FALSE);
		return 0;
	}

	m_mapSink[pISink] = t;
	m_mapWork[pIWork] = t;

	map<ThreadSlotClass, UINT8>::iterator itLimit = m_mapLimit.find(cls);
	if (itLimit == m_mapLimit.end())
	{
		m_mapLimit[cls] = THREADNUM_DEFAULT;
	}

	CheckThreadNum_InLock(cls);

	return t->id;
}

void		CThreadPoolMgr2::DelTask(UINT64 id)	//Sink��Work���󲢲����ڲ�ɾ���������������ѹ�
{
	ATLASSERT(id);
	do
	{
		BOOL bFind = FALSE;
		Util::kObj::CAutoCriticalSection lock(&m_csLock);//��ͬ�߳��ظ����ٽ���û����

		for (map<IThreadObjSink*, TaskItem*>::iterator itSink = m_mapSink.begin(); itSink != m_mapSink.end(); itSink++)
		{
			if (itSink->second->id == id)
			{
				itSink->second->pISink->PostExitMsg();
				bFind = TRUE;
				break;
			}
		}
		if (!bFind)
		{
			return;
		}
	} while (0);
	
	Sleep(KILLTHREAD_WAITTIME);

	do
	{
		Util::kObj::CAutoCriticalSection lock(&m_csLock);

		for (map<IThreadObjSink*, TaskItem*>::iterator itSink = m_mapSink.begin(); itSink != m_mapSink.end(); itSink++)
		{
			if (itSink->second->id == id)
			{
				ThreadSlotClass cls = itSink->second->cls;
				DelTask_InLock(itSink->second);
				CheckThreadNum_InLock(cls);
				return;
			}
		}
	} while (0);
}

void		CThreadPoolMgr2::PreDelAllTask()
{
	BOOL bEmpty = TRUE;

	do
	{
		Util::kObj::CAutoCriticalSection lock(&m_csLock);//��ͬ�߳��ظ����ٽ���û����

		for (map<IThreadObjSink*, TaskItem*>::iterator it = m_mapSink.begin(); it != m_mapSink.end(); it++)
		{
			ATLASSERT(it->first);
			ATLASSERT(it->second);
			if (it->second->dwThreadID)
			{
				it->first->PostExitMsg();
				
				bEmpty = FALSE;
			}
		}
	} while (0);

	Util::Log::Info(_T("Thread"), _T("Enter Left Thread Num: %u, Sink/Work Num: %u, bEmpty: %u\n"), m_mapThread.size(), m_mapSink.size(), bEmpty);
	if (!bEmpty)
	{
		Sleep(SLEEPTHREAD_PROCEXIT);
	}
}

UINT32 WINAPI CThreadPoolMgr2::WorkThreadProc(LPVOID pv)
{
	UINT32 dwThreadID = GetCurrentThreadId();
	
	ThreadSlotClass		cls = (ThreadSlotClass)(UINT32)pv;
	CThreadPoolMgr2*	mgr = &Singleton<CThreadPoolMgr2>::Instance();

//	Sleep(30 * 1000);

	//�����жϵ�ǰ״̬��
	while (true)
	{
		TaskItem * pTaskItem = NULL;

		//Ϊ�����������ڹ���������ע�������ע�ͻ��Ż�
		//�ȿ���ǰ�߳��Ƿ�̫���ˣ�
		do{
			Util::kObj::CAutoCriticalSection lock(&CThreadPoolMgr2::m_csLock);

			//1 �߳�̫���ˣ�
			UINT32 u32LimitNum = 0;
			UINT32 u32ThreadNum = 0;
			UINT32 u32SinkNum = 0;
			mgr->GetCalcByCls_InLock(cls, &u32LimitNum, &u32ThreadNum, &u32SinkNum);
			if (u32LimitNum < u32ThreadNum)
			{
				map<UINT64, TaskItem*>::iterator itThread = mgr->m_mapThread.find(MAKE_UINT64(cls, dwThreadID));
				ATLASSERT(itThread != mgr->m_mapThread.end());
				ATLASSERT(NULL == itThread->second);
				mgr->m_mapThread.erase(itThread);

				return 0;
			}

			if (u32SinkNum < u32ThreadNum)
			{
				map<UINT64, TaskItem*>::iterator itThread = mgr->m_mapThread.find(MAKE_UINT64(cls, dwThreadID));
				ATLASSERT(itThread != mgr->m_mapThread.end());
				ATLASSERT(NULL == itThread->second);
				mgr->m_mapThread.erase(itThread);

				return 0;
			}

			//2 ����Ѱ����task
			for (map<IThreadObjSink*, TaskItem*>::iterator it = mgr->m_mapSink.begin(); it != mgr->m_mapSink.end(); it++)
			{
				ATLASSERT(it->first);
				ATLASSERT(it->second);
				if (!it->second->dwThreadID && !it->second->pIWork->CheckExit_InWorkThread())
				{
					map<UINT64, TaskItem*>::iterator itThread = mgr->m_mapThread.find(MAKE_UINT64(cls, dwThreadID));
					ATLASSERT(itThread != mgr->m_mapThread.end());
					
					itThread->second = it->second;
					it->second->dwThreadID = dwThreadID;

					pTaskItem = it->second;
					break;
				}
			}
		} while (0);

		if (!pTaskItem)
		{
			Sleep(SLEEPTHREAD_WAITTIME);
			continue;
		}

		//3 ִ�У���ʾ�ҵ���Ŀ��
		if (pTaskItem)		//�߳�ִ�е�����������ף�������������Ч����ô����̱߳�Ȼ��Ч����ΪDelTask������ֹ�߳���ɾ�����������ⲿ��ȷ������һ����
		{
			ATLASSERT(pTaskItem->pIWork);
			pTaskItem->pIWork->Run_InWorkThread(pTaskItem->pThreadParam);
			pTaskItem->pIWork->NotifyThreadFinish_InWorkThread();
			//���ֱ��Sinkȷ��Work�Ƴ���
			while (!pTaskItem->pIWork->FilterExit_InWorkThread())
			{
				Sleep(Util::ThreadPool2::EXITTHREADCONFORM_WAITTIME);
			}
		}

		//4 ����
		do{
			//������������ᵼ�������߳̿�������п��У���ȥ���������
			Util::kObj::CAutoCriticalSection lock(&CThreadPoolMgr2::m_csLock);

			pTaskItem->dwThreadID = 0;	

			map<UINT64, TaskItem*>::iterator itThread = mgr->m_mapThread.find(MAKE_UINT64(cls, dwThreadID));
			ATLASSERT(itThread != mgr->m_mapThread.end());
			itThread->second = NULL;

			ATLASSERT(::IsWindow(pTaskItem->hNotifyDelWnd));
			::PostMessage(pTaskItem->hNotifyDelWnd, pTaskItem->u32NotifyDelMsg, (WPARAM)pTaskItem->pISink, 0L);
		} while (0);
	}

	return 0;
}

//�ⲿ������Ӧ�����������������ﲻ�ظ�����
void		CThreadPoolMgr2::CheckThreadNum_InLock(ThreadSlotClass cls)//ǿ����ֹ�̺߳���Ҫ���������������������
{	
	UINT32 u32LimitNum = 0;
	UINT32 u32ThreadNum = 0;
	UINT32 u32SinkNum = 0;
	GetCalcByCls_InLock(cls, &u32LimitNum, &u32ThreadNum, &u32SinkNum);
	//��ǰ�߳�������������������
	//ATLASSERT(u32ThreadNum <= u32SinkNum);
	for (UINT32 i = u32ThreadNum; i < my_min(u32LimitNum, u32SinkNum); i++)
	{
		UINT32	dwThreadID = 0;
		HANDLE	hThread = (HANDLE)_beginthreadex(NULL, 0, &WorkThreadProc, (PVOID)(UINT32)cls, CREATE_SUSPENDED, &dwThreadID);
		if (NULL == hThread)
		{
			ATLASSERT(FALSE);
			return;
		}
		ATLASSERT(dwThreadID);
		map<UINT64, TaskItem*>::iterator it = m_mapThread.find(MAKE_UINT64(cls, dwThreadID));
		ATLASSERT(it == m_mapThread.end());
		m_mapThread[MAKE_UINT64(cls, dwThreadID)] = NULL;
		ResumeThread(hThread);
		CloseHandle(hThread);
	}
}

void	CThreadPoolMgr2::GetCalcByCls_InLock(ThreadSlotClass cls, UINT32* pu32LimitNum, UINT32* pu32ThreadNum, UINT32* pu32SinkNum)
{
	UINT32 u32LimitNum = 0;
	UINT32 u32ThreadNum = 0;
	UINT32 u32SinkNum = 0;

	map<ThreadSlotClass, UINT8>::iterator itLimit =	m_mapLimit.find(cls);
//	ATLASSERT(itLimit != m_mapLimit.end());
	if (itLimit != m_mapLimit.end())
	{
		u32LimitNum = itLimit->second;
	}
	for (map<UINT64, TaskItem*>::iterator itThread = m_mapThread.begin(); itThread != m_mapThread.end(); itThread++)
	{
		if ((ThreadSlotClass)HIDWORD_UINT64(itThread->first) == cls)
		{
			u32ThreadNum++;
		}
	}
	for (map<IThreadObjSink*, TaskItem*>::iterator itSink = m_mapSink.begin(); itSink != m_mapSink.end(); itSink++)
	{
		if (itSink->second->cls == cls)
		{
			u32SinkNum++;
		}
	}

	*pu32LimitNum = u32LimitNum;
	*pu32ThreadNum = u32ThreadNum;
	*pu32SinkNum = u32SinkNum;
}

void		CThreadPoolMgr2::DelTask_InLock(TaskItem* pItem)
{
	ATLASSERT(pItem);
	ATLASSERT(pItem->pISink);
	ATLASSERT(pItem->pIWork);

	//����Ƿ��ظ�
	map<IThreadObjSink*, TaskItem*>::iterator itSink = m_mapSink.find(pItem->pISink);
	if (itSink == m_mapSink.end())
	{
		//		ATLASSERT(FALSE);

		return;
	}

	ATLASSERT(itSink->second);
	ATLASSERT(itSink->second->pIWork);
	map<IThreadObjWork*, TaskItem*>::iterator itWork = m_mapWork.find(pItem->pIWork);
	if (itWork == m_mapWork.end())
	{
		ATLASSERT(FALSE);

		return;
	}

	if (pItem->dwThreadID)
	{
		ATLTRACE(_T("Thread Busy: 0x%x\n"), pItem->dwThreadID);
		return;
	}
	if (!pItem->pIWork->CheckExit_InWorkThread())
	{
		ATLTRACE(_T("Thread Busy: 0x%x\n"), pItem->dwThreadID);
		return;
	}

	m_mapSink.erase(itSink);
	m_mapWork.erase(itWork);

	pItem->pISink->FetchAllMsg();
	if (pItem->pThreadParam)
	{
		pItem->pISink->OnFreeParam(pItem->pThreadParam);
	}
	delete pItem->pISink;
	delete pItem->pIWork;
	delete pItem;
}

void CThreadPoolMgr2::OnNotifyDelTask(WPARAM wParam, LPARAM lParam)
{
	if (FALSE == CThreadPoolMgr2::IsObjectValid())
	{
		Util::Log::Info(_T("CThreadPoolMgr2::OnNotifyDelTask"), _T("The Object is not Valid.\r\n"));
		return;
	}
		
	Util::kObj::CAutoCriticalSection lock(&m_csLock);//��ͬ�߳��ظ����ٽ���û����

	IThreadObjSink* p = (IThreadObjSink*)wParam;
	map<IThreadObjSink*, TaskItem*>::iterator itSink = m_mapSink.find(p);
	if (itSink == m_mapSink.end())
	{
		//ATLASSERT(FALSE);

		return;
	}
	
	DelTask_InLock(itSink->second);
}

BOOL CThreadPoolMgr2::IsObjectValid()
{
	return m_bValid;     //
}
#include "stdafx.h"
#include "UtilThreadPool2.h"
#include "ThreadPoolMgr2.h"
#include "Singleton.h"

//mgr�������ʱ���ü�������Ϊ�ʼ�ͻᴴ������ģ�飬��ʱ��Ҫ�������tbd
//---------------------------------------------------//
void	Util::ThreadPool2::SetThreadNum(ThreadSlotClass cls, UINT8 high)
{
	if (TRUE == CThreadPoolMgr2::IsObjectValid())
		Singleton<CThreadPoolMgr2>::Instance().SetThreadNum(cls, high);
}

UINT64	Util::ThreadPool2::AddTask(ThreadSlotClass cls, IThreadObjSink* pISink, IThreadObjWork* pIWork, void* pThreadParam)	//ע��pParam�����ڣ������new����ָ�룬��������Ϊsink��һ����Ա����������������ʱ���ͷ�
{
	if (TRUE == CThreadPoolMgr2::IsObjectValid())
		return Singleton<CThreadPoolMgr2>::Instance().AddTask(cls, pISink, pIWork, pThreadParam);
	return 0;
}

void	Util::ThreadPool2::DelTask(UINT64 id)	//Sink��Work���󲢲����ڲ�ɾ���������������ѹ�
{
	if (TRUE == CThreadPoolMgr2::IsObjectValid())
	    Singleton<CThreadPoolMgr2>::Instance().DelTask(id);
}

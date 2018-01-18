#pragma once
#include "UtilThreadPool2.h"
#include "EventSupport.h"
#include "DownLoadHelper.h"
#include "DownLoadFileMgrEvent.h"
class CDownLoadSink : public Util::ThreadPool2::CThreadObjSink<C2W_Msg_FileDown, W2C_Msg_FileDown>
{
public:
	CDownLoadSink(CDownLoadFileMgrEvent *pCDownLoadFileMgrEvent);
	~CDownLoadSink();
public:
	//���ϲ�֪ͨ����
	void Detach();
public:
	DECLARE_EVENT_RECEIVER(CDownLoadSink)
	virtual void OnW2CMsgCome(INT32& cmd, W2C_Msg_FileDown& msg);     //��Ϣ��ȡ
	void OnFreeParam(void* pThreadParam);                          //�ͷ��߳��� new�����Ĳ���
private:
	CDownLoadFileMgrEvent *m_pCDownLoadFileMgrEvent;
};


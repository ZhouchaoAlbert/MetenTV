#include "stdafx.h"
#include "DownLoadSink.h"


CDownLoadSink::CDownLoadSink(CDownLoadFileMgrEvent *pCUpLoadFileMgrEvent)
{
	m_pCDownLoadFileMgrEvent = pCUpLoadFileMgrEvent;
}


CDownLoadSink::~CDownLoadSink()
{
}
void CDownLoadSink::Detach()
{
	m_pCDownLoadFileMgrEvent = NULL;
}
void CDownLoadSink::OnFreeParam(void* pThreadParam)
{
	if (NULL != pThreadParam)
	{
		DownloadFileInfo *pDownLoadFileInfo = (DownloadFileInfo*)pThreadParam;
		delete pDownLoadFileInfo;
	}

}
void CDownLoadSink::OnW2CMsgCome(INT32& cmd, W2C_Msg_FileDown& msg)
{
	switch (cmd)
	{
	case CMD_DOWN_PROGRESS:
	{
			if (NULL != m_pCDownLoadFileMgrEvent)
				m_pCDownLoadFileMgrEvent->OnHttpDownProgress(msg.uTaskID, msg.ProgressInfo.pos,msg.ProgressInfo.range, msg.ProgressInfo.speed, msg.ProgressInfo.remain_time);
	}
		break;
	case  CMD_DOWN_RESULT:
	{
			 if (NULL != m_pCDownLoadFileMgrEvent)
				 m_pCDownLoadFileMgrEvent->OnHttpDownResult(msg.uTaskID, msg.ResultInfo.uCode);
	}
		break;
	default:
		ATLASSERT(TRUE);
	}
}
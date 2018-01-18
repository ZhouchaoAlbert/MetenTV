#include "stdafx.h"
#include "DownLoadFileMgr.h"
#include "UtilTools.h"
#include "DownLoadSink.h"
#include "DownLoadWork.h"

CDownLoadFileMgr::CDownLoadFileMgr()
{
	Util::ThreadPool2::SetThreadNum(Util::ThreadPool2::TSC_FILETRANS_DOWN_FILE, MAX_DOWN_FILE);   //Set the  max Thread  of  down file
}
void CDownLoadFileMgr::OnHttpDownProgress(UINT64 TaskID, UINT64 pos, UINT64 range, UINT64 speed, UINT32 remain_time)
{
	map<UINT64, DOWNEVENT>::iterator it = m_mapWorkFile.find(TaskID);
	if (it == m_mapWorkFile.end())
	{
		ATLASSERT(FALSE);
		return;
	}
	BOOL bShowProgress = m_mapWorkFile[TaskID].bShowProgress;
	if (FALSE == bShowProgress)    //不显示进度直接返回
		return;
	CDownLoadSink* pCDownLoadSink = m_mapWorkFile[TaskID].pSink;
	CString strFileLocalPath = m_mapWorkFile[TaskID].strFileLocalPath;
	Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent* pNotifyEvent = m_mapWorkFile[TaskID].pNotifyEvent;
	if (NULL != pNotifyEvent)
		pNotifyEvent->OnHttpDownProgress(TaskID, pos, range, speed, (UINT32)remain_time);        //通知外部变更 进度
}
void CDownLoadFileMgr::OnHttpDownResult(UINT64 TaskID, UINT32 code)
{
	map<UINT64, DOWNEVENT>::iterator it = m_mapWorkFile.find(TaskID);
	if (it == m_mapWorkFile.end())
	{
		ATLASSERT(FALSE);
		return;
	}

	CDownLoadSink* pCDownLoadSink = m_mapWorkFile[TaskID].pSink;
	CString strURL = m_mapWorkFile[TaskID].strURL;
	CString strFilePath = m_mapWorkFile[TaskID].strFileLocalPath;
	CString strKey = strURL + _T("_") + strFilePath;
	UINT32 key = Util::Hash::Hash(strKey);
	if (m_mapTaskFile.find(key) == m_mapTaskFile.end())   //校验操作
	{
		ATLASSERT(FALSE);
	}
	Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent* pNotifyEvent = m_mapWorkFile[TaskID].pNotifyEvent;

	m_mapWorkFile.erase(TaskID);
	m_mapTaskFile.erase(key);       // 这边先删除任务  再通知外面  避免在底层通知上层的回调中  上层再次调用  停止操作   导致异常

	if (NULL != pNotifyEvent)
		pNotifyEvent->OnHttpDownResult(TaskID, code);
}

CDownLoadFileMgr::~CDownLoadFileMgr()
{
	StopAllTask();
}
UINT32 CDownLoadFileMgr::Stop(UINT64 TaskID)
{
	map<UINT64, DOWNEVENT>::iterator it = m_mapWorkFile.find(TaskID);
	if (it != m_mapWorkFile.end())
	{
		CDownLoadSink* pCDownLoadSink = m_mapWorkFile[TaskID].pSink;
		CString strUrl = m_mapWorkFile[TaskID].strURL;
		CString strFilePath = m_mapWorkFile[TaskID].strFileLocalPath;
		CString strKey = strUrl + _T("_") + strFilePath;
		UINT32 key = Util::Hash::Hash(strKey);
		if (m_mapTaskFile.find(key) == m_mapTaskFile.end())   //校验操作
		{
			ATLASSERT(FALSE);
		}
		if (NULL != pCDownLoadSink)
		{
			pCDownLoadSink->Detach();           //分离操作  避免后续还收到相关的消息
			Util::ThreadPool2::DelTask(TaskID);    
			m_mapWorkFile.erase(it);
			m_mapTaskFile.erase(key);
		}
	}
	return Util::FileTrans::c_suc;
}
UINT32 CDownLoadFileMgr::DownLoad(LPCTSTR pszUrl, LPCTSTR pszLocalFilePath, Util::FileTrans::FILETYPE filetype, Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent*pListen, UINT64* pTaskID, BOOL bShowProgress)
{
	if (NULL == pszUrl || NULL==pszLocalFilePath)
		return Util::FileTrans::c_fail;
	CString strUrl = pszUrl,strFilePath=pszLocalFilePath;
	CString strKey = strUrl + _T("_") + pszLocalFilePath;
	UINT32 key = Util::Hash::Hash(strKey);
	map<UINT32, UINT64>::iterator iter = m_mapTaskFile.find(key);
	if (iter != m_mapTaskFile.end())    //任务已经在下载了
	{
		UINT64 uTaskID = iter->second;
		if (m_mapWorkFile.find(uTaskID) == m_mapWorkFile.end())    //校验一下  上面找到了  这边不可能找不到
		{
			ATLASSERT(FALSE);
			return Util::FileTrans::c_fail;
		}
		*pTaskID = uTaskID;
		return Util::FileTrans::c_suc;      //返回成功
	}
	CDownLoadSink* pCDownloadSink = new CDownLoadSink(this);    //这边每次new出来的sink 和 Work  都不需要去手动Delete      Work结束之后线程模型中会自动Delete
	CDownLoadWork* pCDownloadWork = new CDownLoadWork;
	DownloadFileInfo *pFileInfo = new DownloadFileInfo;                   //传进去线程的数据 需要new出来
	if (NULL == pCDownloadSink || NULL == pFileInfo || NULL == pCDownloadWork)
	{
		ATLASSERT(FALSE);
		delete pCDownloadSink;
		delete pFileInfo;
		delete pCDownloadWork;
		return Util::FileTrans::c_fail;
	}
	pFileInfo->uSeverTime = 0; //zjg tbd 服务器时间
	pFileInfo->filetype = filetype;
	pFileInfo->strFileLocalPath = pszLocalFilePath;
	pFileInfo->strUrl = pszUrl;
	UINT64 nThreadID = Util::ThreadPool2::AddTask(Util::ThreadPool2::TSC_FILETRANS_DOWN_FILE, pCDownloadSink, pCDownloadWork, pFileInfo);
	*pTaskID = nThreadID;
	if (0 == nThreadID)
	{
		ATLASSERT(FALSE);
		delete pCDownloadSink;
		delete pFileInfo;
		delete pCDownloadWork;
		return Util::FileTrans::c_fail;
	}
	m_mapWorkFile[nThreadID].FileType = filetype;
	m_mapWorkFile[nThreadID].pSink = pCDownloadSink;
	m_mapWorkFile[nThreadID].pWork = pCDownloadWork;
	m_mapWorkFile[nThreadID].strFileLocalPath = pszLocalFilePath;
	m_mapWorkFile[nThreadID].strURL = pszUrl;
	m_mapWorkFile[nThreadID].pNotifyEvent = pListen;
	m_mapWorkFile[nThreadID].bShowProgress = bShowProgress;
	m_mapTaskFile[key] = nThreadID;
	return Util::FileTrans::c_suc;
}
void CDownLoadFileMgr::StopAllTask()
{
	for (map<UINT64, DOWNEVENT>::iterator it = m_mapWorkFile.begin(); it != m_mapWorkFile.end(); it++)
	{
		CDownLoadSink* pCDownLoadSink = it->second.pSink;
		if (NULL != pCDownLoadSink)
		{
			pCDownLoadSink->Detach();
			Util::ThreadPool2::DelTask(it->first);
		}
	}
	m_mapWorkFile.clear();
	m_mapTaskFile.clear();
}

void CDownLoadFileMgr::RemoveSink(Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent* pSink)
{
	if (m_mapWorkFile.empty())
		return;
	map<UINT64, DOWNEVENT>::iterator it = m_mapWorkFile.begin();
	while(it != m_mapWorkFile.end())
	{
		if (it->second.pNotifyEvent == pSink)    //找到对应的Sink
		{
			CString strUrl = it->second.strURL;
			CString strFileLocalPath = it->second.strFileLocalPath;
			CDownLoadSink* pCDownLoadSink = it->second.pSink;
			UINT64 uTaskID = it->first;
			CString strKey = strUrl + _T("_") + strFileLocalPath;
			UINT32 key = Util::Hash::Hash(strKey);
			
			if (m_mapTaskFile.find(key) != m_mapTaskFile.end())
			{
				m_mapTaskFile.erase(key);
			}
			else
			{
				ATLASSERT(FALSE);
			}
			if (NULL!=pCDownLoadSink)
			      pCDownLoadSink->Detach();           //分离操作  避免后续还收到相关的消息
			Util::ThreadPool2::DelTask(uTaskID);  // 停止任务
			it = m_mapWorkFile.erase(it);  //删除相关信息
		}
		else
		{
			it++;
		}
	}
}
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
	if (FALSE == bShowProgress)    //����ʾ����ֱ�ӷ���
		return;
	CDownLoadSink* pCDownLoadSink = m_mapWorkFile[TaskID].pSink;
	CString strFileLocalPath = m_mapWorkFile[TaskID].strFileLocalPath;
	Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent* pNotifyEvent = m_mapWorkFile[TaskID].pNotifyEvent;
	if (NULL != pNotifyEvent)
		pNotifyEvent->OnHttpDownProgress(TaskID, pos, range, speed, (UINT32)remain_time);        //֪ͨ�ⲿ��� ����
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
	if (m_mapTaskFile.find(key) == m_mapTaskFile.end())   //У�����
	{
		ATLASSERT(FALSE);
	}
	Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent* pNotifyEvent = m_mapWorkFile[TaskID].pNotifyEvent;

	m_mapWorkFile.erase(TaskID);
	m_mapTaskFile.erase(key);       // �����ɾ������  ��֪ͨ����  �����ڵײ�֪ͨ�ϲ�Ļص���  �ϲ��ٴε���  ֹͣ����   �����쳣

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
		if (m_mapTaskFile.find(key) == m_mapTaskFile.end())   //У�����
		{
			ATLASSERT(FALSE);
		}
		if (NULL != pCDownLoadSink)
		{
			pCDownLoadSink->Detach();           //�������  ����������յ���ص���Ϣ
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
	if (iter != m_mapTaskFile.end())    //�����Ѿ���������
	{
		UINT64 uTaskID = iter->second;
		if (m_mapWorkFile.find(uTaskID) == m_mapWorkFile.end())    //У��һ��  �����ҵ���  ��߲������Ҳ���
		{
			ATLASSERT(FALSE);
			return Util::FileTrans::c_fail;
		}
		*pTaskID = uTaskID;
		return Util::FileTrans::c_suc;      //���سɹ�
	}
	CDownLoadSink* pCDownloadSink = new CDownLoadSink(this);    //���ÿ��new������sink �� Work  ������Ҫȥ�ֶ�Delete      Work����֮���߳�ģ���л��Զ�Delete
	CDownLoadWork* pCDownloadWork = new CDownLoadWork;
	DownloadFileInfo *pFileInfo = new DownloadFileInfo;                   //����ȥ�̵߳����� ��Ҫnew����
	if (NULL == pCDownloadSink || NULL == pFileInfo || NULL == pCDownloadWork)
	{
		ATLASSERT(FALSE);
		delete pCDownloadSink;
		delete pFileInfo;
		delete pCDownloadWork;
		return Util::FileTrans::c_fail;
	}
	pFileInfo->uSeverTime = 0; //zjg tbd ������ʱ��
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
		if (it->second.pNotifyEvent == pSink)    //�ҵ���Ӧ��Sink
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
			      pCDownLoadSink->Detach();           //�������  ����������յ���ص���Ϣ
			Util::ThreadPool2::DelTask(uTaskID);  // ֹͣ����
			it = m_mapWorkFile.erase(it);  //ɾ�������Ϣ
		}
		else
		{
			it++;
		}
	}
}
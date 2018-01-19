#include "stdafx.h"
#include "UtilFileTrans.h"
#include "Singleton.h"
#include "DownLoadFileMgr.h"
#include "UtilString.h"
//�����ļ��Ľӿ�, UINT32���ش�����
//ֹͣ�����ļ�

Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent::~CHttpDownLoadEvent()
{
	Singleton<CDownLoadFileMgr>::Instance().RemoveSink(this);
}

//�����ļ��������
UINT32 Util::FileTrans::HttpDownLoad::DownLoad(LPCTSTR pszUrl, LPCTSTR pszLocalFilePath, FILETYPE type, CHttpDownLoadEvent*pListen, UINT64* pTaskID, BOOL bShowProgress, BOOL bTempFileMode)
{
	return Singleton<CDownLoadFileMgr>::Instance().DownLoad(pszUrl, pszLocalFilePath, type, pListen, pTaskID, bShowProgress,bTempFileMode);
}

UINT32 Util::FileTrans::HttpDownLoad::Stop(UINT64 TaskID)
{
	return Singleton<CDownLoadFileMgr>::Instance().Stop(TaskID);
}

 



//BOOL Util::FileTrans::UploadDetection::Start(LPCTSTR pszLocalFilePath, FILETYPE filetype, LPCTSTR lpSeverName)
//{
//	return StartUpload(pszLocalFilePath, filetype,lpSeverName);
//}
//BOOL Util::FileTrans::UploadDetection::Post(LPCTSTR strUrl, LPCTSTR strData)
//{
//	return PostData(strUrl, strData);
//}
//BOOL Util::FileTrans::UploadDetection::Start(UINT8* pBuffer, UINT64 pBufferSize, FILETYPE filetype, LPCTSTR lpSeverName)
//{
	//return StartUpLoadBuffer(pBuffer, pBufferSize, filetype, lpSeverName);
//}

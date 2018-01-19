#pragma once
#include "UtilFileTrans.h"
#include "DownLoadHelper.h"
#include "DownLoadSink.h"
#include "DownLoadWork.h"
#include "DownLoadFileMgrEvent.h"
#include <map>

using namespace std;

struct DOWNEVENT
{
	DOWNEVENT()
	{
		pSink = NULL;
		pWork = NULL;
		pNotifyEvent = NULL;
		bShowProgress = FALSE;
	}
    Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent* pNotifyEvent; //�ص���ָ��  
	Util::FileTrans::FILETYPE   FileType;
	CDownLoadSink* pSink;
	CDownLoadWork* pWork;
	CString strFileLocalPath;   //Ҫ���صı���·��
	CString strURL;                //�ļ������ص�ַ
	BOOL bShowProgress;

};
class CDownLoadFileMgr:public CDownLoadFileMgrEvent
{
public:
	CDownLoadFileMgr();
	~CDownLoadFileMgr();
public:
	void OnHttpDownProgress(UINT64 TaskID, UINT64 pos, UINT64 range, UINT64 speed, UINT32 remain_time);
	 void OnHttpDownResult(UINT64 TaskID, UINT32 code);
public:
	//�ϲ��������  �Ƴ��ù��������е�Sink��Ϣ
	void RemoveSink(Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent* pSink);
public:
	UINT32 DownLoad(LPCTSTR pszUrl, LPCTSTR pszLocalFilePath, Util::FileTrans::FILETYPE filetype, Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent*pListen, UINT64* pTaskID, BOOL bShowProgress, BOOL bTempFileMode);
	UINT32 Stop(UINT64 TaskID);


protected:
	//ֹͣ������������
	void StopAllTask();
private:
	map<UINT64, DOWNEVENT> m_mapWorkFile;		      // TaskID->�ϴ����������Ϣ
	map<UINT32, UINT64> m_mapTaskFile;                     //�ļ�·����URL ��ӦHashֵ   ���ɵ�Key ��TaskID;
};


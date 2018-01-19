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
    Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent* pNotifyEvent; //回调的指针  
	Util::FileTrans::FILETYPE   FileType;
	CDownLoadSink* pSink;
	CDownLoadWork* pWork;
	CString strFileLocalPath;   //要下载的本地路径
	CString strURL;                //文件的下载地址
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
	//上层对象被析够  移除该关联的所有的Sink信息
	void RemoveSink(Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent* pSink);
public:
	UINT32 DownLoad(LPCTSTR pszUrl, LPCTSTR pszLocalFilePath, Util::FileTrans::FILETYPE filetype, Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent*pListen, UINT64* pTaskID, BOOL bShowProgress, BOOL bTempFileMode);
	UINT32 Stop(UINT64 TaskID);


protected:
	//停止所有下载任务
	void StopAllTask();
private:
	map<UINT64, DOWNEVENT> m_mapWorkFile;		      // TaskID->上传任务具体信息
	map<UINT32, UINT64> m_mapTaskFile;                     //文件路径与URL 对应Hash值   生成的Key 对TaskID;
};


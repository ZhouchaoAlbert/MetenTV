#pragma once

#include "UtilThreadPool2.h"
#include "EventSupport.h"
#include "DownLoadHelper.h"

class CDownLoadWork :public Util::ThreadPool2::CThreadObjWork<C2W_Msg_FileDown, W2C_Msg_FileDown>
{
public:
	CDownLoadWork();
	~CDownLoadWork();
public:
	virtual void Run_InWorkThread(void* pParam);	//注意因为需要考虑到尽量主动退出，所以在时间上需要留意Sleep间隔不能大于m_u32WaitSleep/2
public:
	void StartDownLoadFile(CString strUrl,CString strFileLocalPath);
	// Get the accurate File length
	double GetFileSeverLength(CString strUrl);
	//Return the file already down Local Range
	UINT64 CheckFileResumDownLoad(CString strUrl, CString strFileLocalPath,CString & strFileTempLocalPath);
	//DownFile  
	ERROR_CODE  DownFile(CString strFileTempPath, CString strUrl, UINT64 uFileRange, CString strFilePath);
	//Check the file  need  stop
	BOOL  CheckFileInThread();
	//简单校验接口 如果是阿里的url 链接检测是否将错误数据 写入文件当中 导致外部看起下载成功其实失败问题（使用阿里的下载接口应该能解决此问题，为了通用性暂时不这么做）
	BOOL SimpleCheckFileAlioss(CString strUrl, CString strFileLocalPath);
public:
	void PostW2CDownFileResult(INT CMD, INT msgType, INT nSuc, CString URL = _T(""));      //向主线程 发送  结果消息
	void PostW2CDownProgress(INT CMD, INT msgType, UINT64 AlreadyDownByte, UINT64 nFileSize, UINT64 fRate, UINT64 nTime);
public:
	DWORD m_dProgressTime;  //spread the time  when call the ProgressCallback Function. To calculate the  speed.

	DWORD m_nDlTimeOut;  // the DownLoad speed  always  zero, use the variable Control it.

	INT64 m_nDlFlag;           //use calculate the time

	UINT64  m_uStartSeverTime;

	double m_dInitialLen;     //
private:
	Util::FileTrans::FILETYPE	 m_filetype;
};


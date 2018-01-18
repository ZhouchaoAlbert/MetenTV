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
	virtual void Run_InWorkThread(void* pParam);	//ע����Ϊ��Ҫ���ǵ����������˳���������ʱ������Ҫ����Sleep������ܴ���m_u32WaitSleep/2
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
	//��У��ӿ� ����ǰ����url ���Ӽ���Ƿ񽫴������� д���ļ����� �����ⲿ�������سɹ���ʵʧ�����⣨ʹ�ð�������ؽӿ�Ӧ���ܽ�������⣬Ϊ��ͨ������ʱ����ô����
	BOOL SimpleCheckFileAlioss(CString strUrl, CString strFileLocalPath);
public:
	void PostW2CDownFileResult(INT CMD, INT msgType, INT nSuc, CString URL = _T(""));      //�����߳� ����  �����Ϣ
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


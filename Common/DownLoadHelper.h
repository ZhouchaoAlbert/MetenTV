#pragma once
#include "UtilPath.h"
#include <atlstr.h>
#include "UtilFileTrans.h"

using namespace ATL;


#define	STR_DLDSUFFIX _T(".dl")                //Temp file ex
#define DOWNLOAD_TIMEOUT 15000      //Time Out
#define MAX_DOWN_FILE           5

#define MAX_DOWM_ERROR_RETRY            3

#define HTTP_OK 200
#define HTTP_RANGE_OK  206  //�ϵ�����OK
#define HTTP_NOT_FOUND  404

//�߳�ͨ�����
enum
{
	CMD_NONE = 0,
	CMD_DOWN_PROGRESS,	 //�ļ����� down
	CMD_DOWN_RESULT,     //�ļ���� down
};

//���ص��ļ���Ϣ
struct DownloadFileInfo
{
	CString                     strUrl;                          //���ص�URL��ַ��Ϣ
	CString						 strFileLocalPath;           //Ҫ�����ļ��ı���·��
	Util::FileTrans::FILETYPE	filetype;                 //����
	UINT64                    uSeverTime;               //��ǰ��ʼ���صķ����ʱ��  ����ǩ��ʹ��
};

struct DownLoadResult
{
	UINT32 uTaskID;
	UINT32 uCode;
};
struct DownLoadProgress
{
	UINT64						range;
	UINT64						pos;
	UINT64						speed;         //ʵʱ�ٶ�xxByte/s
	UINT32						remain_time;   //ʣ���ʱ��
};
struct C2W_Msg_FileDown
{
	INT msgType;
	C2W_Msg_FileDown()
	{
		msgType = CMD_NONE;
	}
};
struct W2C_Msg_FileDown
{
	W2C_Msg_FileDown()
	{
		msgType = CMD_NONE;
		uTaskID = 0;
	}
	INT msgType;
	UINT64 uTaskID;
	DownLoadProgress ProgressInfo;    //CMD=CMD_UP_PROGRESS ��Ч
	DownLoadResult ResultInfo;            //CMD=CMD_UP_RESULT��Ч
};   

enum ERROR_CODE
{
	ERROR_FILE_OK = 0,  //SUCCESS
	ERROR_FILE_FALSE = 1,           //ʧ��
	ERROR_FILE_NOT,       //�ļ�δ�ҵ�
	ERROR_FILE_USER_CANCEL, //�û�����ȡ������
	ERROR_SIMPLE_CHECK_FAIL,//�򵥵ļ��ʧ��
};

//�жϽӿ��Ƿ���Ҫ������Ȩ���ʵ�URL
BOOL IsSignedUrl(CString strUrl, CString& strObject, CString& strBucket);

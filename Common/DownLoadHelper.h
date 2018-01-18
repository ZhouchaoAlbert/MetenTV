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
#define HTTP_RANGE_OK  206  //断点下载OK
#define HTTP_NOT_FOUND  404

//线程通信相关
enum
{
	CMD_NONE = 0,
	CMD_DOWN_PROGRESS,	 //文件进度 down
	CMD_DOWN_RESULT,     //文件结果 down
};

//下载的文件信息
struct DownloadFileInfo
{
	CString                     strUrl;                          //下载的URL地址信息
	CString						 strFileLocalPath;           //要下载文件的本地路径
	Util::FileTrans::FILETYPE	filetype;                 //类型
	UINT64                    uSeverTime;               //当前开始下载的服务端时间  用来签名使用
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
	UINT64						speed;         //实时速度xxByte/s
	UINT32						remain_time;   //剩余的时间
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
	DownLoadProgress ProgressInfo;    //CMD=CMD_UP_PROGRESS 有效
	DownLoadResult ResultInfo;            //CMD=CMD_UP_RESULT有效
};   

enum ERROR_CODE
{
	ERROR_FILE_OK = 0,  //SUCCESS
	ERROR_FILE_FALSE = 1,           //失败
	ERROR_FILE_NOT,       //文件未找到
	ERROR_FILE_USER_CANCEL, //用户主动取消下载
	ERROR_SIMPLE_CHECK_FAIL,//简单的检测失败
};

//判断接口是否需要生成授权访问的URL
BOOL IsSignedUrl(CString strUrl, CString& strObject, CString& strBucket);

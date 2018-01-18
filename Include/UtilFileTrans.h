#pragma once

#include "BaseDefine.h"
#include <atlstr.h>

using namespace ATL;
namespace Util
{
	namespace FileTrans
	{
		enum FILETYPE
		{
			FILE_TYPE_NONE = 0,      //
			FILE_TYPE_NORMAL = 1,      //普通文件
			FILE_TYPE_PIC = 2,         //图片文件
			FILE_TYPE_RECORD = 3,       //录音文件      
			FILE_TYPE_PIC_GIF=4,                //Gif 
			FILE_TYPE_GROUP_FILE = 5,    //群文件
			FILE_TYPE_SYSETM_FACE =6,  // EC自带的系统表情
		};
		enum FILE_UP_METHOD
		{
			FILE_UP_AUTO = 0,
			FILE_UP_BY_OSS,
			FILE_UP_BY_COS,
		};
		const UINT32 c_suc = 0;
		const UINT32 c_fail = 1;
		const UINT32 c_file_repeat = 2;  // 文件重复上传
		const UINT32 c_key_InvalidAccessKeyId = 3;   //key  和ID失效了
		const UINT32 c_resum_file_timeout = 4;        //向主线程 查询断点续传相关信息的时候超时了
		

		namespace  HttpDownLoad
		{
			//下载的回调事件
			class COMMON_API CHttpDownLoadEvent
			{
			public:
				CHttpDownLoadEvent(){}
				virtual ~CHttpDownLoadEvent();
				virtual void OnHttpDownProgress(UINT64 TaskID, UINT64 pos, UINT64 range, UINT64 speed, UINT32 remain_time) = 0;
				virtual void OnHttpDownResult(UINT64 TaskID, UINT32 code) = 0;
			};
			//下载接口
			/*
			参数说明
			pszURl;  下载地址
			pszLocalFilePath；   //指定要下载的文件的全路径  包含后缀
			type                    ；  //文件类型
			pListen               ：文件上传进度 结果的回调指针
			pTaskID               ： 启动任务之后返回的工作ID  唯一标志
			bShowProgress  ；  是否需要显示进度 
			*/
			COMMON_API UINT32 DownLoad(LPCTSTR pszUrl, LPCTSTR pszLocalFilePath, FILETYPE type, CHttpDownLoadEvent*pListen, UINT64* pTaskID, BOOL bShowProgress = FALSE);
			COMMON_API UINT32 Stop(UINT64 TaskID);
		}
	}
}
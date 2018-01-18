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
			FILE_TYPE_NORMAL = 1,      //��ͨ�ļ�
			FILE_TYPE_PIC = 2,         //ͼƬ�ļ�
			FILE_TYPE_RECORD = 3,       //¼���ļ�      
			FILE_TYPE_PIC_GIF=4,                //Gif 
			FILE_TYPE_GROUP_FILE = 5,    //Ⱥ�ļ�
			FILE_TYPE_SYSETM_FACE =6,  // EC�Դ���ϵͳ����
		};
		enum FILE_UP_METHOD
		{
			FILE_UP_AUTO = 0,
			FILE_UP_BY_OSS,
			FILE_UP_BY_COS,
		};
		const UINT32 c_suc = 0;
		const UINT32 c_fail = 1;
		const UINT32 c_file_repeat = 2;  // �ļ��ظ��ϴ�
		const UINT32 c_key_InvalidAccessKeyId = 3;   //key  ��IDʧЧ��
		const UINT32 c_resum_file_timeout = 4;        //�����߳� ��ѯ�ϵ����������Ϣ��ʱ��ʱ��
		

		namespace  HttpDownLoad
		{
			//���صĻص��¼�
			class COMMON_API CHttpDownLoadEvent
			{
			public:
				CHttpDownLoadEvent(){}
				virtual ~CHttpDownLoadEvent();
				virtual void OnHttpDownProgress(UINT64 TaskID, UINT64 pos, UINT64 range, UINT64 speed, UINT32 remain_time) = 0;
				virtual void OnHttpDownResult(UINT64 TaskID, UINT32 code) = 0;
			};
			//���ؽӿ�
			/*
			����˵��
			pszURl;  ���ص�ַ
			pszLocalFilePath��   //ָ��Ҫ���ص��ļ���ȫ·��  ������׺
			type                    ��  //�ļ�����
			pListen               ���ļ��ϴ����� ����Ļص�ָ��
			pTaskID               �� ��������֮�󷵻صĹ���ID  Ψһ��־
			bShowProgress  ��  �Ƿ���Ҫ��ʾ���� 
			*/
			COMMON_API UINT32 DownLoad(LPCTSTR pszUrl, LPCTSTR pszLocalFilePath, FILETYPE type, CHttpDownLoadEvent*pListen, UINT64* pTaskID, BOOL bShowProgress = FALSE);
			COMMON_API UINT32 Stop(UINT64 TaskID);
		}
	}
}
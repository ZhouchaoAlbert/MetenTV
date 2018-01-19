#include "stdafx.h"
#include "DownLoadWork.h"
#include "DownLoadHelper.h"
#include "UtilPath.h"
#include "UtilString.h"
#include "curl/curl.h"
#include "VersionDefault.h"
#include "UtilTools.h"
#include "BaseFile.h"

CDownLoadWork::CDownLoadWork()
{
	m_nDlTimeOut = 0;
	m_dProgressTime = 0;
	m_dInitialLen = 0;
	m_nDlFlag = 0; 
	m_uStartSeverTime = 0;
	m_bTempFileMode = FALSE;
}


CDownLoadWork::~CDownLoadWork()
{
}
static size_t save_header(void *ptr, size_t size, size_t nmemb, void *data)
{
	return (size_t)(size * nmemb);
}
size_t DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	FILE* fp = (FILE*)pParam;
	if (fp != NULL)
	{
		size_t nWrite = fwrite(pBuffer, nSize, nMemByte, fp);
		return nWrite;
	}
	return 0;
}
//这个回调函数可以告诉我们有多少数据需要传输以及传输了多少数据，单位是字节。
//dltotal是需要下载的总字节数，dlnow是已经下载的字节数。ultotal是将要上传的字节数，ulnow是已经上传的字节数。
//如果你仅仅下载数据的话，那么ultotal，ulnow将会是0，反之，如果你仅仅上传的话，那么dltotal和dlnow也会是0。
//clientp为用户自定义参数，通过设置CURLOPT_XFERINFODATA属性来传递。此函数返回非0值将会中断传输，错误代码是CURLE_ABORTED_BY_CALLBACK
int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	//返回非0值就会终止 curl_easy_perform 执行
	int nRet = 0;
	CDownLoadWork* pThis = (CDownLoadWork*)clientp;
	if (pThis != NULL)
	{
		if (pThis->CheckFileInThread())
			return -2;                                      //need  stop it

		DWORD dwTime = GetTickCount() - pThis->m_dProgressTime;
		bool bUpdate = (dwTime >= 500 || dlnow >= dltotal);

		if (bUpdate)
		{
			if (dlnow != 0.0 && dltotal != 0.0)
			{
				if (dltotal == dlnow)
				{
					int i = 0;
				}
				UINT64 nSpeed = ((dwTime == 0) ? 0 : ((INT64)dlnow - pThis->m_nDlFlag) / (INT64)dwTime);
				if (/*nSpeed != 0*/1)
				{
					UINT32 nReminTime = 0;
					if (nSpeed <= 0)
					{
						nReminTime = 0;
						if (pThis->m_nDlTimeOut == 0)
						{
							pThis->m_nDlTimeOut = GetTickCount();
						}
						DWORD dwTm = GetTickCount() - pThis->m_nDlTimeOut;
						if (dwTm >= DOWNLOAD_TIMEOUT)
						{
							nRet = -3;
						}
					}
					else
					{
						pThis->m_nDlTimeOut = 0;
						nReminTime = (UINT32)((INT64)(dltotal - dlnow) / nSpeed);
					}
					UINT64 uFileSize = (UINT64)pThis->m_dInitialLen + (UINT64)dltotal;
					UINT64 uAlreadyDownSize = (UINT64)dlnow + (UINT64)pThis->m_dInitialLen;    //总数
					pThis->PostW2CDownProgress(CMD_DOWN_PROGRESS, CMD_DOWN_PROGRESS, uAlreadyDownSize, uFileSize, nSpeed * 1000, nReminTime);
					pThis->m_dProgressTime = GetTickCount();
					pThis->m_nDlFlag = (INT64)dlnow;
				}
			}
		}
	}
	return nRet;
}
void CDownLoadWork::PostW2CDownFileResult(INT CMD, INT msgType, INT nSuc, CString URL)
{
	W2C_Msg_FileDown msg;
	msg.uTaskID = GetTaskID();
	msg.msgType = msgType;
	msg.ResultInfo.uCode = nSuc;
	msg.ResultInfo.uTaskID = (UINT32)GetTaskID();
	PostW2CMsg_InWorkThread(CMD, msg);
}
void CDownLoadWork::PostW2CDownProgress(INT CMD, INT msgType, UINT64 AlreadyDownByte, UINT64 nFileSize, UINT64 fRate, UINT64 nTime)
{
	W2C_Msg_FileDown msg;
	msg.uTaskID = GetTaskID();
	msg.msgType = msgType;
	msg.ProgressInfo.pos = AlreadyDownByte;
	msg.ProgressInfo.range = nFileSize;
	msg.ProgressInfo.speed = fRate;
	msg.ProgressInfo.remain_time = (UINT32)nTime;
	PostW2CMsg_InWorkThread(CMD, msg);
	NotifyNewW2CMsg_InWorkThread();                           //通知主线程 退出结束
}
void CDownLoadWork::Run_InWorkThread(void* pParam)
{
	DownloadFileInfo *pDownloadFileInfo = (DownloadFileInfo*)pParam;
	m_filetype = pDownloadFileInfo->filetype;
	if (NULL != pDownloadFileInfo)
	{
		m_uStartSeverTime = pDownloadFileInfo->uSeverTime;
		m_bTempFileMode = pDownloadFileInfo->bTempFileMode;
		StartDownLoadFile(pDownloadFileInfo->strUrl, pDownloadFileInfo->strFileLocalPath);
	}
	
}
void CDownLoadWork::StartDownLoadFile(CString strUrl, CString strFileLocalPath)
{
	CString strTempLocalPath;
	ERROR_CODE code = ERROR_FILE_FALSE;

	for (UINT32 i = 0; i < MAX_DOWM_ERROR_RETRY; i++)
	{
		UINT64 uFileRange = CheckFileResumDownLoad(strUrl, strFileLocalPath, strTempLocalPath);
		code = DownFile(strTempLocalPath, strUrl, uFileRange, strFileLocalPath);
		if (ERROR_FILE_OK==code)
		{
			if (FALSE == SimpleCheckFileAlioss(strUrl, strFileLocalPath))
			{
				code = ERROR_SIMPLE_CHECK_FAIL;
				DeleteFile(strFileLocalPath);
				Log_InWorkThread(Util::Log::E_RLL_ERROR, _T("[%s][%d][down_error] simple check file failed. the url=[%s],path=[%s]."), __FUNCTIONW__, __LINE__, strUrl, strFileLocalPath);
			}
			break;
		}
		else if (ERROR_FILE_NOT==code)
		{
			Log_InWorkThread(Util::Log::E_RLL_ERROR, _T("[%s][%d][down_error] Down file failed. the object not exit. the url=[%s],path=[%s]."), __FUNCTIONW__, __LINE__, strUrl, strFileLocalPath);
			break;
		}
		else if (ERROR_FILE_USER_CANCEL == code)
		{
			break;
		}
		else
		{
			Log_InWorkThread(Util::Log::E_RLL_ERROR, _T("[%s][%d][down_error] Down file failed the url=[%s],path=[%s] ReTry count=[%d]"), __FUNCTIONW__, __LINE__, strUrl, strFileLocalPath,i+1);
			Sleep(1000);   
		}
	}

	if (ERROR_FILE_OK == code)
	{
		PostW2CDownFileResult(CMD_DOWN_RESULT, CMD_DOWN_RESULT, Util::FileTrans::c_suc);
	}
	else if (ERROR_FILE_FALSE==code)
	{
		PostW2CDownFileResult(CMD_DOWN_RESULT, CMD_DOWN_RESULT, Util::FileTrans::c_fail);
	}
	else
	{
		PostW2CDownFileResult(CMD_DOWN_RESULT, CMD_DOWN_RESULT, Util::FileTrans::c_fail);
	}
}
double CDownLoadWork::GetFileSeverLength(CString strUrl)
{
	if (strUrl.IsEmpty())
		return 0.0;
	CStringA szfileurl;
	Util::String::W_2_A(strUrl, szfileurl);
	double len = 0.0;
	long filesize = 0;
	CURL *handle = curl_easy_init();

	curl_easy_setopt(handle, CURLOPT_URL, szfileurl.GetString());

	curl_easy_setopt(handle, CURLOPT_HEADER, 1);    //只要求header头

	curl_easy_setopt(handle, CURLOPT_NOBODY, 1);    //不需求body

	curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, save_header); // 取数据时连同HTTP头部一起取回.
	curl_easy_setopt(handle, CURLOPT_HEADERDATA, &filesize);
	CURLcode retcCode = curl_easy_perform(handle);

	if (retcCode == CURLE_OK)
	{
		if (CURLE_OK == curl_easy_getinfo(handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &len))
		{

		}
		else
		{
			len = -1;
		}

	}
	else
	{
		//ATLASSERT(FALSE);
		len = -1;
	}
	curl_easy_cleanup(handle);
	return len;
}
UINT64  CDownLoadWork::CheckFileResumDownLoad(CString strUrl, CString strFileLocalPath, CString& strFileTempLocalPath)
{
	UINT32 uHashPath = Util::Hash::Hash(strFileLocalPath);
	UINT32  uHashUrl = Util::Hash::Hash(strUrl);
	CString strHashPath,strHashURL;
	strHashPath.Format(_T("%u"), uHashPath);
	strHashURL.Format(_T("%u"), uHashUrl);
	UINT64 uFileLen = 0;
	strFileTempLocalPath = Util::Path::GetParentDirPath(strFileLocalPath) + _T("\\") + strHashPath + _T("_") + strHashURL +STR_DLDSUFFIX;
	if (Util::Path::IsFileExist(strFileTempLocalPath))
	{
		uFileLen = Util::Path::GetFileLength(strFileTempLocalPath);
		if (TRUE == m_bTempFileMode)
		{
			DeleteFile(strFileTempLocalPath);
		}
		//double dFileSeverLen = GetFileSeverLength(strUrl);     //这边服务端校验先去掉 cos 这边还不支持Header 请求，而 且这边仅仅是拿服务端 文件大小而已
		//if (dFileSeverLen <= 0)
		//{
		//	ATLASSERT(FALSE);    //dFileSeverLen 获取服务端的len 失败了  将会忽略对本地数据的校验
		//	;
		//}
		//else
		//{
		//	if ((uFileLen <= 0) || dFileSeverLen < uFileLen)
		//	{
		//		::DeleteFile(strFileTempLocalPath);
		//		uFileLen = 0;
		//	}
		//}
	}
	if (TRUE == m_bTempFileMode)
	{
		if (Util::Path::IsFileExist(strFileLocalPath))
			DeleteFile(strFileLocalPath);
	}
	
	m_dInitialLen = (double)uFileLen;
	return uFileLen;
}
ERROR_CODE  CDownLoadWork::DownFile(CString strFileTempPath, CString strUrl, UINT64 uFileRange, CString strFilePath)
{
	ERROR_CODE code = ERROR_FILE_FALSE;
	if (strFileTempPath.IsEmpty() || strUrl.IsEmpty())
		return code;
	CURL *pCurl = NULL;
	pCurl = curl_easy_init();
	CString strBucket, strObject, strSingUrl=strUrl;
	if (IsSignedUrl(strUrl, strObject, strBucket))
	{

	}
	CStringA strURUtf8;
	Util::String::W_2_Utf8(strSingUrl, strURUtf8);
	FILE* pFile = NULL;
	errno_t errcode = _wfopen_s(&pFile, strFileTempPath, _T("ab+"));
	if (0 == errcode)
	{
		errcode = fseek(pFile, 0, SEEK_END);//定位文件指针到文件末尾//成功返回0，不成功返回-1；
	}
	else
	{
		if (NULL!=pFile)
		    fclose(pFile);
		Log_InWorkThread(Util::Log::E_RLL_ERROR, _T("[%s][%d][down_error] Create file failed the error code=[%d] url=[%s]"), __FUNCTIONW__,__LINE__,errcode,strUrl);
		return code;
	}
	if (pCurl != NULL)
	{
		curl_easy_setopt(pCurl, CURLOPT_URL, strURUtf8.GetString());
		curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, FALSE);   //跳过ssl证书检测
		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, DownloadCallback);
		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pFile);    //写数据 用户参数
		CStringA strUserAgent;
		strUserAgent.Format("Mozilla/4.0 (compatible; MSIE 5.00; Windows 98; EC %d(%d))", HEADER_VERSION, BUILD_NUMBER);  
		curl_easy_setopt(pCurl, CURLOPT_USERAGENT, strUserAgent.GetString());
		curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1);     //屏蔽其他信号
		// 设置重定向的最大次数   302跳转最大次数
		curl_easy_setopt(pCurl, CURLOPT_MAXREDIRS, 5);
		//断点续传位置格式“x-y”
		if (0 != uFileRange)       //断点续传操作
		{
			char szRange[64] = { 0x0 };
			sprintf_s(szRange, 64, "%llu-", uFileRange);
			curl_easy_setopt(pCurl, CURLOPT_RANGE, szRange);
		}
		curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(pCurl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
		curl_easy_setopt(pCurl, CURLOPT_PROGRESSDATA, this);
		m_dProgressTime = GetTickCount();
		CURLcode curCode = curl_easy_perform(pCurl);
		if (CURLE_ABORTED_BY_CALLBACK == curCode)
		{
			code = ERROR_FILE_USER_CANCEL;
		}
		int respcode = 0;
		CURLcode retcCode = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &respcode);
		if ((CURLE_OK==curCode) && CURLE_OK == retcCode && ((HTTP_OK == respcode) || (HTTP_RANGE_OK == respcode)))
			code = ERROR_FILE_OK;
		else
		{
			Log_InWorkThread(Util::Log::E_RLL_ERROR, _T("[%s][%d][down_error] curl error code=[%d] http response code=[%d] url=[%s]"), __FUNCTIONW__, __LINE__, curCode, respcode, strUrl);
		}
		if (NULL!=pFile)
			fclose(pFile);
		curl_easy_cleanup(pCurl);
		//文件压根就不存在  删除本地文件
		if (HTTP_NOT_FOUND == respcode)
		{
			code = ERROR_FILE_NOT;
			DeleteFile(strFileTempPath);
		}
		if (ERROR_FILE_OK==code)
		{
			INT32 nRes=_trename(strFileTempPath, strFilePath);
			if (0!=nRes)
				Log_InWorkThread(Util::Log::E_RLL_ERROR, _T("[%s][%d][down_error] rename error code=[%d] url=[%s], hr: 0x%x"), __FUNCTIONW__, __LINE__, nRes,strUrl, GetLastError());
		}
	}
	return code;
}
BOOL  CDownLoadWork::CheckFileInThread()
{
	INT32 cmd = 0;
	C2W_Msg_FileDown msg;
	GetC2WMsg_InWorkThread(cmd, msg);
	if (CheckExit_InWorkThread())
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CDownLoadWork::SimpleCheckFileAlioss(CString strUrl, CString strFileLocalPath)
{
#define  ONE_KB     1024
	CString key(L"oss-cn-hangzhou.aliyuncs.com");
	if (-1 != strUrl.Find(key))
	{
		UINT64 uLen = Util::Path::GetFileLength(strFileLocalPath);
		if (uLen <= ONE_KB)
		{
			CBaseFile file;
			BOOL bSuc = file.OpenFile(strFileLocalPath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
			if (TRUE == bSuc)
			{
				UINT64 uFileSize = file.GetFileLen();
				if (0 == uFileSize)
				{
					return FALSE;
				}
				UINT8* szBuff = new UINT8[(UINT32)uFileSize];
				UINT64 uRealSize=file.GetFileContent(szBuff, uFileSize);
				CStringA strBuf((char*)szBuff, (UINT32)uRealSize);
				delete [] szBuff;
				if (-1 != strBuf.Find("<HostId>imagent.oss-cn-hangzhou.aliyuncs.com</HostId>") && -1 != strBuf.Find("<Error>"))
				{
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

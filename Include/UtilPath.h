#pragma once

//	目录结构
//	C:\Progma Files\EC\							InstallFolder
//	C:\Progma Files\EC\Bin\						ExeFolder
//	C:\Progma Files\EC\Config\					CfgFolder

//	C:\Progma Files\EC\Resource\				ResFolder
//	C:\Progma Files\EC\Resource\Xml\			ResXmlFolder
//	C:\Progma Files\EC\Resource\Material\		ResMatFolder
//	C:\Users\ecuser\Documents\EC\				DataFolder
//	C:\Users\ecuser\Documents\EC\Temp			DTmpFolder
//	C:\Users\ecuser\Documents\EC\Log\						LogFolder
//	C:\Users\ecuser\Documents\EC\1111\			UserFolder
//	C:\Users\ecuser\Documents\EC\1111\Image		GetUserImageFolder		收发图片
//	C:\Users\ecuser\Documents\EC\1111\Face		GetUserFaceFolder		头像	
//	C:\Users\ecuser\Documents\EC\1111\Catch		GetUserCatchFolder		截屏


//请每个进程注意设置SetExeFolder，访问某些函数时候，如果没有设置UID，可能会有错误
//获取文件夹下面的所有文件是否要获取子文件夹下的文件
enum FileScanFlag
{
	SINGLEFOLDER,	//单层文件夹
	MULTIFOLDER		//d多层文件夹
};

//#include <string>
#include <vector>
#include "BaseDefine.h"
#include "atlstr.h"

using namespace std;
using namespace ATL;
namespace Util
{
	namespace Path
	{
		//初始化
		COMMON_API BOOL	SetExeFolder(CString strFilePath);

		// 获取安装目录
		COMMON_API CString GetInstallFolder();

		// 获取主Exe所在目录
		COMMON_API CString GetExeFolder();

		// 获取主Config所在目录
		COMMON_API CString GetCfgFolder();

		// 获取主Log所在目录
		COMMON_API CString GetLogFolder(BOOL bCreateIfNotExist = FALSE);

		// 获取主Resource所在目录
		COMMON_API CString GetResFolder();

		// 获取主Resource::XML所在目录
		COMMON_API CString GetResXmlFolder();

		//获取声音文件目录
		COMMON_API CString GetResSoundFolder();

		// 获取主Resource::Material所在目录
		COMMON_API CString GetResMatFolder();

		//获取主Resource::Image 所在目录
		COMMON_API CString GetResImageFolder();
		//获取Emoji表情所在的相关目录

		COMMON_API CString GetResEmojiFolder();

		// 获取资源图片所在目录代码中用到的图片
		COMMON_API CString GetResToolsFolder();

		// 获取数据根目录
		COMMON_API CString GetDataFolder(BOOL bCreateIfNotExist = FALSE);

		// 获取Tmp数据根目录
		COMMON_API CString GetDTmpFolder(BOOL bCreateIfNotExist = FALSE);

		// 获取Upgrad目录
		COMMON_API CString GetDUpgradeFolder(BOOL bCreateIfNotExist = FALSE);

		// 获取Url解析的缩略图片数据根目录
		COMMON_API CString GetUrlThumbFolder(BOOL bCreateIfNotExist = FALSE);

		
		COMMON_API CString GetAppReportFolder(BOOL bCreateIfNotExist = FALSE);


		//CEF本地资源路径
		COMMON_API CString GetCefResFolder(BOOL bCreateIfNotExist = FALSE);
		//CefCachePath
		COMMON_API CString GetCefCachePath(BOOL bCreateIfNotExist = FALSE);
		//CEF资源目录
		COMMON_API CString GetCefResPath();
		//CEF User目录
		COMMON_API CString GetCefUserPath(BOOL bCreateIfNotExist = FALSE);
	
		// 将文件路径分割为各名称集合
		COMMON_API size_t SplitString(LPCTSTR pszPath, LPCTSTR pszToken, std::vector<CString>& aFileNames);

		// 标准化文件目录名，即后面不需要带'\'
		COMMON_API CString StandardFilePath(LPCTSTR pszFilePath);

		//将文件相对或绝对路径按照'\'划分为左右两个部分
		COMMON_API BOOL ExtractCurrentPath(LPCTSTR pszFilePath, CString& strCurFilePath, CString& strSubFilePath);

		//获取父目录
		COMMON_API CString GetParentDirPath(CString strFilePath);
		COMMON_API CString GetParentDirName(CString strFilePath);

		//文件目录路径
		COMMON_API BOOL ValidDirPath(CString strDirPath);
		COMMON_API BOOL ValidFilePath(CString strFilePath);
		
		COMMON_API UINT64 GetFileLength(LPCTSTR szFileName);
		COMMON_API CString StdFilePath(CString strPath);
		COMMON_API BOOL IsFileExist(CString strPath);

		//获取文件夹下的所有文件 strPath(要扫描的目录)、vecPath(文件路径)、vecFolder(目录)、flag（扫描模式）
		COMMON_API BOOL GetMyFilePath(CString strPath, vector<CString>&vecPath, vector<CString>&vecFolder, FileScanFlag flag = FileScanFlag::MULTIFOLDER);
		//判断是否是文件
		COMMON_API BOOL IsFilePath(LPCTSTR strPath);
		//获取文件的创建时间、修改时间、访问时间
		COMMON_API BOOL GetFileTime(LPCTSTR szFilePath, SYSTEMTIME &stCreateTime, SYSTEMTIME &stModifyTime, SYSTEMTIME &stVisitTime);
		COMMON_API CString GetFileName(LPCTSTR strPath);
		COMMON_API CString GetFileNameWithExt(LPCTSTR strPath);
		//获取文件后缀
		COMMON_API CString GetFileExt(LPCTSTR strPath);
		//打开文件 或者文件夹 由bIsFolder决定  打开失败 返回错误码
		COMMON_API  UINT64 OpenFile(LPCTSTR strPath, BOOL bIsFolder);
		// 打开目录
		COMMON_API UINT64 OpenFolder(LPCTSTR strFolder);
		//打开文件 或压缩包并选中
		COMMON_API  BOOL OpenFileSelect(LPCTSTR strPath);
		//是否是目录
		COMMON_API BOOL IsDirectory(LPCTSTR lpszPath);
		//删除文件或者文件夹(文件夹下所有文件都会删除) bRecycle true:表示放在回收站、false:表示直接删掉；
		COMMON_API BOOL DeleteFolder(LPCTSTR pszPath, BOOL bRecycle = TRUE);

        COMMON_API BOOL DllRegister(LPCTSTR pszDllName);
		COMMON_API BOOL RegisterLocalServer(LPCTSTR pszExePath);
		
		COMMON_API BOOL FixedTimeMsgLoop(UINT32 ms);
		COMMON_API BOOL GetCmdLineValue(vector<CString>& v, LPCTSTR lpszName, CString& strValue);
		COMMON_API HRESULT CreateObjectFromDllFile(LPCTSTR pszLib, IUnknown* pUnkOuter, REFCLSID rclsid, REFIID riid, LPVOID * ppv);

		COMMON_API UINT64 GetFileModifyTime(LPCTSTR strPath);
		COMMON_API UINT64 GetFileCreateTime(LPCTSTR strPath);
		COMMON_API UINT64 GetFileAccessTime(LPCTSTR strPath);


		//获取指定系统目录
		COMMON_API BOOL MyGetFolderPath(int csidl, LPTSTR pszPath);
	}
}



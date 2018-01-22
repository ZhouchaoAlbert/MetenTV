#pragma once

//	Ŀ¼�ṹ
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
//	C:\Users\ecuser\Documents\EC\1111\Image		GetUserImageFolder		�շ�ͼƬ
//	C:\Users\ecuser\Documents\EC\1111\Face		GetUserFaceFolder		ͷ��	
//	C:\Users\ecuser\Documents\EC\1111\Catch		GetUserCatchFolder		����


//��ÿ������ע������SetExeFolder������ĳЩ����ʱ�����û������UID�����ܻ��д���
//��ȡ�ļ�������������ļ��Ƿ�Ҫ��ȡ���ļ����µ��ļ�
enum FileScanFlag
{
	SINGLEFOLDER,	//�����ļ���
	MULTIFOLDER		//d����ļ���
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
		//��ʼ��
		COMMON_API BOOL	SetExeFolder(CString strFilePath);

		// ��ȡ��װĿ¼
		COMMON_API CString GetInstallFolder();

		// ��ȡ��Exe����Ŀ¼
		COMMON_API CString GetExeFolder();

		// ��ȡ��Config����Ŀ¼
		COMMON_API CString GetCfgFolder();

		// ��ȡ��Log����Ŀ¼
		COMMON_API CString GetLogFolder(BOOL bCreateIfNotExist = FALSE);

		// ��ȡ��Resource����Ŀ¼
		COMMON_API CString GetResFolder();

		// ��ȡ��Resource::XML����Ŀ¼
		COMMON_API CString GetResXmlFolder();

		//��ȡ�����ļ�Ŀ¼
		COMMON_API CString GetResSoundFolder();

		// ��ȡ��Resource::Material����Ŀ¼
		COMMON_API CString GetResMatFolder();

		//��ȡ��Resource::Image ����Ŀ¼
		COMMON_API CString GetResImageFolder();
		//��ȡEmoji�������ڵ����Ŀ¼

		COMMON_API CString GetResEmojiFolder();

		// ��ȡ��ԴͼƬ����Ŀ¼�������õ���ͼƬ
		COMMON_API CString GetResToolsFolder();

		// ��ȡ���ݸ�Ŀ¼
		COMMON_API CString GetDataFolder(BOOL bCreateIfNotExist = FALSE);

		// ��ȡTmp���ݸ�Ŀ¼
		COMMON_API CString GetDTmpFolder(BOOL bCreateIfNotExist = FALSE);

		// ��ȡUpgradĿ¼
		COMMON_API CString GetDUpgradeFolder(BOOL bCreateIfNotExist = FALSE);

		// ��ȡUrl����������ͼƬ���ݸ�Ŀ¼
		COMMON_API CString GetUrlThumbFolder(BOOL bCreateIfNotExist = FALSE);

		
		COMMON_API CString GetAppReportFolder(BOOL bCreateIfNotExist = FALSE);


		//CEF������Դ·��
		COMMON_API CString GetCefResFolder(BOOL bCreateIfNotExist = FALSE);
		//CefCachePath
		COMMON_API CString GetCefCachePath(BOOL bCreateIfNotExist = FALSE);
		//CEF��ԴĿ¼
		COMMON_API CString GetCefResPath();
		//CEF UserĿ¼
		COMMON_API CString GetCefUserPath(BOOL bCreateIfNotExist = FALSE);
	
		// ���ļ�·���ָ�Ϊ�����Ƽ���
		COMMON_API size_t SplitString(LPCTSTR pszPath, LPCTSTR pszToken, std::vector<CString>& aFileNames);

		// ��׼���ļ�Ŀ¼���������治��Ҫ��'\'
		COMMON_API CString StandardFilePath(LPCTSTR pszFilePath);

		//���ļ���Ի����·������'\'����Ϊ������������
		COMMON_API BOOL ExtractCurrentPath(LPCTSTR pszFilePath, CString& strCurFilePath, CString& strSubFilePath);

		//��ȡ��Ŀ¼
		COMMON_API CString GetParentDirPath(CString strFilePath);
		COMMON_API CString GetParentDirName(CString strFilePath);

		//�ļ�Ŀ¼·��
		COMMON_API BOOL ValidDirPath(CString strDirPath);
		COMMON_API BOOL ValidFilePath(CString strFilePath);
		
		COMMON_API UINT64 GetFileLength(LPCTSTR szFileName);
		COMMON_API CString StdFilePath(CString strPath);
		COMMON_API BOOL IsFileExist(CString strPath);

		//��ȡ�ļ����µ������ļ� strPath(Ҫɨ���Ŀ¼)��vecPath(�ļ�·��)��vecFolder(Ŀ¼)��flag��ɨ��ģʽ��
		COMMON_API BOOL GetMyFilePath(CString strPath, vector<CString>&vecPath, vector<CString>&vecFolder, FileScanFlag flag = FileScanFlag::MULTIFOLDER);
		//�ж��Ƿ����ļ�
		COMMON_API BOOL IsFilePath(LPCTSTR strPath);
		//��ȡ�ļ��Ĵ���ʱ�䡢�޸�ʱ�䡢����ʱ��
		COMMON_API BOOL GetFileTime(LPCTSTR szFilePath, SYSTEMTIME &stCreateTime, SYSTEMTIME &stModifyTime, SYSTEMTIME &stVisitTime);
		COMMON_API CString GetFileName(LPCTSTR strPath);
		COMMON_API CString GetFileNameWithExt(LPCTSTR strPath);
		//��ȡ�ļ���׺
		COMMON_API CString GetFileExt(LPCTSTR strPath);
		//���ļ� �����ļ��� ��bIsFolder����  ��ʧ�� ���ش�����
		COMMON_API  UINT64 OpenFile(LPCTSTR strPath, BOOL bIsFolder);
		// ��Ŀ¼
		COMMON_API UINT64 OpenFolder(LPCTSTR strFolder);
		//���ļ� ��ѹ������ѡ��
		COMMON_API  BOOL OpenFileSelect(LPCTSTR strPath);
		//�Ƿ���Ŀ¼
		COMMON_API BOOL IsDirectory(LPCTSTR lpszPath);
		//ɾ���ļ������ļ���(�ļ����������ļ�����ɾ��) bRecycle true:��ʾ���ڻ���վ��false:��ʾֱ��ɾ����
		COMMON_API BOOL DeleteFolder(LPCTSTR pszPath, BOOL bRecycle = TRUE);

        COMMON_API BOOL DllRegister(LPCTSTR pszDllName);
		COMMON_API BOOL RegisterLocalServer(LPCTSTR pszExePath);
		
		COMMON_API BOOL FixedTimeMsgLoop(UINT32 ms);
		COMMON_API BOOL GetCmdLineValue(vector<CString>& v, LPCTSTR lpszName, CString& strValue);
		COMMON_API HRESULT CreateObjectFromDllFile(LPCTSTR pszLib, IUnknown* pUnkOuter, REFCLSID rclsid, REFIID riid, LPVOID * ppv);

		COMMON_API UINT64 GetFileModifyTime(LPCTSTR strPath);
		COMMON_API UINT64 GetFileCreateTime(LPCTSTR strPath);
		COMMON_API UINT64 GetFileAccessTime(LPCTSTR strPath);


		//��ȡָ��ϵͳĿ¼
		COMMON_API BOOL MyGetFolderPath(int csidl, LPTSTR pszPath);
	}
}



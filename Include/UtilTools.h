#pragma once

#include "BaseDefine.h"
#include <atlstr.h>
using namespace ATL;
namespace Util
{
	namespace Tools
	{
		//��ȡMac��ַ��
		COMMON_API INT32 GetMAC(UINT64 *mac);
		COMMON_API INT32 GetMAC(char *pMac);
		//��ȡ���̿ռ��С(�����ֽ���)
		COMMON_API UINT64 GetDiskSpaceTotalSize(CString strDisk);
		COMMON_API UINT64 GetDiskSpaceFreeSize(CString strDisk);
		//ϵͳʱ��ת���ַ���ʱ��(xxxx��-xx��-xx�� xx:xx:xx�� (xx:xx:xx  ʱ �� ��)
		COMMON_API void SystemTimeToStringTime(SYSTEMTIME st, CString &strTime);
		//ϵͳʱ��ת������;
		COMMON_API INT64 SystemTimeToI64Time(SYSTEMTIME st);
		//�ֽ���ת�ַ���(����λ)
		COMMON_API CString DataUnitToStr(UINT64 u32Size);
		//�ַ�������ת����ʱ���
		COMMON_API time_t StringToDatetime(const TCHAR *strTime);
		//��ȡ��Ļ�ķֱ���
		COMMON_API CString  GetScreenResolution();

		//��ȡ����ϵͳ�İ汾��
		COMMON_API CString GetSystemVer();

		//��ȡ��ǰ����ϵͳ�� ��ǰ�ڴ� (ע�������ǲ���ϵͳʶ������ڴ��С)
		COMMON_API  CString GetSystemMemory();

		//��ȡEC ��ǰ���ڴ�ʹ����
		COMMON_API  UINT32 GetECUsedMemory();

		//��ȡ��ǰϵͳʹ�õ����뷨
		COMMON_API CString GetCurrentIme();

		//��ȡEC��ǰʹ�õ�GDI����
		COMMON_API UINT32 GetUsedGDICount();

		//��ȡϵͳ��32bit��64bit������32��ʾ32bit,����64��ʾ64bit,����-1����ʾʧ�ܣ�
		COMMON_API INT32 GetSystemBits();

		COMMON_API CString GetHidePhoneNumber(const CString& strNumber);
	
		//ʱ��ת������ (xx-xx-xx xx:xx:xx)
		COMMON_API time_t StringToDatetime(const TCHAR *strTime);

		// ϵͳ�Ƿ���vista������
		COMMON_API BOOL IsVistaOrLater();
	}
	namespace Process
	{
		//�жϽ����Ƿ���ڣ�
		COMMON_API DWORD IsProcessExist(CString csProcessName);
		//��������
		COMMON_API BOOL CreateProcessEx(CString csProcessPath);
		//ͨ������ID��������
		COMMON_API BOOL KillProcess(DWORD pID);
		//��URL
		COMMON_API void  OpenPage(CString strURL); 
        //��ȡ��ǰ�����ڴ�ռ���������λKB��
        COMMON_API UINT32 GetCurProcMemUser();
		//ͨ������ID��ȡ���ھ��
		COMMON_API HWND GetWindowsHandleByPid(DWORD pID, LPCTSTR pStrName);
	}

	namespace Hash
	{
		COMMON_API UINT32 Hash(const TCHAR* str);
	}
}
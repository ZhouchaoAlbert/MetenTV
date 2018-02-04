// MetenTVProtect.cpp : WinMain ��ʵ��


#include "stdafx.h"
#include "resource.h"
#include "MetenTVProtect_i.h"
#include "xdlldata.h"

#include "Utilkobj.h"
#include "ShmObj.h"

using namespace ATL;


class CMetenTVProtectModule : public ATL::CAtlExeModuleT< CMetenTVProtectModule >
{
public :
	DECLARE_LIBID(LIBID_MetenTVProtectLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_METENTVPROTECT, "{332E24D6-D730-4FE4-A398-C0E2F434A1A4}")
	};

CMetenTVProtectModule _AtlModule;


BOOL HideTaskBarHelper(HWND hWnd, LPARAM lParam)
{
#ifndef ABM_SETSTATE 
#define ABM_SETSTATE 0x0000000a 
#endif

	APPBARDATA apBar;
	memset(&apBar, 0, sizeof(apBar));
	apBar.cbSize = sizeof(apBar);
	apBar.hWnd = hWnd;
	if (apBar.hWnd != NULL)
	{
		apBar.lParam = lParam;
		SHAppBarMessage(ABM_SETSTATE, &apBar); //�����������Զ�����
	}

	return TRUE;
}

void HideTaskBar(BOOL bHide, BOOL* pDoHide)
{
	int nCmdShow;
	LPARAM lParam;

	HWND hWnd = FindWindow(_T("Shell_TrayWnd"), NULL);
	if (bHide == TRUE)
	{
		nCmdShow = SW_HIDE;
		lParam = ABS_AUTOHIDE | ABS_ALWAYSONTOP;
	}
	else
	{
		nCmdShow = SW_SHOW;
		lParam = ABS_ALWAYSONTOP;
	}

	::ShowWindow(hWnd, nCmdShow);//����������
	BOOL bIsHide = ::IsWindowVisible(hWnd);
	//�Ѿ���������״̬����������Ҫ��ʾ����ָ���־
	if (bIsHide != FALSE && bHide == FALSE)
	{
		*pDoHide = FALSE;
	}
	else if (bIsHide == FALSE && bHide != FALSE)
	{
		*pDoHide = TRUE;
	}
	HideTaskBarHelper(hWnd, lParam);
}

//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	//Ψһʵ��
	HANDLE hMutex = Util::kObj::CreateMutexFormat(_T("Proc_MetenTV_IPC_Unique"));
	if (!hMutex)
	{
		ATLASSERT(FALSE);
		return -2;
	}
	HRESULT hr = GetLastError();
	if (ERROR_ALREADY_EXISTS == hr)
	{
		return -3;
	}
	Util::kObj::CMutexLifeWrapper objMutex(hMutex);

	HANDLE hMutex2 = Util::kObj::CreateMutexFormat(_T("Proc_MetenTV_IPC_SHMDATA"));
	CShmObj shm;
	{
		Util::kObj::CAutoMutex	mutex(hMutex2);
		if (mutex.CanAccess())
		{		
			if (shm.NeedInit())
			{
				memset(shm.GetBuf(), 0, shm.GetLen());
			}
		}
	}
	
	MSG msg;
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//�����Ƿ����
			HWND hWnd = FindWindow(_T("MetenTV_Class"), _T("��������̨"));
			HRESULT hr = GetLastError();
			//Util::Log::Info(_T("Launch"), _T("Hwnd: 0x%x, IsWindow: %u, bPerfLaunch: %u, hr: %0xx"), hWnd, ::IsWindow(hWnd), bPerfLaunch, hr);
			if (!hWnd || !::IsWindow(hWnd))
			{
				Util::kObj::CAutoMutex	mutex(hMutex2);
				if (mutex.CanAccess())
				{
					BOOL* do_task_hide = (BOOL*)shm.GetBuf();
					//�����ڴ�����
					if (*do_task_hide)
					{
						HideTaskBar(FALSE, do_task_hide);
					}
				}

				break;
			}
			
			Sleep(500);
		}
	}

	return 0;
//	return _AtlModule.WinMain(nShowCmd);
}


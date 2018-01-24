// CUpdatePromptDlg.h : CUpdatePromptDlg 的声明

#pragma once

#include "resource.h"       // 主符号

#include <atlhost.h>

using namespace ATL;

// CUpdatePromptDlg

class CUpdatePromptDlg : 
	public CAxDialogImpl<CUpdatePromptDlg>
{
public:
	CUpdatePromptDlg()
	{
	}

	~CUpdatePromptDlg()
	{
	}

	enum { IDD = IDD_UPDATEPROMPTDLG };

BEGIN_MSG_MAP(CUpdatePromptDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	CHAIN_MSG_MAP(CAxDialogImpl<CUpdatePromptDlg>)
END_MSG_MAP()

// 处理程序原型: 
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CAxDialogImpl<CUpdatePromptDlg>::OnInitDialog(uMsg, wParam, lParam, bHandled);
		CenterWindow();
		bHandled = TRUE;
		return 1;  // 使系统设置焦点
	}

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
};



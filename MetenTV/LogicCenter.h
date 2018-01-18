#pragma once

#include <atlstr.h>
#include <map>
#include <vector>
#include "UtilFileTrans.h"

using namespace std;
using namespace ATL;

class LogicCenter : public Util::FileTrans::HttpDownLoad::CHttpDownLoadEvent
{
public:
	LogicCenter();
	virtual ~LogicCenter();

	void	OnJsEvent_GetList();
	void	OnJsEvent_SyncList(const map<CString, CString>& list);	//id->url
	void	OnJsEvent_EnterFullScreen();
	void	OnJsEvent_ExitFullScreen();
	void	OnJsEvent_Upgrade(const CString& ver, const CString& url, const CString& md5);

protected:
	virtual void OnHttpDownProgress(UINT64 TaskID, UINT64 pos, UINT64 range, UINT64 speed, UINT32 remain_time);
	virtual void OnHttpDownResult(UINT64 TaskID, UINT32 code);

private:
	void	LoadConf();
	void	SaveConf(const CString& id = _T("")); //empty: all

	void	CallJs_LocalList(const map<CString, CString>& list);	//id->filepath
	void	CallJs_DownCompleted(const CString& id, const CString& filepath);
	void	CallJs_MessageBox(const CString& text);

	enum FILE_ITEM_STATE
	{
		FIE_NONE = 0,
		FIE_DOWNING = 1,
		FIE_AVAILABLE = 2,
		FIE_DISCARD = 3,
	};
	struct FileItem
	{
		FileItem() : state(FIE_NONE), task_id(0), seq(0)
		{
		}

		CString			filepath;
		CString			url;
		FILE_ITEM_STATE	state;
		UINT64			task_id;
		UINT32			seq;
	};
	map<CString, FileItem>	m_items;
	UINT32				m_seq;
	CString				m_strConfFilePath;
	CString				m_strMainHtmlFile;
	UINT64				m_MainHtml_TaskID;
	
	void	GetFilePathList(map<CString, CString>& list);
	BOOL	GetSections(vector<CString>& vSections);
	CString GetStateStrByCode(const FILE_ITEM_STATE& state); 
	FILE_ITEM_STATE GetStateCodeByStr(const CString& str);
};


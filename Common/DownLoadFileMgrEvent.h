#pragma once
#include <atlstr.h>
using namespace ATL;
class CDownLoadFileMgrEvent
{
public:
	virtual void OnHttpDownProgress(UINT64 TaskID, UINT64 pos, UINT64 range, UINT64 speed, UINT32 remain_time) = 0;
	virtual void OnHttpDownResult(UINT64 TaskID, UINT32 code) = 0;
};




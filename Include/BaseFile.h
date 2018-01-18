#pragma once
#include "BaseDefine.h"
#include <atlstr.h>
using namespace ATL;
class COMMON_API CBaseFile
{
public:
	CBaseFile();
	~CBaseFile();
public:
	BOOL OpenFile(LPCTSTR strFilePath, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition);
	UINT64 GetFileLen();
	//uFileSeekCount 移动的字节数 uFileMethod基准位置
	BOOL   SetFilePoint(UINT64 uFileSeekCount,UINT32 uBasePos);
	//获取 文件内容 buffer 返回实际读取的字节数  
	UINT64 GetFileContent(UINT8* pBuffer,UINT64 uBufferLen);
	void CloseFile();
private:
	HANDLE m_hHande;
};


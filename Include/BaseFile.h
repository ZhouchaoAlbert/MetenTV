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
	//uFileSeekCount �ƶ����ֽ��� uFileMethod��׼λ��
	BOOL   SetFilePoint(UINT64 uFileSeekCount,UINT32 uBasePos);
	//��ȡ �ļ����� buffer ����ʵ�ʶ�ȡ���ֽ���  
	UINT64 GetFileContent(UINT8* pBuffer,UINT64 uBufferLen);
	void CloseFile();
private:
	HANDLE m_hHande;
};


#include "stdafx.h"
#include "BaseFile.h"


CBaseFile::CBaseFile()
{
	m_hHande = NULL;
}


CBaseFile::~CBaseFile()
{
	if (NULL != m_hHande)
	{
		CloseHandle(m_hHande);
		m_hHande = NULL;
	}
}
BOOL CBaseFile::OpenFile(LPCTSTR strFilePath, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition)
{
	HANDLE hFileHandle = CreateFile(strFilePath, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, NULL, NULL);
	if (INVALID_HANDLE_VALUE == hFileHandle)
	{
		CloseHandle(hFileHandle);
		return FALSE;
	}
	m_hHande = hFileHandle;
	return TRUE;
}
UINT64 CBaseFile::GetFileLen()
{
	if (NULL == m_hHande)
	{
		return 0;
	}
	DWORD dfileSize = GetFileSize(m_hHande, NULL);
	return dfileSize;
}
BOOL CBaseFile::SetFilePoint(UINT64 uFileSeekCount, UINT32 uBasePos)
{
	if (NULL == m_hHande)
		return FALSE;
	DWORD dwRes = SetFilePointer(m_hHande, (LONG)uFileSeekCount, 0, uBasePos);
	if (INVALID_SET_FILE_POINTER == dwRes)
	{
		return FALSE;
	}
	return TRUE;
}
//获取 文件内容 buffer 返回实际读取的字节数  
UINT64 CBaseFile::GetFileContent(UINT8* pBuffer, UINT64 uBufferLen)
{
	DWORD readsize = 0;
	ReadFile(m_hHande, pBuffer, (DWORD)uBufferLen, &readsize, NULL);
	return readsize;
}
void CBaseFile::CloseFile()
{
	if (NULL != m_hHande)
	{
		CloseHandle(m_hHande);
		m_hHande = NULL;
	}
}

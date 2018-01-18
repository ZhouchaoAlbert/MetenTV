#ifndef __LOG_NEW_H__
#define __LOG_NEW_H__

#include <atlstr.h>
#include <list>
#include "UtilLog.h"

using namespace ATL;
using namespace std;

class CLogNew
{
protected:
	CLogNew();
public:
	~CLogNew();
public:
	static CLogNew * GetObject();
	// ��ʼ����־��
	bool Init();
	// ж����־��
	void UnInit();
	// ������־�ȼ�
	void SetLevel(Util::Log::EnumRawLogLevel eLogLevel);
	// �ı���־·����Ϣ
	BOOL Change(LPCTSTR szFilePath, UINT32 u32UID);
	// ƴ��־�ַ���
	void Write(Util::Log::EnumRawLogLevel eLogLevel, LPCTSTR pchKeyword, LPCTSTR pchFmt, va_list va);
private:
	// �����־���ݵ�����
	void Add(const CString & strText);
	// ��־ͷ
	void GetText(CString& strText, LPCTSTR pchKeyword, LPCTSTR pchFmt, va_list va);
	// ��־�ļ�д��
	void Writes();
	// ���ļ�
	bool OpenFile(LPCTSTR pchFilePath);
	// �ر��ļ�
	void CloseFile();
	// д�ļ�
	void WriteFile(const CString & strBuffer);
	// �ļ��Ƿ����
	bool IsTooLarge();
	// ����������
	void Backup();
	// �����ļ�������
	void FileAppendNumber(CString & strFilePath);
	// ��־�߳�
	static UINT WINAPI ThreadProc(void* pVoid);
private:
	bool						m_bRun;
	HANDLE						m_hThread;
	HANDLE						m_hFile;
	CString						m_strFilePath;
	list<CString>				m_Logs;
	UINT32						m_u32UserID;
	CRITICAL_SECTION			m_cs;
	Util::Log::EnumRawLogLevel	m_eLogLevel;
};

#endif


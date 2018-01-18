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
	// 初始化日志库
	bool Init();
	// 卸载日志库
	void UnInit();
	// 设置日志等级
	void SetLevel(Util::Log::EnumRawLogLevel eLogLevel);
	// 改变日志路径信息
	BOOL Change(LPCTSTR szFilePath, UINT32 u32UID);
	// 拼日志字符串
	void Write(Util::Log::EnumRawLogLevel eLogLevel, LPCTSTR pchKeyword, LPCTSTR pchFmt, va_list va);
private:
	// 添加日志数据到队列
	void Add(const CString & strText);
	// 日志头
	void GetText(CString& strText, LPCTSTR pchKeyword, LPCTSTR pchFmt, va_list va);
	// 日志文件写入
	void Writes();
	// 打开文件
	bool OpenFile(LPCTSTR pchFilePath);
	// 关闭文件
	void CloseFile();
	// 写文件
	void WriteFile(const CString & strBuffer);
	// 文件是否过大
	bool IsTooLarge();
	// 备份老数据
	void Backup();
	// 重名文件加数字
	void FileAppendNumber(CString & strFilePath);
	// 日志线程
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


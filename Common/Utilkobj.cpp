#include "stdafx.h"
#include "Utilkobj.h"

#include "atlstr.h"

HANDLE Util::kObj::CreateMutexFormat(LPCTSTR pstrFmt, ...)
{
	va_list va = (va_list)(&pstrFmt + 1);

	CString str;
	str.FormatV(pstrFmt, va);

	HANDLE h = CreateMutex(NULL, FALSE, str);
	return h;
}

HANDLE Util::kObj::OpenMutexFormat(LPCTSTR pstrFmt, ...)
{
    va_list va = (va_list)(&pstrFmt + 1);

    CString str;
    str.FormatV(pstrFmt, va);

    HANDLE h = OpenMutex(MUTEX_ALL_ACCESS, FALSE, str);
    return h;
}

UINT64 Util::kObj::GetEscapeCpuRDTSC()
{
	__asm _emit 0x0F
	__asm _emit 0x31
	/*

	unsigned long high32 = 0, low32 = 0;
	#ifdef WIN32 // WIN32
	_asm
	{
	RDTSC;
	mov high32, ebx;
	mov low32, eax;
	}
	#else
	__asm__("RDTSC" : "=a"(low32), "=d"(high32));
	#endif
	unsigned long long counter = high32;
	counter = (counter << 32) + low32;
	return counter;
	*/
}

Util::kObj::CAutoCriticalSection::CAutoCriticalSection(CCriticalSection* pCS, BOOL bEnable/* = TRUE*/)
{
	m_bEnable = bEnable;
	if (bEnable)
	{
		ATLASSERT(NULL != pCS);
		m_pCS = pCS;
		m_pCS->Enter();
	}
}

Util::kObj::CAutoCriticalSection::~CAutoCriticalSection()
{
	if (m_bEnable)
	{
		ATLASSERT(NULL != m_pCS);
		m_pCS->Leave();
	}
}

Util::kObj::CAutoMutex::CAutoMutex(HANDLE hMutex, DWORD dwMsWait/* = INFINITE*/)
{
	ATLASSERT(hMutex);
	m_hMutex = hMutex;

	m_bCanAccess = FALSE;
	DWORD dwRet = WaitForSingleObject(m_hMutex, dwMsWait);
	if (WAIT_OBJECT_0 == dwRet)
	{
		m_bCanAccess = TRUE;
	}
	else if (WAIT_ABANDONED == dwRet)
	{
		ReleaseMutex(m_hMutex);
	}
}

Util::kObj::CAutoMutex::~CAutoMutex()
{
	if (m_bCanAccess)
	{
		ReleaseMutex(m_hMutex);
	}
}

BOOL	Util::kObj::CAutoMutex::CanAccess()
{
	return m_bCanAccess;
}
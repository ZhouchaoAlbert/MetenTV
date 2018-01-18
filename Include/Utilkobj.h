#pragma once

#include "BaseDefine.h"
#include <atlsync.h>

using namespace ATL;

namespace Util
{
	namespace kObj
	{
		HANDLE COMMON_API CreateMutexFormat(LPCTSTR pstrFmt, ...);
		HANDLE COMMON_API OpenMutexFormat(LPCTSTR pstrFmt, ...);
		UINT64 COMMON_API GetEscapeCpuRDTSC();

		class COMMON_API CAutoCriticalSection
		{
		public:
			CAutoCriticalSection(CCriticalSection* pCS, BOOL bEnable = TRUE);
			virtual ~CAutoCriticalSection();

		private:
			CCriticalSection*	m_pCS;
			BOOL m_bEnable;
		};

        //封装生命期
		class COMMON_API CMutexLifeWrapper
        {
        public:
            CMutexLifeWrapper(HANDLE hMutex)
            {
                m_hMutex = hMutex;
            }
            ~CMutexLifeWrapper()
            {
                if (NULL != m_hMutex)
                {
                    ::CloseHandle(m_hMutex);
                    m_hMutex = NULL;
                }
            }
        private:
            HANDLE m_hMutex;
        };

        //不管hMutex生命期，只封装WaitSingleObject
		class COMMON_API CAutoMutex
		{
		public:
			CAutoMutex(HANDLE hMutex, DWORD dwMsWait = INFINITE);
			virtual ~CAutoMutex();

			BOOL	CanAccess();

		private:
			HANDLE	m_hMutex;
			BOOL	m_bCanAccess;
		};
	}
}



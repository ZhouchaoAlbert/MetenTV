#pragma once

#include <basetsd.h>
#include <memory>
#include "concurrent_queue.h"
#include "BaseDefine.h"
#include "UtilWinMsg.h"
#include "XTimerSinkHelper.h"
#include "atlstr.h"
#include "UtilLog.h"

using namespace ATL;
using namespace std;

//ע�⣺Sink��Work��1��1������1��Sink�����ӦN��N > 1����Work���󣻶���ģ�����C2W��W2C���Ͷ�Sink��Work���뽻��һ�£���Ϊsink��work�Ļ�ȡ/������ͨ��void*ǿ��ת�ͣ��粻һ�������Ͳ���ȫ
//�������߳�ʹ���˶�ʱ�������������߳̽��մ�����Ϣ������������Ҫ���ݣ��߳��� * ��ʱ��ʱ����/��ʱ * ÿ�γ���ѭ��Ȧ��FETCH_W2CMSG_LOOPNUM
//���ǵ����ǲ�����TerminateThread�����޷�Ԥ�ϵı��������Թ����̱߳��룺1���Լ���������Ƿ���Ҫ�˳���2������Sink��Work������new�����ģ������̳߳����ʵ�ʱ����������3��Sink��Work���������virtual����
//�ر�ע�⣺�ⲿ����������������������Ȳ���ֱ�ӵ���DelTask��ֻ��new������delete

//
//Sink��Ӧ�ò����ʵ��OnFreeParam��OnW2CMsgCome�����Ը��Ի�����SetFetchParam������ѯTimer�ӳټ��������ÿ����ѯʱ������ȡ���ٸ����ݡ������������ƴﲻ��Ҫ��Ҳ��������OnFetchMsg
//			��������	OnFreeParam	OnW2CMsgCome
//			��ѡ����	OnFetchMsg	OnWorkFinish

//Work��Ӧ�ò����ʵ��Run_InWorkThread������̬����CheckExit_InWorkThread�����õ��ó���ײ���̰߳�ȫ�ĺ������࣬������ʹ�ó�Ա�����⣩��ֱ�������̷߳������ݺͱ�������
//			��������	Run_InWorkThread
//			��ѡ����
//			�������	GetC2WMsg_InWorkThread�������Ե��ã���Ϊ�����������C2Wͨ�����ݻ�ȡ�������˳���Ϣ
//		���ǵ�ϵͳ�ۺ����ܣ����ڷ������Եĳ��ϣ�������һЩSleep���������ѯ��������Բο�CLpcWork��ʵʩ����

//���ǵ����أ���Ϊ���̴߳����ٶȿ��ܳ�Ϊƿ����������Ҫ������������һ����˵�������̵߳�������������߳����룩����������ģ�ͣ�״̬���ۻ���
//ǰ�߻�����ڹ����߳������ڸ���������ܵ�����������ʵ������������Ʒ���

//�ڲ�ɾ����Work��Ч��Ϊ��׼����Work�յ����̼߳�SinkҪ����˳���Ϣ��ϵͳ�ŻὫ��ɾ��

//ע��Sink��OnXxx������Ҫ����ס�����DoModal����Ϊ�����ڹ������̳߳ع���ʲôʱ�������ǲ�ȷ���ģ�DoModalֻ��������Model���ڣ��������������߳���������/�¼��ʹ���

namespace Util
{
	namespace ThreadPool2
	{
		//Killǰ�ȴ��̳߳�ʱʱ��
		const UINT32	KILLTHREAD_WAITTIME = 250;
		const UINT32	EXITTHREADCONFORM_WAITTIME = 100;
		//sys����
		const INT32		SYSCMD_C2W_EXITBYSINK = 0x01;
		const INT32		SYSCMD_W2C_LOG = 0x02;
		const INT32		SYSCMD_W2C_THREADFINISH = 0x03;
		//Fetch

		const UINT32	FETCH_W2CMSG_DELAYTIME = 100;	//�೤����ִ��һ��Fetch��
		const UINT32	FETCH_W2CMSG_NUMONCE = 10;		//���Fetchϣ��ȡ����������
		const UINT32	FETCH_W2CMSG_LOOPNUM = 10;		//ÿȡһ�����ݳ��Զ��ٴ�try_pop����Ϊ�����е�������ϵͳ������log������Ӧ�ò��ע��
		const UINT32	CHECK_C2WMSG_ONLYEXITNUM = 100;	//����100�Σ�ֱ��Exit��Ϣ
		template<typename cmddata_t>
		struct queueitem_t
		{
			BOOL		sys;	//ϵͳ��������
			INT32		cmd;
			cmddata_t	data;
			BSTR		bstr;
			INT32		pad;
			queueitem_t()
			{
				sys = FALSE;
				cmd = 0;
				bstr = NULL;
				pad = 0;
			}
			queueitem_t(INT32 _cmd, cmddata_t _data, BOOL _sys = FALSE, INT32 _pad = 0, LPCTSTR pszText = NULL)
			{
				sys = _sys;
				cmd = _cmd;
				data = _data;
				pad = _pad;
				bstr = pszText ? ::SysAllocString(pszText) : NULL;
			}
		};
		//--------------------------------------------------//
		class  IThreadObjSink
		{
		public:
			IThreadObjSink(){  }
			virtual ~IThreadObjSink(){}//����Ϊvirtual

			virtual BOOL	GetSinkMeta(HWND* pHwnd, UINT32* pMsgID, UINT32* pWaitSleep, void** ppC2W, void** ppW2C) = 0;
			virtual void	OnFreeParam(void* pThreadParam) = 0;	//ʹ�����߳��յ���Ϣ�����������˳�
			virtual BOOL	PostExitMsg() = 0;						//ʹ�����߳��յ���Ϣ�����������˳�
			virtual void	SetTaskID(UINT64 u64TaskID) = 0;
			virtual void	FetchAllMsg() = 0;
		};

		//���������߳�
		template<typename c2w_t, typename w2c_t>
		class  CThreadObjSink : public IThreadObjSink	//Ӧ�ò����ʵ��OnFreeParam��OnW2CMsgCome
		{
		public:
			CThreadObjSink()
			{
				m_queueC2W = new Concurrency::concurrent_queue<queueitem_t<c2w_t> >;
				m_queueW2C = new Concurrency::concurrent_queue<queueitem_t<w2c_t> >;

				m_u32TimerInterval = FETCH_W2CMSG_DELAYTIME;
				m_u32FetchNumOnce = FETCH_W2CMSG_NUMONCE;

				m_shWinMsg.HookEvent(evtOnAssociateMsg, this, &CThreadObjSink::OnNewMsg);
				m_shTimer.HookEvent(evtOnTimer, this, &CThreadObjSink::OnFetchDelayTimer);

				m_u64TaskID = 0;
			}
			virtual ~CThreadObjSink()//����Ϊvirtual
			{
				ATLTRACE(_T("~CThreadObjSink: 0x%x\n"), &m_shTimer);
				m_shTimer.UnhookEvent(evtOnTimer, this, &CThreadObjSink::OnFetchDelayTimer);
				m_shWinMsg.UnhookEvent(evtOnAssociateMsg, this, &CThreadObjSink::OnNewMsg);

				SAFEFREENEW(m_queueC2W);
				SAFEFREENEW(m_queueW2C);
			}

			virtual void OnW2CMsgCome(INT32& cmd, w2c_t& msg) = 0;		//ʵ��ΪWM_MSG+��ʱ�����ʵ��
			virtual void OnWorkFinish()							//������ʾAttach�Ĺ����߳���������乤����һ����˵��������ΪCheckExit_InWorkThread�������˳���
			{
				PostExitMsg();
			}
			UINT64	GetTaskID(){ return m_u64TaskID; }

		protected:
			//ϣ�����ŵ���ֹ�������
			virtual BOOL	PostExitMsg()						//ʹ�����߳��յ���Ϣ�����������˳�
			{
				c2w_t msg;
				queueitem_t<c2w_t> item(SYSCMD_C2W_EXITBYSINK, msg, TRUE);
				BOOL bRet = PostC2WMsgHelper(item);
				return bRet;
			}
			void	SetTaskID(UINT64 u64TaskID)
			{
				m_u64TaskID = u64TaskID;
			}

			void	SetFetchParam(UINT32 u32TimerInterval, UINT32 u32FetchNumOnce)
			{
				m_u32TimerInterval = u32TimerInterval;
				m_u32FetchNumOnce = u32FetchNumOnce;

				if (!m_shTimer.IsActive())
				{
					m_shTimer.Start(_T("FetchDelayTimer"), m_u32TimerInterval);
				}
			}
			BOOL	PostC2WMsg(INT32 cmd, c2w_t msg)
			{
				queueitem_t<c2w_t> item(cmd, msg);
				return PostC2WMsgHelper(item);
			}
			BOOL	GetW2CMsg(INT32& cmd, w2c_t& msg)
			{
				queueitem_t<w2c_t> item;
				if (GetW2CMsgHelper(item))
				{
					ATLASSERT(!item.sys);
					cmd = item.cmd;
					msg = item.data;

					return TRUE;
				}

				return FALSE;
			}
			virtual BOOL GetSinkMeta(HWND* pHwnd, UINT32* pMsgID, UINT32* pWaitSleep, void** ppC2W, void** ppW2C)
			{
				*pHwnd = m_shWinMsg.GetHwnd(); //Ĭ�ϻ����m_shWinMsg��Enable
				*pMsgID = m_shWinMsg.GetMsgID();
				*ppC2W = m_queueC2W;
				*ppW2C = m_queueW2C;
				*pWaitSleep = KILLTHREAD_WAITTIME;
				
				return TRUE;
			}
			virtual void FetchAllMsg()
			{
				INT32 cmd = 0;
				w2c_t msg;
				for (;;)
				{
					if (GetW2CMsg(cmd, msg))
					{
						OnW2CMsgCome(cmd, msg);
					}
					else
					{
						break;
					}
				}
			}
			virtual void OnFetchMsg()
			{
				INT32 cmd = 0;
				w2c_t msg;
				for (size_t i = 0; i < m_u32FetchNumOnce; i++)
				{
					if (GetW2CMsg(cmd, msg))
					{
						OnW2CMsgCome(cmd, msg);
					}
					else
					{
						break;
					}
				}
			}

		private:
			DECLARE_EVENT_RECEIVER2(COMMON_API, CThreadObjSink)
			Concurrency::concurrent_queue<queueitem_t<c2w_t> >*		m_queueC2W;	//���ഴ��������
			Concurrency::concurrent_queue<queueitem_t<w2c_t> >*		m_queueW2C;	//���ഴ��������
			Util::WinMsg::CAssociateMsgSinkHelper		m_shWinMsg;
			XTimerSinkHelper							m_shTimer;
			UINT32										m_u32TimerInterval;
			UINT32										m_u32FetchNumOnce;
			UINT64										m_u64TaskID;

			BOOL	PostC2WMsgHelper(queueitem_t<c2w_t>& item)
			{
				ATLASSERT(m_queueC2W);

				m_queueC2W->push(item);
				return TRUE;
			}
			BOOL	GetW2CMsgHelper(queueitem_t<w2c_t>& item)
			{
				ATLASSERT(m_queueW2C);

				//tbd׷�٣������������ٶ����Ż�
				for (UINT32 i = 0; i < FETCH_W2CMSG_LOOPNUM; i++)
				{
					if (m_queueW2C->try_pop(item))
					{
						if (item.sys)
						{
							switch (item.cmd)
							{
							case SYSCMD_W2C_LOG:
								ATLASSERT(item.bstr);
								{
									CString str = COLE2T(item.bstr);
									::SysFreeString(item.bstr);
									item.bstr = NULL;

									Util::Log::EnumRawLogLevel level = (Util::Log::EnumRawLogLevel)item.pad;
									switch (level)
									{
									case Util::Log::E_RLL_NONE:
										ATLASSERT(FALSE);
										break;
									case Util::Log::E_RLL_DEBUG:
										Util::Log::Debug(_T("WorkThread"), _T("%s"), str);
										break;
									case Util::Log::E_RLL_INFO:
										Util::Log::Info(_T("WorkThread"), _T("%s"), str);
										break;
									case Util::Log::E_RLL_WARN:
										Util::Log::Warn(_T("WorkThread"), _T("%s"), str);
										break;
									case Util::Log::E_RLL_ERROR:
										Util::Log::Error(_T("WorkThread"), _T("%s"), str);
										break;
									case Util::Log::E_RLL_FATAL:
										Util::Log::Fatal(_T("WorkThread"), _T("%s"), str);
										break;
									case Util::Log::E_RLL_SYSTEM:
										ATLASSERT(FALSE);
										break;
									default:
										ATLASSERT(FALSE);
										break;
									}
								}
								break;
							case SYSCMD_W2C_THREADFINISH:
								OnWorkFinish();
								break;
							default:
								ATLASSERT(FALSE);
								break;
							}
						}
						else
						{
							return TRUE;
						}
					}
					else
					{
						break;
					}
				}

				return FALSE;
			}
			void OnNewMsg(WPARAM wParam, LPARAM lParam)
			{
				if (!m_shTimer.IsActive())
				{
					m_shTimer.Start(_T("FetchDelayTimer"), m_u32TimerInterval);
				}
			}
			void OnFetchDelayTimer()
			{
				OnFetchMsg();

				//��������ûȡ�ꣿ�����´�
				if (m_queueW2C->empty())
				{
					m_shTimer.Stop(_T("FetchDelayTimer"));
				}
			}
		};

		//--------------------------------------------------//
		class  IThreadObjWork
		{
		public:
			IThreadObjWork(){  }
			virtual ~IThreadObjWork(){}//����Ϊvirtual
			
			virtual void Run_InWorkThread(void* pParam) = 0;	//ע����Ϊ��Ҫ���ǵ����������˳���������ʱ������Ҫ����Sleep������ܴ���m_u32WaitSleep/2
			virtual BOOL SetSinkMeta(HWND Hwnd, UINT32 MsgID, UINT32 WaitSleep, void* pC2W, void* pW2C) = 0;
			virtual BOOL NotifyThreadFinish_InWorkThread() = 0;
			virtual BOOL CheckExit_InWorkThread() = 0;
			virtual BOOL FilterExit_InWorkThread() = 0;
			virtual void SetTaskID(UINT64 u64TaskID) = 0;

		};

		//���������ڹ����̣߳����죨��ʼ����������������ʼ�����������������߳�
		template<typename c2w_t, typename w2c_t>
		class  CThreadObjWork : public IThreadObjWork
		{
		public:
			CThreadObjWork()
			{
				m_queueC2W = NULL;
				m_queueW2C = NULL;
				m_hHostWnd = NULL;
				m_u32HostMsgID = 0;
				m_bExitBySink = FALSE;
				m_u32WaitSleep = KILLTHREAD_WAITTIME;
				m_u64TaskID = 0;
			}
			virtual ~CThreadObjWork()	//����Ϊvirtual
			{
				m_queueC2W = NULL;
				m_queueW2C = NULL;
				m_hHostWnd = NULL;
				m_u32HostMsgID = 0;
				m_u32WaitSleep = KILLTHREAD_WAITTIME;
				m_u64TaskID = 0;
			}
			UINT64	GetTaskID(){ return m_u64TaskID; }

		protected:
			//ִ���߳��������һ���ʱ��������������Ա���ǿ����ֹ�̣߳�����TRUE��ʾ�ⲿϣ�����˳�
			BOOL	CheckExit_InWorkThread()
			{
				return m_bExitBySink;
			}
			BOOL	FilterExit_InWorkThread()
			{
				for (size_t i = 0; i < CHECK_C2WMSG_ONLYEXITNUM && !m_bExitBySink; i++)
				{
					queueitem_t<c2w_t> item;
					if (!GetC2WMsgHelper_InWorkThread(item))
					{
						break;
					}
				}

				return m_bExitBySink;
			}
			void	SetTaskID(UINT64 u64TaskID)
			{ 
				m_u64TaskID = u64TaskID; 
			}
			void	NotifyNewW2CMsg_InWorkThread()
			{
				ATLASSERT(m_hHostWnd);
				ATLASSERT(m_u32HostMsgID);
				::PostMessage(m_hHostWnd, m_u32HostMsgID, 0L, 0L);
			}
			BOOL NotifyThreadFinish_InWorkThread()//��ͨ����Ϣ����ʹ��queue����Ϊϣ��������������
			{
				w2c_t msg; 
				queueitem_t<w2c_t> item(SYSCMD_W2C_THREADFINISH, msg, TRUE);
				BOOL bRet = PostW2CMsgHelper_InWorkThread(item);
				if (bRet)
				{
					NotifyNewW2CMsg_InWorkThread();
				}
				return bRet;
			}
			void	Log_InWorkThread(Util::Log::EnumRawLogLevel level, LPCTSTR pstrFmt, ...)
			{
				CString strText;
				va_list va = (va_list)(&pstrFmt + 1);
				strText.FormatV(pstrFmt, va);

				w2c_t msg;
				queueitem_t<w2c_t> item(SYSCMD_W2C_LOG, msg, TRUE, level, strText);
				PostW2CMsgHelper_InWorkThread(item);
			}
			BOOL	GetC2WMsg_InWorkThread(INT32& cmd, c2w_t& msg)
			{
				queueitem_t<c2w_t> item;
				if (GetC2WMsgHelper_InWorkThread(item))
				{
					ATLASSERT(!item.sys);
					cmd = item.cmd;
					msg = item.data;

					return TRUE;
				}

				return FALSE;
			}
			UINT32 GetC2WMsgCount()
			{
				return m_queueC2W->unsafe_size()
			}
			BOOL	PostW2CMsg_InWorkThread(INT32 cmd, w2c_t msg)
			{
				queueitem_t<w2c_t> item(cmd, msg);
				return PostW2CMsgHelper_InWorkThread(item);
			}

			UINT32	m_u32WaitSleep;
		private:
			Concurrency::concurrent_queue<queueitem_t<c2w_t> >*		m_queueC2W;	//�����������
			Concurrency::concurrent_queue<queueitem_t<w2c_t> >*		m_queueW2C;	//�����������
			HWND	m_hHostWnd;
			UINT32	m_u32HostMsgID;
			BOOL	m_bExitBySink;	
			UINT64	m_u64TaskID;
			
			virtual BOOL SetSinkMeta(HWND Hwnd, UINT32 MsgID, UINT32 WaitSleep, void* pC2W, void* pW2C)
			{
				m_u32HostMsgID = MsgID;
				m_queueC2W = (Concurrency::concurrent_queue<queueitem_t<c2w_t> >*)pC2W;
				m_queueW2C = (Concurrency::concurrent_queue<queueitem_t<w2c_t> >*)pW2C;
				m_hHostWnd = Hwnd;
				m_u32WaitSleep = WaitSleep;

				return TRUE;
			}
			BOOL	PostW2CMsgHelper_InWorkThread(queueitem_t<w2c_t>& item)
			{
				ATLASSERT(m_queueW2C);

				//����Ѿ�ȷ���˳�����ô���ֻ�ܷ�ϵͳ��Ϣ������Ϣ
				if (m_bExitBySink)
				{
					if (!item.sys)
					{
						return FALSE;
					}
				}

				m_queueW2C->push(item);
				return TRUE;
			}
			BOOL	GetC2WMsgHelper_InWorkThread(queueitem_t<c2w_t>& item)
			{
				ATLASSERT(m_queueC2W);

				//tbd׷�٣������������ٶ����Ż�

				for (size_t i = 0; i < 2; i++)
				{
					if (m_queueC2W->try_pop(item))
					{
						if (item.sys)
						{
							switch (item.cmd)
							{
							case SYSCMD_C2W_EXITBYSINK:
								m_bExitBySink = TRUE;
								break;
							default:
								ATLASSERT(FALSE);
								break;
							}
						}
						else
						{
							return TRUE;
						}
					}
				}

				return FALSE;
			}
		};

		//---------------------------------------------------//
		enum ThreadSlotClass
		{
			TSC_NONE = 0,
			TSC_CORE = 1,	//����ҵ��ģ��
			//�ļ���������
			TSC_FILETRANS_NORMAL ,  // �����ļ���
			TSC_FILETRANS_PIC,             //ͼƬ��
			TSC_FILETRANS_RECORD,   //¼���ļ���
			TSC_FILETRANS_NET_DETECTION,    //����̽���
			TSC_FILETRANS_DOWN_FILE,         //down file  max 5
			TSC_FILETRANS_UPLOAD_LOG,		 //��־�ļ��ϴ� MAX 5
			TSC_NETBROKEN_CHECK,	//������
			TSC_ALIOSSKEY,    //��ȡ����key���̲߳� �ұ��� ���ҽ���һ����Run

			TSC_HTTPS_REQUEST,	//���ڵ�����CURL���HTTPS�������߳�
			TSC_DOMAIN_DETECT,	//̽���������

			TSC_EMAILPLANE,        //�ʼ��ƻ�
			TSC_EXTRACTZIP,        //CEF ��ѹ��Դ
		};
		COMMON_API void		SetThreadNum(ThreadSlotClass cls, UINT8 high);	//Ĭ��ÿ�����ͻ���THREADNUM_DEFAULT���߳�
		COMMON_API UINT64	AddTask(ThreadSlotClass cls, IThreadObjSink* pISink, IThreadObjWork* pIWork, void* pThreadParam);	//ע��pParam�����ڣ������new����ָ�룬��������Ϊsink��һ����Ա����������������ʱ���ͷ�,����0��ʾʧ��
		COMMON_API void		DelTask(UINT64 id);	
	}
}

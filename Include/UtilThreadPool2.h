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

//注意：Sink和Work是1对1，不能1个Sink对象对应N（N > 1）个Work对象；而且模版参数C2W和W2C类型对Sink和Work必须交叉一致，因为sink到work的获取/设置是通过void*强制转型，如不一样会类型不安全
//由于主线程使用了定时器，请评估主线程接收处理消息的吞吐量：主要依据：线程数 * 定时器时间间隔/耗时 * 每次尝试循环圈数FETCH_W2CMSG_LOOPNUM
//考虑到我们不主动TerminateThread避免无法预料的崩溃，所以工作线程必须：1，自己经常检查是否需要退出；2，所有Sink和Work必须是new出来的，便于线程池在适当时机主动清理；3，Sink和Work派生类必须virtual析构
//特别注意：外部（包括派生类的析构函数等不能直接调用DelTask）只能new，不能delete

//
//Sink：应用层必须实现OnFreeParam和OnW2CMsgCome，可以个性化设置SetFetchParam：“轮询Timer延迟间隔”，“每次轮询时候最多获取多少个数据”，如果这个控制达不到要求，也可以重载OnFetchMsg
//			必须重载	OnFreeParam	OnW2CMsgCome
//			可选重载	OnFetchMsg	OnWorkFinish

//Work：应用层必须实现Run_InWorkThread，并常态调用CheckExit_InWorkThread，不得调用程序底层非线程安全的函数和类，（除了使用成员函数外）不直接与主线程发生数据和变量交互
//			必须重载	Run_InWorkThread
//			可选重载
//			必须调用	GetC2WMsg_InWorkThread，周期性调用，因为这个函数触发C2W通信数据获取，包括退出消息
//		考虑到系统综合性能，对于非阻塞性的场合，建议做一些Sleep，以免空轮询，具体可以参考CLpcWork的实施策略

//考虑到流控，因为主线程处理速度可能成为瓶颈，所以需要评估吞吐量，一般来说，工作线程的数据输出（主线程输入）可能有两种模型：状态和累积。
//前者或可以在工作线程做周期更新再输入管道；后者依据实际情况采用类似方法

//内部删除以Work有效性为基准，但Work收到主线程即Sink要求的退出消息后，系统才会将其删除

//注意Sink的OnXxx函数不要阻塞住如调用DoModal，因为生命期管理受线程池管理，什么时候析构是不确定的，DoModal只是限制主Model窗口，不会限制主该线程其他窗口/事件和处理

namespace Util
{
	namespace ThreadPool2
	{
		//Kill前等待线程超时时间
		const UINT32	KILLTHREAD_WAITTIME = 250;
		const UINT32	EXITTHREADCONFORM_WAITTIME = 100;
		//sys命令
		const INT32		SYSCMD_C2W_EXITBYSINK = 0x01;
		const INT32		SYSCMD_W2C_LOG = 0x02;
		const INT32		SYSCMD_W2C_THREADFINISH = 0x03;
		//Fetch

		const UINT32	FETCH_W2CMSG_DELAYTIME = 100;	//多长毫秒执行一次Fetch？
		const UINT32	FETCH_W2CMSG_NUMONCE = 10;		//这次Fetch希望取出多少数据
		const UINT32	FETCH_W2CMSG_LOOPNUM = 10;		//每取一次数据尝试多少次try_pop？因为数据有但可能是系统本身如log而不是应用层关注的
		const UINT32	CHECK_C2WMSG_ONLYEXITNUM = 100;	//最多查100次，直到Exit消息
		template<typename cmddata_t>
		struct queueitem_t
		{
			BOOL		sys;	//系统保留控制
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
			virtual ~IThreadObjSink(){}//必须为virtual

			virtual BOOL	GetSinkMeta(HWND* pHwnd, UINT32* pMsgID, UINT32* pWaitSleep, void** ppC2W, void** ppW2C) = 0;
			virtual void	OnFreeParam(void* pThreadParam) = 0;	//使工作线程收到消息后酌情主动退出
			virtual BOOL	PostExitMsg() = 0;						//使工作线程收到消息后酌情主动退出
			virtual void	SetTaskID(UINT64 u64TaskID) = 0;
			virtual void	FetchAllMsg() = 0;
		};

		//工作于主线程
		template<typename c2w_t, typename w2c_t>
		class  CThreadObjSink : public IThreadObjSink	//应用层必须实现OnFreeParam和OnW2CMsgCome
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
			virtual ~CThreadObjSink()//必须为virtual
			{
				ATLTRACE(_T("~CThreadObjSink: 0x%x\n"), &m_shTimer);
				m_shTimer.UnhookEvent(evtOnTimer, this, &CThreadObjSink::OnFetchDelayTimer);
				m_shWinMsg.UnhookEvent(evtOnAssociateMsg, this, &CThreadObjSink::OnNewMsg);

				SAFEFREENEW(m_queueC2W);
				SAFEFREENEW(m_queueW2C);
			}

			virtual void OnW2CMsgCome(INT32& cmd, w2c_t& msg) = 0;		//实际为WM_MSG+定时器结合实现
			virtual void OnWorkFinish()							//仅仅表示Attach的工作线程正常完成其工作（一般来说，不是因为CheckExit_InWorkThread触发的退出）
			{
				PostExitMsg();
			}
			UINT64	GetTaskID(){ return m_u64TaskID; }

		protected:
			//希望文雅的终止的情况下
			virtual BOOL	PostExitMsg()						//使工作线程收到消息后酌情主动退出
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
				*pHwnd = m_shWinMsg.GetHwnd(); //默认会调用m_shWinMsg的Enable
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
			Concurrency::concurrent_queue<queueitem_t<c2w_t> >*		m_queueC2W;	//本类创建和销毁
			Concurrency::concurrent_queue<queueitem_t<w2c_t> >*		m_queueW2C;	//本类创建和销毁
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

				//tbd追踪，保持消费者速度最优化
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

				//还有数据没取完？继续下次
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
			virtual ~IThreadObjWork(){}//必须为virtual
			
			virtual void Run_InWorkThread(void* pParam) = 0;	//注意因为需要考虑到尽量主动退出，所以在时间上需要留意Sleep间隔不能大于m_u32WaitSleep/2
			virtual BOOL SetSinkMeta(HWND Hwnd, UINT32 MsgID, UINT32 WaitSleep, void* pC2W, void* pW2C) = 0;
			virtual BOOL NotifyThreadFinish_InWorkThread() = 0;
			virtual BOOL CheckExit_InWorkThread() = 0;
			virtual BOOL FilterExit_InWorkThread() = 0;
			virtual void SetTaskID(UINT64 u64TaskID) = 0;

		};

		//基本工作于工作线程，构造（初始化）和析构（反初始化）函数工作于主线程
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
			virtual ~CThreadObjWork()	//必须为virtual
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
			//执行线程最好在在一个额定时间检查这个函数，以避免强制终止线程，返回TRUE表示外部希望其退出
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
			BOOL NotifyThreadFinish_InWorkThread()//不通过消息而是使用queue是因为希望队列能有序处理
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
			Concurrency::concurrent_queue<queueitem_t<c2w_t> >*		m_queueC2W;	//本类仅仅引用
			Concurrency::concurrent_queue<queueitem_t<w2c_t> >*		m_queueW2C;	//本类仅仅引用
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

				//如果已经确认退出，那么最多只能发系统消息；收消息
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

				//tbd追踪，保持消费者速度最优化

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
			TSC_CORE = 1,	//核心业务模块
			//文件传输类型
			TSC_FILETRANS_NORMAL ,  // 正常文件槽
			TSC_FILETRANS_PIC,             //图片槽
			TSC_FILETRANS_RECORD,   //录音文件槽
			TSC_FILETRANS_NET_DETECTION,    //网络探测的
			TSC_FILETRANS_DOWN_FILE,         //down file  max 5
			TSC_FILETRANS_UPLOAD_LOG,		 //日志文件上传 MAX 5
			TSC_NETBROKEN_CHECK,	//网络监测
			TSC_ALIOSSKEY,    //获取阿里key的线程槽 且必须 有且仅有一个在Run

			TSC_HTTPS_REQUEST,	//基于第三方CURL库的HTTPS请求处理线程
			TSC_DOMAIN_DETECT,	//探测解析域名

			TSC_EMAILPLANE,        //邮件计划
			TSC_EXTRACTZIP,        //CEF 解压资源
		};
		COMMON_API void		SetThreadNum(ThreadSlotClass cls, UINT8 high);	//默认每个类型会有THREADNUM_DEFAULT个线程
		COMMON_API UINT64	AddTask(ThreadSlotClass cls, IThreadObjSink* pISink, IThreadObjWork* pIWork, void* pThreadParam);	//注意pParam生命期，如果是new出来指针，建议是作为sink的一个成员变量，比如在析构时候释放,返回0表示失败
		COMMON_API void		DelTask(UINT64 id);	
	}
}

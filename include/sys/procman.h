#ifndef _PROCMAN_H_
#define _PROCMAN_H_

#include "plat_export.h"
#include "sys/blog.h"
#include <time.h>
#include "utl/sharememorypool.h"
#include "event/csea.h"
#ifdef WIN32
#include <windows.h>
//#include <afxwin.h>
//#include <afxsock.h>
#endif

#define	PROC_SHM_KEY			2010050201		//PROC共享内存标识
#define PROC_MAX_NUM			16
#define PROC_MAX_DEAD_TIME		5				//进程dead后，超过此时间即将进程kill并情况标志

//#define PROC_SHM_DIR		"dat/map/sys"		//共享内存相对目录
//#define PROC_SHM_FILE		"sys.dat"			//共享内存映射文件

#if defined(__unix)
#define	END_PROC_SIGNAL		SIGTERM
#elif defined(WIN32)
#define END_PROC_SIGNAL		WM_CLOSE
#endif

#define LOG_PROC_NORMAL			100
#define LOG_PROC_START_CHECK	101
#define LOG_PROC_RUN_CHECK		102
#define LOG_PROC_KILL			103

//进程状态
enum ProcStatus 
{
	PROCESS_START =	0,		//启动
	PROCESS_STOP,			//正常退出
	PROCESS_ERROR,			//异常退出
	PROCESS_NOTEXIST,		//不存在
	PROCTIME = 4
};
//////////////////////////////////////////////////////////////////////////
//进程启动方式
enum ProcStartMode
{
	NO_START = 0,			//不启动
	DAEMON_START,			//守护
	TIME_START,				//定时启动
	CYCLE_START				//周期启动
};

//进程编号
enum ProcHandle
{
	PROCHANDLE_PLATSRV	= 0,
	PROCHANDLE_DACSRV		= 10001,
	PROCHANDLE_SCDSRV		= 10002,
	PROCHANDLE_EVTSRV		= 10003,
	PROCHANDLE_UIBSRV		= 10004,
	PROCHANDLE_RTWSRV		= 10005,
	PROCHANDLE_IEDCLISRV	= 10006,
	PROCHANDLE_IEDSRV		= 10007,
	PROCHANDLE_PASSRV		= 10008,
	PROCHANDLE_BESSRV		= 10009,
	PROCHANDLE_ZFSRV		= 10010,
	PROCHANDLE_DAPSRV		= 10011,
	PROCHANDLE_GATESRV		= 10012
};

typedef struct _proc_check_info_
{
	hUInt32		pid;
	hUChar		flag;		//有效标志
	bool		isIniting;	//是否正在初始化(wfp add)
	hUInt32 	startTime;
	hUInt32		updateTime;	//进程更新时间	
	char		name[SYS_PROCNAME_LEN];
	char		desc[SYS_DESC_LEN];
}proc_check_info;

typedef struct _proc_mem_
{
	proc_check_info checkInfo[PROC_MAX_NUM];
	proc_check_info eisd;
}proc_mem;

namespace PDT
{

	//得到进程ID
	class PLAT_EXPORT CProcessID
	{
	private:
#if defined(WIN32)
		static HWND				m_mainHwnd;
#elif defined(__unix)
		static int				m_mainId;
#endif
	public:
		CProcessID();
		~CProcessID(){}

#if defined(WIN32)
		void	setProcId(HWND hwnd)	{m_mainHwnd = hwnd;}
		hInt64	procId()		{return	(hInt64)m_mainHwnd;}
#elif defined(__unix)
		void	setProcId(int pid)	{m_mainId = pid;}
		int		procId()		{return	m_mainId;}
#endif
	};

	//进程管理类
	class PLAT_EXPORT CProcessManage : public CShareMemoryBase
	{
	private:
		static	int				m_order;		//当前进程的序号
		static	proc_mem		*m_ptr;
		proc_check_info			*m_pProcInfoVec;
		proc_check_info			*m_pEisdInfo;
		CSysEvent				m_event;
	public:
		CProcessManage();
		virtual	~CProcessManage();
		int				createProcEvent(const char *name,const char *desc,unsigned char state);

		virtual	void	sendSignal(const char *,int);
		virtual	void	startProc(const char *name,const char *desc,
#if defined(WIN32)
			HWND hwnd=NULL,
#endif
			int eisdflag=FALSE);

		//wfp add for load long time
		virtual void	beginInitProc();
		virtual void	endInitProc();

		virtual	void	endProc(int eisdflag=FALSE);

		//return TRUE or FALSE;flag=isEisdProcess
		virtual	int		isProcExist(const char *name,int flag=FALSE);	
		virtual	void	killAll();
		virtual	void	checkProcStart();
		virtual	void	checkProcState();

		//return TRUE or FALSE
		virtual	int		checkEisd();
		void			execProc(const char *name,const char *desc=NULL,const char *para1=NULL,const char *para2=NULL,const char *para3=NULL,const char *para4=NULL,const char *para5=NULL);
		virtual	void	clear()
		{
			killAll();
		}

		//外部程序调用:重启由eisd守护的所有进程--wfp add at 20110915
		virtual void	restartSystem();
		//eisd调用
		virtual bool	isRestartSystem();
		//eisd调用
		virtual void	endRestartSystem();
	};

}//end namespace


#endif //_PROCMAN_H_

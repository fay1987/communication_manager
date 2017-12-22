#include "sys/procman.h"
#include "sys/plat_api.h"
#include "event/type.h"

using namespace PDT;

#if defined(_PDT_PLAT_)
extern CLogInfo	g_logInfo;
#endif

int CProcessManage::m_order = -1;
proc_mem* CProcessManage::m_ptr = NULL;
#define PROC_SHM_NAME	"PROCSHM"


#if defined(WIN32)
HWND CProcessID::m_mainHwnd;
#elif defined(__unix)
int	 CProcessID::m_mainId;
#endif
CProcessID::CProcessID()
{
#if defined(__unix)
	if (m_mainId <= 0) m_mainId = getpid();
#elif defined(WIN32)
	if (m_mainHwnd <= 0) m_mainHwnd = 0;//*AfxGetMainWnd(); //wfp changed
#endif
}

CProcessManage::CProcessManage()
{
	//QT共享内存方式
	//m_ptr = NULL;
	//m_shareMem.setKey(PROC_SHM_KEY);

	//if( !m_shareMem.attach() )
	//{
	//	m_shareMem.create(sizeof(proc_mem));
	//	m_ptr = (proc_mem*)m_shareMem.data();
	//	if ( m_ptr != NULL )  memset(m_ptr,0,sizeof(proc_mem));
	//}

	//m_ptr = (proc_mem*)m_shareMem.data();

	//if(m_ptr == NULL)
	//{
	//	qDebug("CREATE SHARE MEMORY ERROR!/n");
	//	exit(0);
	//}

	if ( m_ptr == NULL )
	{
		bool isNetCreated = false;
		createAndMapShareMemory(PROC_SHM_KEY,sizeof(proc_mem),isNetCreated);
		m_ptr = (proc_mem*)shmPointer();
		if ( m_ptr == NULL )
		{
			perror("create procman memory error\n");
			exit(0);
		}
		if ( isNetCreated ) ACE_OS::memset(m_ptr,0,sizeof(proc_mem));
	}
	
	m_pProcInfoVec = m_ptr->checkInfo;
	m_pEisdInfo = &(m_ptr->eisd);
}

CProcessManage::~CProcessManage()
{
}

int	CProcessManage::createProcEvent(const char *name,const char *desc,unsigned char state)
{

	nSysEvent	sysEvent;
	ACE_OS::memset(&sysEvent,0,sizeof(nSysEvent));
	sysEvent.m_eventSort = EVENT_SORT_RUN;
	sysEvent.m_eventType = EVENT_RUN_PROC;

	ACE_OS::strsncpy(sysEvent.m_parentCode,name,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_code,name,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_name,desc,SHORTNAME_LEN);

	CDateTime dateTime = CDateTime::currentDateTime();
	sysEvent.m_ymd = dateTime.year()*100*100 + dateTime.month()*100 + dateTime.day();
	sysEvent.m_hmsms= dateTime.hour()*100*100*1000 + dateTime.minute()*100*1000 + dateTime.second()*1000 + dateTime.microsec()/1000;

	sysEvent.m_isRevert	= false;
	sysEvent.m_security = 1;
	sysEvent.PrintOut = 1;
	sysEvent.m_stateValue = state;


	CString stateDesc;
	switch (state)
	{
	case PROCESS_START:
		stateDesc = "启动";
		sysEvent.m_isRevert	= true;
		break;
	case PROCESS_STOP:
		stateDesc = "正常退出";
		break;
	case PROCESS_ERROR:
		stateDesc = "异常退出";
		break;
	case PROCESS_NOTEXIST:
		stateDesc = "不存在";
		break;
	default:
		stateDesc = "无效";
		break;
	}

	ACE_OS::sprintf(sysEvent.m_charDesc,"%s %s %s",desc,name,stateDesc.c_str());

	return (bool)m_event.insertEvent(sysEvent);
}

//发送信号给所有的子进程
void CProcessManage::sendSignal(const char *name,int sig)
{
#if  defined(WIN32)
	DWORD lp;
#endif

	for(int i=0;i<PROC_MAX_NUM;i++)
	{
		if ( m_pProcInfoVec[i].flag == FALSE ) continue;
		if ( m_pProcInfoVec[i].pid <= 0 ) continue;
		if ( strcmp(m_pProcInfoVec[i].name,name) != 0 ) continue;
#if defined(__unix)
		kill(m_pProcInfoVec[i].pid,sig);
#elif defined(WIN32)
		SendMessageTimeout((HWND)(int)m_pProcInfoVec[i].pid,sig,0,0,SMTO_ABORTIFHUNG,5000000,&lp);
#endif
		if(sig == END_PROC_SIGNAL)
		{
#ifdef WIN32
			killProc(name);
#endif
			//执行下面几行是为了防止应用程序退出时未执行end_proc时产生异常
			if(m_pProcInfoVec[i].flag==TRUE && m_pProcInfoVec[i].pid>0 && strcmp(m_pProcInfoVec[i].name,name)==0)
			{
				m_pProcInfoVec[i].pid = 0;
				m_pProcInfoVec[i].flag = FALSE;
				m_pProcInfoVec[i].isIniting = false;
				m_pProcInfoVec[i].name[0] = 0;
				m_pProcInfoVec[i].desc[0] = 0;
			}
		}
	}
}

void CProcessManage::startProc(const char *name,const char *desc,
#if defined(WIN32)
							   HWND hwnd,
#endif
							   int eisdflag)
{
	CProcessID processid;
	int i;
	time_t cur_time;
	time(&cur_time);
	if(eisdflag == FALSE)	//normal start_proc()
	{
		//printf("Start Process: %s %s pid:%d \n",name,desc,processid.get_proc_id());
		for(i=0;i<PROC_MAX_NUM;i++)
		{
			//unix下如有此进程号则对此进程号所在记录重新设置
#if defined(WIN32)
			if(m_pProcInfoVec[i].flag == TRUE)	continue;
			if(hwnd!=NULL)	processid.setProcId(hwnd);
			m_pProcInfoVec[i].pid = processid.procId();
			logprint(LOG_PROC_NORMAL,"start_proc________i = %d , name = %s , pid = %d\n",i,name,m_pProcInfoVec[i].pid);
#elif defined(__unix)
			int tmppid = processid.procId();
			logprint(LOG_PROC_NORMAL,"start_proc________name = %s , m_pProcInfoVec[%d]->pid = %d, this process pid = %d \n",
				name,i,(int)m_pProcInfoVec[i].pid,getpid());
			if(m_pProcInfoVec[i].pid != tmppid)	continue;
#endif
			//logprint(LOG_PROC_NORMAL,"process id already exist, update process infomation , m_pProcInfoVec[%d].name = %s , pid = %d\n",i, name,(int)m_pProcInfoVec[i].pid);
			m_pProcInfoVec[i].flag = TRUE;
			m_pProcInfoVec[i].isIniting = false;
			ACE_OS::strncpy(m_pProcInfoVec[i].name,name,SYS_PROCNAME_LEN);
			ACE_OS::strncpy(m_pProcInfoVec[i].desc,desc,SYS_DESC_LEN);
			m_pProcInfoVec[i].startTime = (hUInt32)cur_time;
			m_pProcInfoVec[i].updateTime = (hUInt32)cur_time;
			createProcEvent(name,desc,PROCESS_START);
			m_order = i;
			return;
		}
#if defined (__unix)
		//unix下如没有此进程号则新添一项
		for(i=0;i<PROC_MAX_NUM;i++)
		{
			if(m_pProcInfoVec[i].flag == TRUE)	continue;
			m_pProcInfoVec[i].flag = TRUE;
			m_pProcInfoVec[i].isIniting = false;	
			processid.setProcId(getpid());
			m_pProcInfoVec[i].pid = processid.procId();
			ACE_OS::strncpy(m_pProcInfoVec[i].name,name,SYS_PROCNAME_LEN);
			ACE_OS::strncpy(m_pProcInfoVec[i].desc,desc,SYS_DESC_LEN);
			m_pProcInfoVec[i].startTime = cur_time;
			m_pProcInfoVec[i].updateTime = cur_time;
			createProcEvent(name,desc,PROCESS_START);
			m_order = i;
			return;
		}
#endif
	}
	else	//eisd	start_proc()
	{
#if defined(WIN32)
		if(hwnd!=NULL)	processid.setProcId(hwnd);
#else
		processid.setProcId(getpid());
#endif
		m_pEisdInfo->pid = processid.procId();
		m_pEisdInfo->flag = TRUE;
		m_pEisdInfo->isIniting = false;
		ACE_OS::strncpy(m_pEisdInfo->name,name,SYS_PROCNAME_LEN);
		ACE_OS::strncpy(m_pEisdInfo->desc,desc,SYS_DESC_LEN);
		m_pEisdInfo->startTime = (hUInt32)cur_time;
		m_pEisdInfo->updateTime = (hUInt32)cur_time;
	}
}

void CProcessManage::beginInitProc()
{
	//普通进程
	CProcessID processid;
	int pid = processid.procId();
	logprint(LOG_PROC_NORMAL,"begin_init_proc________pid = %d\n",pid);

	if ( m_order < 0 || m_order >= PROC_MAX_NUM ) return ;

	m_pProcInfoVec[m_order].isIniting = true;
}

void CProcessManage::endInitProc()
{
	//普通进程
	CProcessID processid;
	int pid = processid.procId();
	logprint(LOG_PROC_NORMAL,"end_init_proc________pid = %d\n",pid);

	if ( m_order < 0 || m_order >= PROC_MAX_NUM ) return ;

	//更新注册当前进程
	m_pProcInfoVec[m_order].updateTime = (hUInt32)time(NULL);

	m_pProcInfoVec[m_order].isIniting = false;
}

void CProcessManage::endProc(int eisdflag/*=FALSE*/)
{
	//eisd进程
	if ( eisdflag )
	{
		m_pEisdInfo->pid = 0;
		m_pEisdInfo->flag = FALSE;
		m_pEisdInfo->isIniting = false;
		m_pEisdInfo->name[0] = 0;
		m_pEisdInfo->desc[0] = 0;
		return;
	}
	//普通进程
	CProcessID processid;
	int pid = processid.procId();
	logprint(LOG_PROC_NORMAL,"end_proc________pid = %d\n",pid);
	if ( pid == 0 ) return;
	for(int i=0;i<PROC_MAX_NUM;i++)
	{
		if(m_pProcInfoVec[i].flag == FALSE)	continue;
		if(m_pProcInfoVec[i].pid != pid) continue;
		createProcEvent(m_pProcInfoVec[i].name,m_pProcInfoVec[i].desc,PROCESS_STOP);
		m_pProcInfoVec[i].pid = 0;
		m_pProcInfoVec[i].flag = FALSE;
		m_pProcInfoVec[i].isIniting = false;
		m_pProcInfoVec[i].name[0] = 0;
		m_pProcInfoVec[i].desc[0] = 0;
		m_order = -1;
		break;
	}
}

int CProcessManage::isProcExist(const char *name,int flag)
{
	if(flag == FALSE)
	{
		for(int i=0;i<PROC_MAX_NUM;i++)
		{
			if(m_pProcInfoVec[i].flag == FALSE)	continue;
			if(strcmp(name,m_pProcInfoVec[i].name) != 0)	continue;
			return	TRUE;
		}
	}
	else
	{
		if(m_pEisdInfo->flag==TRUE && strcmp(name,m_pEisdInfo->name)==0)
			return	TRUE;
	}
	return	FALSE;
}

void CProcessManage::killAll()
{
	for (int i=0;i<PROC_MAX_NUM;i++)
	{
		if ( m_pProcInfoVec[i].flag == FALSE ) continue;

		sendSignal(m_pProcInfoVec[i].name,END_PROC_SIGNAL);
	}

}

void CProcessManage::checkProcStart()
{
}

void CProcessManage::checkProcState()
{
	//更新eisd的时间
	time_t curTime = time(NULL);
	m_pEisdInfo->updateTime = (hUInt32)curTime;

	//check 注册进程
	for(int i=0;i<PROC_MAX_NUM;i++)
	{
		if(m_pProcInfoVec[i].flag == FALSE)	continue;
		if( m_pProcInfoVec[i].isIniting ) continue;		//wfp add

		if(m_pProcInfoVec[i].pid==0)
		{
			m_pProcInfoVec[i].flag = FALSE;
			m_pProcInfoVec[i].isIniting = false;
			m_pProcInfoVec[i].name[0] = 0;
			m_pProcInfoVec[i].desc[0] = 0;
		}
		else
		{
#if defined (__unix)
			if( kill(m_pProcInfoVec[i].pid,0)<0 )	//进程不存在
			{
				createProcEvent(m_pProcInfoVec[i].name,m_pProcInfoVec[i].desc,PROCESS_ERROR);
				m_pProcInfoVec[i].flag = FALSE;
				m_pProcInfoVec[i].isIniting = false;
				m_pProcInfoVec[i].pid = 0;
				m_pProcInfoVec[i].desc[0] = 0;
				m_pProcInfoVec[i].name[0] = 0;
				continue;
			}
#endif
			if ( ((hUInt32)curTime - m_pProcInfoVec[i].updateTime) >= 3*PROC_MAX_DEAD_TIME )
			{
				logprint(LOG_PROC_KILL,"proc <%s> time out________pid = %d\n",m_pProcInfoVec[i].name,(int)m_pProcInfoVec[i].pid);
				if( isProcExist(m_pProcInfoVec[i].name) )
				{
					createProcEvent(m_pProcInfoVec[i].name,m_pProcInfoVec[i].desc,PROCESS_ERROR);//保存进程异常事件(非底层处理)
					sendSignal(m_pProcInfoVec[i].name,END_PROC_SIGNAL);
				}
				m_pProcInfoVec[i].flag = FALSE;
				m_pProcInfoVec[i].isIniting = false;
				m_pProcInfoVec[i].pid = 0;
				m_pProcInfoVec[i].desc[0] = 0;
				m_pProcInfoVec[i].name[0] = 0;
			}
		}
	}//end for
}

int CProcessManage::checkEisd()
{
	if(m_pEisdInfo->flag == FALSE)	return	FALSE;

	time_t curTime = time(NULL);

	if ( ((hUInt32)curTime - m_pEisdInfo->updateTime) >= 2*PROC_MAX_DEAD_TIME )
		return FALSE;
	else	
	{
		if ( m_order < 0 || m_order >=PROC_MAX_NUM ) return FALSE;

		//更新注册当前进程
		m_pProcInfoVec[m_order].updateTime = (hUInt32)curTime;
		logprint(LOG_PROC_NORMAL,"进程名 = %s , time = %d", m_pProcInfoVec[m_order].name,(int)curTime);
	}

	return	TRUE;
}	

//启动一个进程
void CProcessManage::execProc(const char *name,const char *desc,const char *para1,const char *para2,const char *para3,const char *para4,const char *para5)
{
	CProcessID processid;
	if(strlen(name)==0 || strlen(name)>PATH_LEN+SYS_PROCNAME_LEN)	return;
	char cmd_name[512],rootpath[PATH_LEN],procname[SYS_PROCNAME_LEN];
#if defined(WIN32)
	if(isalpha(name[0])!=0 && name[1]==':' && name[2]=='\\')
	{
		strcpy(cmd_name,name);
		strcpy(procname,strrchr(name,'\\')+1);
	}
	else
	{
		strcpy( rootpath,ACE_OS::getenv(SYS_ENV_VAR) );
		sprintf(cmd_name,"%s\\bin\\%s",rootpath,name);
		strcpy(procname,name);
	}
	printf("%s\n",cmd_name);
	if(strstr(cmd_name,".") == NULL)	strcat(cmd_name,".exe");
	FILE *fp = fopen(cmd_name,"rb");
	if(fp == NULL)
	{
		return;
	}
	fclose(fp);
	if(para1 && strlen(para1)>0 && strlen(para1)<=SYS_PROCPARA_LEN)
	{
		strcat(cmd_name," ");
		strcat(cmd_name,para1);
	}
	if(para2 && strlen(para2)>0 && strlen(para2)<=SYS_PROCPARA_LEN)
	{
		strcat(cmd_name," ");
		strcat(cmd_name,para2);
	}
	if(para3 && strlen(para3)>0 && strlen(para3)<=SYS_PROCPARA_LEN)
	{
		strcat(cmd_name," ");
		strcat(cmd_name,para3);
	}
	if(para4 && strlen(para4)>0 && strlen(para4)<=SYS_PROCPARA_LEN)
	{
		strcat(cmd_name," ");
		strcat(cmd_name,para4);
	}
	if(para5 && strlen(para5)>0 && strlen(para5)<=SYS_PROCPARA_LEN)
	{
		strcat(cmd_name," ");
		strcat(cmd_name,para5);
	}
	STARTUPINFO si;
	memset(&si,0,sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	CreateProcess(NULL,cmd_name,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
#elif defined(__unix)
	if(name[0] == '/')
	{
		strcpy(cmd_name,name);
		strcpy(procname,strrchr(name,'/')+1);
	}
	else
	{
		strcpy( rootpath,ACE_OS::getenv(SYS_ENV_VAR) );
		sprintf(cmd_name,"%s/bin/%s",rootpath,name);
		strcpy(procname,name);
	}
	FILE *fp = fopen(cmd_name,"rb");
	if(fp == NULL)
	{
		return;
	}
	fclose(fp);
	int i;
	for(i=0;i<PROC_MAX_NUM;i++)
	{
		if(m_pProcInfoVec[i].flag == FALSE)	break;
	}
	if(i >= PROC_MAX_NUM)	return;
	int pid = fork();
	if(pid == 0)
	{
		sigrelse(SIGALRM);
		processid.setProcId(getpid());
#if (defined(__alpha )|| defined(_AIX))
		putenv("LANG=zh_CN");
#elif defined(__sun)
		putenv("LANG=zh");
#endif
		putenv("DISPLAY=:0.0");
		execl(cmd_name,para1,para2,para3,para4,para5,NULL);
	}
#endif
}

void CProcessManage::restartSystem()
{
	//设置为重启状态
	m_pEisdInfo->flag = FALSE;
}

bool CProcessManage::isRestartSystem()
{
	if ( m_pEisdInfo->flag == TRUE ) return false;
	return true;
}

void CProcessManage::endRestartSystem()
{
	m_pEisdInfo->flag = TRUE;
}

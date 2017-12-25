#include "sys/plat_api.h"

#if defined(WIN32)
#include "winprocess.h"
#elif	defined(__sun)
#include <sys/old_procfs.h>
#include <sys/termios.h>	/* TIOCNOTTY */
#include <dirent.h>
#elif defined(_AIX)
#include <sys/procfs.h>
#elif defined(__hpux)
#include <sys/termios.h>
#elif defined(__linux)
#include <sys/termios.h>	/* TIOCNOTTY */
#include <sys/procfs.h>
#include <dirent.h>
#endif


int killProc(int pid)
{
	if(pid <= 0)	return FALSE;
#if defined(__unix)
	if(kill(pid,SIGKILL) == 0)	return TRUE;
	return FALSE;
#elif defined(WIN32)
	return WinKillProc(pid);
#endif
}

int killProc(const char *name)
{
	if(name==NULL || strlen(name)==0)	return FALSE;
#if defined(__unix)
	return	FALSE;
#elif defined(WIN32)
	return WinKillProc(name);
#endif
}

int getProcName(int pid,char *procname,int flag)
{
	if(procname == NULL)	return	FALSE;
#if defined(__unix)
#if	defined(__linux)
	//sprintf(procname,"%d",pid);	//wfp masked at 2009-12-28

	#define READ_BUF_SIZE 50
	char buffer[READ_BUF_SIZE];
	char name[READ_BUF_SIZE];
	char filename[READ_BUF_SIZE];
	FILE *status;
	sprintf(filename, "/proc/%d/status",pid);
	if (! (status = fopen(filename, "r")) ) 
		return FALSE;

	/* Read first line in /proc/?pid?/status */
	if (fgets(buffer, READ_BUF_SIZE-1, status) == NULL) 
	{
		fclose(status);
		return FALSE;
	}
	fclose(status);

	/* Buffer should contain a string like "Name:   binary_name" */
	sscanf(buffer, "%*s %s", name);
	strcpy(procname,name);
#elif defined(__hpux)      //wfp add 
	struct pst_status pbuf;
	memset(&pbuf, 0, sizeof(pbuf));
	pstat_getproc(&pbuf, sizeof(pbuf), 0, getpid());
	strcpy(procname, pbuf.pst_ucomm);  //wfp add end
#else
	char ProcImage[16];
#if defined (_AIX)
	sprintf(ProcImage,"/proc/%d/psinfo",pid);
#else
	sprintf(ProcImage,"/proc/%05d",pid);
#endif
	int fd = open(ProcImage,O_RDONLY);
	if (fd == -1)	return FALSE;
#if defined (_AIX)
	psinfo_t procinfo;
	int rc = read(fd, &procinfo, sizeof(psinfo_t));
	if (rc !=  sizeof(psinfo_t))
		printf("cannot read proc info from psinfo\n");
#else
	prpsinfo_t procinfo;
	ioctl(fd,PIOCPSINFO,&procinfo);
#endif
	close(fd);
	strcpy(procname,procinfo.pr_fname);
#endif
	return TRUE;
#elif defined(WIN32)
	return WinGetProcName(pid,procname,flag);
#endif
}

//参见9100/utl_prc.c:UTL_FindProcByName()函数
/*
 * 函数功能：判断进程是否存在
 * 函数输入：name--进程名；isExcludeSelf--是否排除当前进程
 * 函数返回：-1--失败;0/1--成功
 */
int findProcByName(const char *name,bool isExcludeSelf/*=false*/)
{
#if defined(WIN32)
	return WinFindProc(name,isExcludeSelf);
#else //unix
	DIR  *dir;
	char processName[32] = "";
	int pid = -1;

#if !defined(__hpux) && !defined(__aix)
	struct dirent *dir_content;
	prpsinfo_t  psinfo;

	const int procPidLen = 5;
	dir = opendir("/proc");

	while(1) 
	{
		dir_content = readdir(dir);
		if(dir_content == NULL)
			break;

#if defined (__linux)
		//if( dir_content->d_reclen != procPidLen )
		//	continue;
#elif !defined(__sun)
		if( dir_content->d_namlen != procPidLen )
			continue;
#endif

		pid = atoi(dir_content->d_name);
		if (pid < 0) continue;

		getProcName(pid,processName);
		if ( strcmp(name,processName) == 0 ) 
		{
			if ( isExcludeSelf && (getpid() ==pid) ) continue;	//wfp add
			closedir(dir);
			return 0;
		}
	}//end while

	closedir(dir);
	return -1;
#endif
#endif//end unix
}

#ifdef	__unix

/*+++++++++++++++++++++++++ FUNCTION DESCRIPTION ++++++++++++++++++++++++++++++
*
* NAME	       :  procToDaemon FOR UNIX
*
* DESCRIPTION :  Start a process as a daemon. This must be the first call
*		  for any daemon processes. After this call, DAE specific
*		  initialization routines can be called. This procedure does
*		  not support ERR error logging and will use the standard
*		  I/O to log any anomalies.
*
*		  Reference: Unix Network Programming by Richard Stevens, p.72.
*
* COMPLETION
* STATUS      :  This is a void procedure.
*
*-----------------------------------------------------------------------------
*/
int	procToDaemon()
{
	/* If started by init (process 1) from the /etc/inittab file, there is no */
	/* need to fork. Simply detach from the terminal. Since the processing is */
	/* the same as EPM childs, simply set define the "EPM Child" environment  */
	/* variable and continue processing as usual.			     */
	/*									        */
	/* NOTE: This test is unreliable due to an unavoidable ambiguity if the   */
	/*	    process is started by some other process and orphaned (i.e., if   */
	/*	    the parent process terminates before we are started). ----------  */
	if(getppid() != 1)
	{
		printf("Parent Process ID = %d\n",getpid());
		sigset(SIGTSTP,SIG_IGN);
		sigset(SIGTTIN,SIG_IGN);
		sigset(SIGTTOU,SIG_IGN);

		/* If the process was not started in background, fork and let the parent  */
		/* exit. This also guarantees the first child is not a process group	    */
		/* leader. -------------------------------------------------------------- */
		pid_t	pid;
		pid = fork();
		if(pid > 0)	exit(0);     /* Terminate the parent process */
		if(pid == -1)
		{
			printf("Process is running in the terminal......\n");
			return	FALSE;
		}

		/* ====================================================================== */
		/* First child processing continues here				     */
		/* ====================================================================== */

		/* Disassociate from controlling the terminal and process group. Ensure   */
		/* the process can't reacquire a new controlling terminal. -------------- */

		/* SIGTSTP is defined for BSD like systems, in which case setpgid and     */
		/* ioctl are used to start as a daemon.  On HP-UX, TIOCNOTTY is not	     */
		/* defined, so we use the standard System V procedure on HP-UX.	     */
		setpgid(0,getpid());
#ifdef	__sun
		setpgrp();
#else
		setsid();
#endif

#ifndef __hpux  //wfp add
		int	fd = open("/dev/tty",O_RDWR);
		if(fd > 0)
		{
			ioctl(fd,TIOCNOTTY,(char *)NULL);
			close(fd);
		}
#endif
		sigset(SIGHUP,SIG_IGN);
		char	rundir[PATH_LEN];

		char* root = ACE_OS::getenv(SYS_ENV_VAR);
		if ( root == NULL ) chdir("/");
		else
		{
			strcat(rundir,"/bin");
			chdir(rundir);
		}
		printf("Process is running in the background......\n");
		printf("Child Process ID = %d\n",getpid());
	}
	umask(0);
	return	TRUE;
}

void kill_defunct_child_proc()
{
	sighold(SIGCHLD);
	int i,temp,pid;
	for(i=0;i<200;i++)
	{
		pid = waitpid(-1,&temp,WNOHANG);
		if(pid <= 0)	break;
		kill(pid,SIGKILL);
	}
	sigrelse(SIGCHLD);
}

//线程栈大小设置
void setThreadStackSize(pthread_attr_t *pattr,int stacksize/*=BF_THREAD_STACK_SIZE*/)
{	
	if (pattr == NULL)  return;

#ifdef __hpux
	size_t localsize = 0;
	pthread_attr_getstacksize(pattr,&localsize);
	if (localsize < stacksize)
	{
		pthread_attr_setstacksize(pattr,stacksize);//for hpux' default size of stack is less than 1M
	}
#endif
}

#endif //__unix

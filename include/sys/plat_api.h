#ifndef _PDT_PLAT_API_H_
#define _PDT_PLAT_API_H_

#include "plat_export.h"
#include "sys/sysmacro.h"
#include <ace/OS.h>

//进程控制
PLAT_EXPORT	int		killProc(int pid);
PLAT_EXPORT	int		killProc(const char *name);
//flag：TRUE——包含路径，FALSE——不包含路径
PLAT_EXPORT	int		getProcName(int pid,char *procname,int flag=false);
PLAT_EXPORT int		findProcByName(const char* name,bool isExcludeSelf=false);

#ifdef	__unix
//将进程放入后台运行
int							procToDaemon();
//处理已经死去的子进程
void						kill_defunct_child_proc();
//线程栈大小设置
void 						setThreadStackSize(pthread_attr_t *pattr,int stacksize=SYS_THREAD_STACK_SIZE);
#endif //unix


#endif //_PDT_PLAT_API_H_

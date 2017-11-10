#ifndef _PDT_PLAT_API_H_
#define _PDT_PLAT_API_H_

#include "plat_export.h"
#include "sys/sysmacro.h"
#include <ace/OS.h>

//���̿���
PLAT_EXPORT	int		killProc(int pid);
PLAT_EXPORT	int		killProc(const char *name);
//flag��TRUE��������·����FALSE����������·��
PLAT_EXPORT	int		getProcName(int pid,char *procname,int flag=false);
PLAT_EXPORT int		findProcByName(const char* name,bool isExcludeSelf=false);

#ifdef	__unix
//�����̷����̨����
int							procToDaemon();
//�����Ѿ���ȥ���ӽ���
void						kill_defunct_child_proc();
//�߳�ջ��С����
void 						setThreadStackSize(pthread_attr_t *pattr,int stacksize=SYS_THREAD_STACK_SIZE);
#endif //unix


#endif //_PDT_PLAT_API_H_

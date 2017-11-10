#ifndef __IEC61850_IEDSHM_H__
#define __IEC61850_IEDSHM_H__

#include "sysincs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#else
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#ifndef TRUE 
#define TRUE	1
#endif
	
#ifndef FALSE	
#define FALSE	0
#endif
	
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

extern void*	shmemory_ptr;
#if defined(WIN32)
extern HANDLE	shmID;
extern int		create_flag;
#endif

void*	GetShmPtr();
int		CheckShareMemory(int shmkey);
int		CreateShareMemory(int shmkey,int len);
void*	MapShareMemory(int shmkey);
int		UnMapShareMemory();
int		FreeShareMemory(int shmkey);
void*	CreateAndMapShareMemory(int shmkey,int len,int *flag);

#ifdef __cplusplus
}
#endif	//__cplusplus

#endif	//__IEC61850_IEDSHM_H__
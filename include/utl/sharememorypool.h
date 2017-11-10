/*==============================================================================
* �ļ����� : sharememorypool.h
* �ļ����� ��ACE�����ڴ����(�����map�ļ�)
* ģ�� : �����ڴ�
* ����ʱ�� : 2009-06-08
* ���� : ����ƽ
* �汾 : v1.0

* �޸ĺ�汾	: 1.1
* �޸�ժҪ		: 
* �޸���		:  
* �޸�����		: 
* ʹ��˵��		��map�ļ�����ʱ��������ĳ���̶߳�ռ�����ڲ����˽�������
				  û��ʹ���߳�������˱����ڽ�������ʱ����map�ļ�
==============================================================================*/
#ifndef _BFS_UTL_SHARE_MEMORY_POOL_H_
#define _BFS_UTL_SHARE_MEMORY_POOL_H_

#include "utl/utl_export.h"
#include <ace/Malloc_T.h>
#include <ace/PI_Malloc.h>	
#include <ace/MMAP_Memory_Pool.h>
#include <ace/Process_Mutex.h>

class	UTL_EXPORT CShareMemoryPool
{
private:
	typedef	ACE_Malloc_T<ACE_MMAP_MEMORY_POOL,ACE_SYNCH_PROCESS_MUTEX,ACE_PI_Control_Block>	Malloc;
	//typedef	ACE_Malloc_T<ACE_LITE_MMAP_MEMORY_POOL,ACE_SYNCH_MUTEX,ACE_PI_Control_Block>	Malloc;
public:
	CShareMemoryPool();
	/**
	* @function			����
	* @param fname		ӳ���ļ���(��: f:/bf1000v4/dat/map/sys/blog.dat )
	* @param shmName	�����ڴ�����,���ڶ�λ�����ڴ�
	* @param len		�����ڴ��С
	*/
	CShareMemoryPool(const char* fname,const char* shmName,int len);
	virtual	~CShareMemoryPool();
public:
	//������ָ��
	const	void			*shmPointer()	const;
	int						sync();
protected:
	/**
	* @function			ӳ��Ӳ���ļ��������ڴ�
	* @param fname		ӳ���ļ���(��: f:/bf1000v4/dat/map/sys/blog.dat )
	* @param shmName	�����ڴ�����,���ڶ�λ�����ڴ�
	* @param len		�����ڴ��С
	*/
	bool	map(const char* fname,const char* shmName,int len);	

	//�ͷ��ڴ�
	void	unMap();					
private:
	void			*m_pShm;			//�����ڴ�ָ��
	Malloc			*m_pMalloc;			//�ڴ�������ָ��
};

#endif //_BFS_UTL_SHARE_MEMORY_POOL_H_

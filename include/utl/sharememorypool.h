/*==============================================================================
* 文件名称 : sharememorypool.h
* 文件功能 ：ACE共享内存基类(会产生map文件)
* 模块 : 共享内存
* 创建时间 : 2009-06-08
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本	: 1.1
* 修改摘要		: 
* 修改者		:  
* 修改日期		: 
* 使用说明		：map文件创建时，必须由某个线程独占。由于采用了进程锁，
				  没有使用线程锁，因此必须在进程启动时创建map文件
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
	* @function			构造
	* @param fname		映射文件名(如: f:/bf1000v4/dat/map/sys/blog.dat )
	* @param shmName	共享内存名称,用于定位共享内存
	* @param len		共享内存大小
	*/
	CShareMemoryPool(const char* fname,const char* shmName,int len);
	virtual	~CShareMemoryPool();
public:
	//参数区指针
	const	void			*shmPointer()	const;
	int						sync();
protected:
	/**
	* @function			映射硬盘文件到共享内存
	* @param fname		映射文件名(如: f:/bf1000v4/dat/map/sys/blog.dat )
	* @param shmName	共享内存名称,用于定位共享内存
	* @param len		共享内存大小
	*/
	bool	map(const char* fname,const char* shmName,int len);	

	//释放内存
	void	unMap();					
private:
	void			*m_pShm;			//共享内存指针
	Malloc			*m_pMalloc;			//内存分配对象指针
};

#endif //_BFS_UTL_SHARE_MEMORY_POOL_H_

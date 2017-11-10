#ifndef _BFS_UTL_SHARE_MEMORY_H_
#define _BFS_UTL_SHARE_MEMORY_H_

#include "utl/utl_export.h"

//共享内存管理类
class UTL_EXPORT CShareMemoryBase
{
private:
	void		*m_ptr;
#if defined(WIN32)
	HANDLE		m_shmId;
	bool		m_isCreated;
#endif
public:
	CShareMemoryBase();
	CShareMemoryBase(int shmkey,int len,const char* fname=NULL);
	virtual	~CShareMemoryBase();

	virtual	void	*shmPointer()	{return	m_ptr;}

	virtual	bool	checkShareMemory(int shmkey);
	virtual	bool	createShareMemory(int shmkey,int len,const char* fname=NULL);
	virtual	void	*mapShareMemory(int shmkey);
	virtual	bool	unMapShareMemory();
	virtual	bool	unMapShareMemory(void *shmMemoryPtr);
	virtual	bool	freeShareMemory(int shmkey);
	/**
	* @function			创建或者map共享内存,程序自动根据实际情况选其一
	* @param shmkey		共享内存key
	* @param len		欲开辟的共享内存长度
	* @param isNewCreate 是否是新创建的共享内存
	* @param fname		若要生成硬盘映射文件,则传入改变量
	* @retval			 共享内存基地址
	*/
	virtual	void	*createAndMapShareMemory(int shmkey,int len,bool& isNewCreate,const char* fname=NULL);
};

#endif //_BFS_UTL_SHARE_MEMORY_H_

#ifndef _BFS_UTL_SHARE_MEMORY_H_
#define _BFS_UTL_SHARE_MEMORY_H_

#include "utl/utl_export.h"

//�����ڴ������
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
	* @function			��������map�����ڴ�,�����Զ�����ʵ�����ѡ��һ
	* @param shmkey		�����ڴ�key
	* @param len		�����ٵĹ����ڴ泤��
	* @param isNewCreate �Ƿ����´����Ĺ����ڴ�
	* @param fname		��Ҫ����Ӳ��ӳ���ļ�,����ı���
	* @retval			 �����ڴ����ַ
	*/
	virtual	void	*createAndMapShareMemory(int shmkey,int len,bool& isNewCreate,const char* fname=NULL);
};

#endif //_BFS_UTL_SHARE_MEMORY_H_

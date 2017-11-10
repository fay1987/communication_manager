/*==============================================================================
* �ļ����� : memory.h
* ģ�� : �ڴ����
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : v1.1
* �޸�ժҪ : 
* �޸��� :  �����
* �޸����� : 2008��5��6��
==============================================================================*/
#ifndef _PDT_UTL_MEMORY_H_
#define _PDT_UTL_MEMORY_H_

#include <ace/OS_NS_stdlib.h>
#include "utl/utl_export.h"

namespace PDT
{
	//Malloc�Զ��ڴ����
	class	UTL_EXPORT	CMemoryMalloc
	{
	public:
		CMemoryMalloc(size_t size=0);
		virtual	~CMemoryMalloc ();
	public:
		//��С
		size_t	size()	const;
		//���ô�С
		void	*resize(size_t size,bool zero=false);		//zero���Ƿ�����
		//�ڴ��ַ
		void	*addr();
		const	void	*addr()	const;
	private:
		void	*m_ptr;
		size_t	m_size;
	};
	//----------------------------------------------------------------------------
	//New�Զ��ڴ����
	template<class T>
	class	CMemoryNew
	{
	public:
		CMemoryNew(size_t size);
		virtual	~CMemoryNew ();
	public:
		//��С
		size_t	size()	const;
		//���ô�С
		void	*resize(size_t size,bool zero=false);		//zero���Ƿ�����
		//�ڴ��ַ
		T		*addr();
		const	T	*addr()	const;
	private:
		T		*m_ptr;
		size_t	m_size;
	};
}

#include "utl/memoryNew.inl"

#endif

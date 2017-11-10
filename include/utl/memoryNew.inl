/*==============================================================================
* �ļ����� : memoryNew.cpp
* ģ�� : �ڴ����
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.2
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 2007��7��25��
==============================================================================*/
#include "utl/memory.h"
#include "ace/OS.h"
using	namespace	PDT;

//============================================================================
template<class T> ACE_INLINE
CMemoryNew<T>::CMemoryNew(size_t size)
{
	m_ptr = 0;
	m_size = 0;
	resize(size,false);
}

template<class T> ACE_INLINE
CMemoryNew<T>::~CMemoryNew()
{
	if(m_ptr)	delete	[]  m_ptr;
}
//----------------------------------------------------------------------------
template<class T> ACE_INLINE
size_t	CMemoryNew<T>::size()	const
{
	return	m_size;
}
//----------------------------------------------------------------------------
template<class T> ACE_INLINE
void	*CMemoryNew<T>::resize(size_t size,bool zero)
{
	if(size == 0)
	{
		if(m_ptr)	delete		m_ptr;
		m_ptr = 0;
	}
	else	if(size != m_size)
	{
		if(m_ptr)	delete		m_ptr;
		m_ptr = new	T[size];
		if(zero)	ACE_OS::memset(m_ptr,0,size*sizeof(T));
	}
	m_size = size;
	return	m_ptr;
}
//----------------------------------------------------------------------------
template<class T> ACE_INLINE
T	*CMemoryNew<T>::addr()
{
	return	m_ptr;
}

template<class T> ACE_INLINE
const	T	*CMemoryNew<T>::addr()	const
{
	return	m_ptr;
}


//============================================================================

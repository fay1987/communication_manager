/*==============================================================================
* 文件名称 : memory.h
* 模块 : 内存分配
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : v1.1
* 修改摘要 : 
* 修改者 :  尹宏昊
* 修改日期 : 2008年5月6日
==============================================================================*/
#ifndef _PDT_UTL_MEMORY_H_
#define _PDT_UTL_MEMORY_H_

#include <ace/OS_NS_stdlib.h>
#include "utl/utl_export.h"

namespace PDT
{
	//Malloc自动内存分配
	class	UTL_EXPORT	CMemoryMalloc
	{
	public:
		CMemoryMalloc(size_t size=0);
		virtual	~CMemoryMalloc ();
	public:
		//大小
		size_t	size()	const;
		//设置大小
		void	*resize(size_t size,bool zero=false);		//zero：是否清零
		//内存地址
		void	*addr();
		const	void	*addr()	const;
	private:
		void	*m_ptr;
		size_t	m_size;
	};
	//----------------------------------------------------------------------------
	//New自动内存分配
	template<class T>
	class	CMemoryNew
	{
	public:
		CMemoryNew(size_t size);
		virtual	~CMemoryNew ();
	public:
		//大小
		size_t	size()	const;
		//设置大小
		void	*resize(size_t size,bool zero=false);		//zero：是否清零
		//内存地址
		T		*addr();
		const	T	*addr()	const;
	private:
		T		*m_ptr;
		size_t	m_size;
	};
}

#include "utl/memoryNew.inl"

#endif

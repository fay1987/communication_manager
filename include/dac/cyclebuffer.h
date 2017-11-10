/*==============================================================================
* 文件名称 : cyclebuffer.h
* 文件功能 ：定义循环缓冲
* 模块 : 网关通道分发模块
* 创建时间 : 2009-06-08
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_BFS_DAC_CYCLEBUFFER_H_
#define	_BFS_DAC_CYCLEBUFFER_H_

#include "utl/datatype.h"

#define DEFAULT_BUFFER_SIZE		(1*1024*1024)	//1M

namespace BFS
{
	class CCycleBuffer
	{
	public:
		CCycleBuffer(int size=DEFAULT_BUFFER_SIZE);
		~CCycleBuffer();
	public:
		bool			get(hUChar& val);
		hInt32			get(hUChar* buf,hUInt32 len);

		bool			put(hUChar val);
		bool			put(const hUChar *buf,hUInt32 len);

		hInt32			length()	const;		//可用字节数
		hInt32			remain()	const;		//剩余空间长度

		bool			back(hInt32 num);		//将get的数据放回
	private:
		hUChar*			m_pData;				//缓冲区
		hUInt32			m_size;					//缓冲区大小
		hUInt32			m_write;				//缓冲区写指针
		hUInt32			m_read;					//缓冲区读指针		

	};
}
#endif	//_BFS_DAC_CYCLEBUFFER_H_

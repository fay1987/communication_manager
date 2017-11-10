#ifndef __BUFFEREXT_H__
#define	__BUFFEREXT_H__

#include "utl/datatype.h"

///默认缓冲区容量大小
#define  DEFAULT_BUF_CAPACITY		4096

class CBufferExt
{	
public:
	CBufferExt()
	{
		m_pData = NULL;
		m_capacity = 0;
		resize();
	}
	~CBufferExt()
	{
		if ( m_pData != NULL )
		{
			delete [] m_pData;
			m_pData = NULL;
		}
	}
public:
	///缓冲区首字节地址
	hUChar*			data(){return m_pData;}
	///可用字节数
	hInt32			length()	const
	{
		return m_length;
	}
	///设置新长度
	void			setLength(hUInt32 len)
	{
		if ( m_pData == NULL ) return;

		if ( len > m_capacity )
			m_length = m_capacity;
		else
			m_length = len;
	}
	///剩余空间长度
	hInt32			remain()	const
	{
		if ( m_pData == NULL ) return 0;
		return (m_capacity - m_length) <=0 ? 0 : (m_capacity - m_length);
	}
	///放入一个字节
	bool			put(hUChar val)
	{
		if ( m_pData == NULL ) return false;
		if ( m_length + 1 > m_capacity ) return false;

		m_pData[m_length] = val;
		m_length++;

		return true;
	}
	///放入多个字节
	bool			put(const hUChar *buf,hUInt32 len)
	{
		if ( m_pData == NULL ) return false;
		if ( buf == NULL || len <= 0 ) return false;

		if ( (m_length + len ) > m_capacity ) return false;

		memcpy(m_pData+m_length,buf,len);
		m_length += len;

		return true;
	}
	///移动前面指定len长度的字节
	void			move(hUInt32 len)
	{
		if ( m_pData == NULL ) return;

		if ( len >= m_length )
			reset();
		else
		{
			memmove(m_pData,m_pData+len,m_length - len );
			m_length -= len;
		}
	}
	///重新将缓冲区所有字节复位成指定val值
	void			reset(hUChar val=0)
	{
		if ( m_pData == NULL ) return;
		memset(m_pData,val,m_capacity);
		m_length = 0;
	}
	///重新设置容量
	/**
	* @function			若需要特殊的容量大小，可使用该函数设置
	* @param capacity	容量
	* @param val		缓冲区初始化指
	* @simple example	mainTx()函数起始位置,if( !isSending() ) return;
	*/
	void			resize(hUInt32 capacity=DEFAULT_BUF_CAPACITY,hUChar initVal=0)
	{
		if ( m_pData != NULL )
		{
			delete [] m_pData;
			m_pData = NULL;
		}

		if ( m_pData == NULL )
		{
			m_pData = new hUChar[capacity];
			m_capacity = capacity;
			reset(initVal);
		}
	}
private:
	hUChar*			m_pData;		//缓冲区
	hUInt32			m_capacity;		//当前容量
	hUInt32			m_length;		//当前有效长度				
};

#endif	//__BUFFEREXT_H__

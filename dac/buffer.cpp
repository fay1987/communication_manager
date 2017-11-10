/*==============================================================================
* 文件名称 : bufferinf.cpp
* 模块 : 前置数据访问接口
* 创建时间 : 2009-04-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "dac/buffer.h"

using namespace PDT;

CBuffer::CBuffer()
{
	CShareMemory shareMemory;
	if ( shareMemory.m_ptrCom == NULL )
	{
		m_pComPara = NULL;
		m_pComInfo = NULL;
	}
	else
	{
		m_pComPara = &shareMemory.m_ptrCom->para;
		m_pComInfo = &shareMemory.m_ptrCom->info;

		m_channel = -1;
		m_type = BUFFER_RX;
		//m_read = 0;	//edit by yff
	}
}

CBuffer::CBuffer(hInt32 channel,hUChar type /* = BUFFER_RX */)
{
	CShareMemory shareMemory;
	if ( shareMemory.m_ptrCom == NULL )
	{
		m_pComPara = NULL;
		m_pComInfo = NULL;
	}
	else
	{
		m_pComPara = &shareMemory.m_ptrCom->para;
		m_pComInfo = &shareMemory.m_ptrCom->info;

		init(channel,type);
	}
}

CBuffer::~CBuffer()
{
}

bool CBuffer::init(hInt32 channel,hUChar type)
{
	if ( channel < 0 || channel >= DAC_CHANNEL_NUM )
	{
		m_channel = -1;
		return false;
	}
	else
	{
		m_channel = channel;
		m_type = type;

		//edit by yff
		//if ( m_type == BUFFER_RX )
		//	m_read = m_pComInfo->recvBuffer[m_channel].write;
		//else
		//	m_read = m_pComInfo->sendBuffer[m_channel].write;
		//edit by yff end.
		return true;
	}
}

bool CBuffer::get(hUChar& val)
{
	if ( m_channel == -1 ) return false;

	//edit by yff
	if ( m_type == BUFFER_RX )
	{
		hUInt32 read = m_pComInfo->recvBuffer[m_channel].read;
		val = m_pComInfo->recvBuffer[m_channel].data[read];
		m_pComInfo->recvBuffer[m_channel].read = (read + 1) % DAC_RECV_BUFSIZE;
	}
	else
	{
		hUInt32 read = m_pComInfo->sendBuffer[m_channel].read;
		val = m_pComInfo->sendBuffer[m_channel].data[read];
		m_pComInfo->sendBuffer[m_channel].read = (read + 1) % DAC_SEND_BUFSIZE;
	}
	//edit by yff end
	return true;
}

hInt32	CBuffer::get(hUChar* buf,hUInt32 len)
{
	if ( m_channel == -1 ) return -1;

	hUInt32 bufSize = 0;
	hUInt32 write = 0;
	hUChar* pData = NULL;
	//edit by yff
	hUInt32 read = 0;
	//edit by yff end.

	if ( m_type == BUFFER_RX )
	{	
		bufSize = DAC_RECV_BUFSIZE;
		write = m_pComInfo->recvBuffer[m_channel].write;
		pData = m_pComInfo->recvBuffer[m_channel].data;
		read = m_pComInfo->recvBuffer[m_channel].read;	//edit by yff
	}
	else
	{
		bufSize = DAC_SEND_BUFSIZE;
		write = m_pComInfo->sendBuffer[m_channel].write;
		pData = m_pComInfo->sendBuffer[m_channel].data;
		read = m_pComInfo->sendBuffer[m_channel].read;	//edit by yff
	}

	//入口校验
	if ( buf == NULL || len < 0 || len > bufSize ) return -1;
	if ( len == 0 ) return 0;

	hUInt32 realLen = 0;
	if ( length() <= len ) //数据不够
		realLen = length();
	else
		realLen = len;


	////edit by yff
	if ( read > write )
	{
		if ( (read + realLen) >= bufSize )
		{
			hUInt32 hasLen,remainLen;
			hasLen = bufSize - read;
			memcpy( buf,pData+read,hasLen);
			remainLen = realLen - hasLen;
			memcpy( buf + hasLen,pData,remainLen );
			
			if ( m_type == BUFFER_RX )
			{	
				 m_pComInfo->recvBuffer[m_channel].read = (read + hasLen + remainLen) % bufSize;
			}
			else
			{
				m_pComInfo->sendBuffer[m_channel].read  = (read + hasLen + remainLen) % bufSize;
			}

			return hasLen + remainLen;
		}
		else
		{
			memcpy(buf,pData+read,realLen);

			if ( m_type == BUFFER_RX )
			{	
				m_pComInfo->recvBuffer[m_channel].read = (read + realLen) % bufSize;
			}
			else
			{
				m_pComInfo->sendBuffer[m_channel].read  = (read + realLen) % bufSize;
			}
			//m_read =( m_read + realLen ) % bufSize;
	
			return realLen;
		}
	}
	else
	{
		memcpy(buf,pData+read,realLen );
		if ( m_type == BUFFER_RX )
		{	
			m_pComInfo->recvBuffer[m_channel].read = (read + realLen) % bufSize;
		}
		else
		{
			m_pComInfo->sendBuffer[m_channel].read  = (read + realLen) % bufSize;
		}
		//m_read = (m_read + realLen ) % bufSize;

		return realLen;
	}

	//if ( m_read > write )
	//{
	//	if ( (m_read + realLen) >= bufSize )
	//	{
	//		hUInt32 hasLen,remainLen;
	//		hasLen = bufSize - m_read;
	//		memcpy( buf,pData+m_read,hasLen);
	//		remainLen = realLen - hasLen;
	//		memcpy( buf + hasLen,pData,remainLen );

	//		m_read = (m_read + hasLen + remainLen) % bufSize;

	//		return hasLen + remainLen;
	//	}
	//	else
	//	{
	//		memcpy(buf,pData+m_read,realLen);
	//		m_read =( m_read + realLen ) % bufSize;

	//		return realLen;
	//	}
	//}
	//else
	//{
	//	memcpy(buf,pData+m_read,realLen );
	//	m_read = (m_read + realLen ) % bufSize;

	//	return realLen;
	//}
	//edit by yff end.
}

bool CBuffer::put(hUChar val)
{
	if ( m_channel == -1 ) return false;

	hUInt32 write = 0;
	if ( m_type == BUFFER_RX ) 
	{
		write = m_pComInfo->recvBuffer[m_channel].write;
		m_pComInfo->recvBuffer[m_channel].data[write] = val;
		m_pComInfo->recvBuffer[m_channel].write = (write + 1) % DAC_RECV_BUFSIZE;
	}
	else
	{
		write = m_pComInfo->sendBuffer[m_channel].write;
		m_pComInfo->sendBuffer[m_channel].data[write] = val;
		m_pComInfo->sendBuffer[m_channel].write = (write + 1) % DAC_SEND_BUFSIZE;
	}
	return true;
}

bool CBuffer::put(const hUChar *buf,hUInt32 len)
{
	if ( m_channel == -1 ) return false;
	if ( buf == NULL || len <= 0 ) return false;

	hUInt32 bufSize = 0;
	hUInt32 write = 0;
	hUChar* pData = NULL;
	if ( m_type == BUFFER_RX )
	{	
		bufSize = DAC_RECV_BUFSIZE;
		write = m_pComInfo->recvBuffer[m_channel].write;
		pData = m_pComInfo->recvBuffer[m_channel].data;
	}
	else
	{
		bufSize = DAC_SEND_BUFSIZE;
		write = m_pComInfo->sendBuffer[m_channel].write;
		pData = m_pComInfo->sendBuffer[m_channel].data;
	}

	//需要环绕一遍
	if ( write + len > bufSize )
	{
		hUInt32 hasLen,remainLen;
		hasLen = bufSize - write;
		memcpy( pData+write,buf,hasLen );
		
		if ( m_type == BUFFER_RX )
		{
			m_pComInfo->recvBuffer[m_channel].write = (write + hasLen) % bufSize;
			write = m_pComInfo->recvBuffer[m_channel].write;
		}
		else 
		{
			m_pComInfo->sendBuffer[m_channel].write = (write + hasLen) % bufSize;
			write = m_pComInfo->sendBuffer[m_channel].write;
		}

		//剩余没拷贝的字节数
		remainLen = (len - hasLen >= bufSize - write ? bufSize - write : len - hasLen);
		memcpy( pData+write,buf+hasLen,remainLen );

		if ( m_type == BUFFER_RX )
			m_pComInfo->recvBuffer[m_channel].write = (write + remainLen) % bufSize;
		else 
			m_pComInfo->sendBuffer[m_channel].write = (write + remainLen) % bufSize;
	}
	else
	{
		memcpy( pData+write,buf,len );
		if ( m_type == BUFFER_RX )
			m_pComInfo->recvBuffer[m_channel].write = (write + len) % bufSize;
		else 
			m_pComInfo->sendBuffer[m_channel].write = (write + len) % bufSize;
	}
	return true;
}

hInt32	CBuffer::length()	const
{
	if ( m_channel == -1 ) return -1;

	hUInt32 bufSize = 0;
	hUInt32 write = 0;
	hUInt32 read = 0;	//edit by yff
	if ( m_type == BUFFER_RX )
	{	
		bufSize = DAC_RECV_BUFSIZE;
		write = m_pComInfo->recvBuffer[m_channel].write;
		read = m_pComInfo->recvBuffer[m_channel].read; //edit by yff
	}
	else
	{
		bufSize = DAC_SEND_BUFSIZE;
		write = m_pComInfo->sendBuffer[m_channel].write;
		read = m_pComInfo->sendBuffer[m_channel].read; //edit by yff
	}

	//edit by yff
	if( write >= read )	
		return write - read;	
	else
		return write + bufSize - read;  
	//edit by yff end.
}

hInt32	CBuffer::remain() const 
{
	if ( m_channel == -1 ) return -1;

	hUInt32 bufSize = 0;
	hUInt32 write = 0;
	hUInt32 read = 0;	//edit by yff
	if ( m_type == BUFFER_RX )
	{	
		bufSize = DAC_RECV_BUFSIZE;
		write = m_pComInfo->recvBuffer[m_channel].write;
		read = m_pComInfo->recvBuffer[m_channel].read; //edit by yff
	}
	else
	{
		bufSize = DAC_SEND_BUFSIZE;
		write = m_pComInfo->sendBuffer[m_channel].write;
		read = m_pComInfo->sendBuffer[m_channel].read; //edit by yff
	}

	//edit by yff
	if ( write > read )
		return bufSize - ( write - read ) -1 ;
	else if ( write < read )
		return read - write -1;
	else
		return bufSize - 1;
	//edit by yff end.
}

bool CBuffer::back(hInt32 num)
{
	if ( m_channel == -1 ) return false;
	if ( num <= 0 ) return false;

	//edit by yff
	hUInt32 read = 0;
	if ( m_type == BUFFER_RX )
	{
		read = m_pComInfo->recvBuffer[m_channel].read;
		m_pComInfo->recvBuffer[m_channel].read = (read + DAC_RECV_BUFSIZE - num) % DAC_RECV_BUFSIZE;
	}
	else
	{
		m_pComInfo->sendBuffer[m_channel].read;
		m_pComInfo->sendBuffer[m_channel].read  = (read + DAC_RECV_BUFSIZE - num) % DAC_RECV_BUFSIZE;
	}
	//if ( m_type == BUFFER_RX )
	//	m_read = ( m_read + DAC_RECV_BUFSIZE - num ) % DAC_RECV_BUFSIZE;
	//else
	//	m_read = ( m_read + DAC_SEND_BUFSIZE - num ) % DAC_SEND_BUFSIZE;
	//edit by yff end.
	return true;
}

#ifndef __BUFFEREXT_H__
#define	__BUFFEREXT_H__

#include "utl/datatype.h"

///Ĭ�ϻ�����������С
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
	///���������ֽڵ�ַ
	hUChar*			data(){return m_pData;}
	///�����ֽ���
	hInt32			length()	const
	{
		return m_length;
	}
	///�����³���
	void			setLength(hUInt32 len)
	{
		if ( m_pData == NULL ) return;

		if ( len > m_capacity )
			m_length = m_capacity;
		else
			m_length = len;
	}
	///ʣ��ռ䳤��
	hInt32			remain()	const
	{
		if ( m_pData == NULL ) return 0;
		return (m_capacity - m_length) <=0 ? 0 : (m_capacity - m_length);
	}
	///����һ���ֽ�
	bool			put(hUChar val)
	{
		if ( m_pData == NULL ) return false;
		if ( m_length + 1 > m_capacity ) return false;

		m_pData[m_length] = val;
		m_length++;

		return true;
	}
	///�������ֽ�
	bool			put(const hUChar *buf,hUInt32 len)
	{
		if ( m_pData == NULL ) return false;
		if ( buf == NULL || len <= 0 ) return false;

		if ( (m_length + len ) > m_capacity ) return false;

		memcpy(m_pData+m_length,buf,len);
		m_length += len;

		return true;
	}
	///�ƶ�ǰ��ָ��len���ȵ��ֽ�
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
	///���½������������ֽڸ�λ��ָ��valֵ
	void			reset(hUChar val=0)
	{
		if ( m_pData == NULL ) return;
		memset(m_pData,val,m_capacity);
		m_length = 0;
	}
	///������������
	/**
	* @function			����Ҫ�����������С����ʹ�øú�������
	* @param capacity	����
	* @param val		��������ʼ��ָ
	* @simple example	mainTx()������ʼλ��,if( !isSending() ) return;
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
	hUChar*			m_pData;		//������
	hUInt32			m_capacity;		//��ǰ����
	hUInt32			m_length;		//��ǰ��Ч����				
};

#endif	//__BUFFEREXT_H__

/*==============================================================================
* �ļ����� : cyclebuffer.h
* �ļ����� ������ѭ������
* ģ�� : ����ͨ���ַ�ģ��
* ����ʱ�� : 2009-06-08
* ���� : ����ƽ
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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

		hInt32			length()	const;		//�����ֽ���
		hInt32			remain()	const;		//ʣ��ռ䳤��

		bool			back(hInt32 num);		//��get�����ݷŻ�
	private:
		hUChar*			m_pData;				//������
		hUInt32			m_size;					//��������С
		hUInt32			m_write;				//������дָ��
		hUInt32			m_read;					//��������ָ��		

	};
}
#endif	//_BFS_DAC_CYCLEBUFFER_H_

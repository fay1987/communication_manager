/*==============================================================================
* �ļ����� : bufferinf.h
* �ļ����� �����屨�Ļ���ӿ�
* ģ�� : ǰ�òɼ��ӿ�
* ����ʱ�� : 2009-06-08
* ���� : ����ƽ
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_PDT_DAC_BUFFER_H_
#define	_PDT_DAC_BUFFER_H_

#include "dac/dacshm.h"
#include "dac/dacdef.h"

namespace PDT
{
	class DAC_EXPORT CBuffer
	{
	public:
		CBuffer();
		CBuffer(hInt32 channel,hUChar type = BUFFER_RX);
		~CBuffer();

	public:
		bool			init(hInt32 channel,hUChar type = BUFFER_RX);
		bool			get(hUChar& val);
		hInt32			get(hUChar* buf,hUInt32 len);

		bool			put(hUChar val);
		bool			put(const hUChar *buf,hUInt32 len);

		hInt32			length()	const;		//�����ֽ���
		hInt32			remain()	const;		//ʣ��ռ䳤��

		bool			back(hInt32 num);		//��get�����ݷŻ�
	private:
		DAC_COM_PARA		*m_pComPara;	
		DAC_COM_INFO		*m_pComInfo;	

		hInt32				m_channel;
		hUChar				m_type;

		//�������
		hUInt32				m_read;			//���ջ�������ָ��		

	};
}
#endif	//_PDT_DAC_BUFFER_H_

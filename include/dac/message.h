/*==============================================================================
* �ļ����� : message.h
* �ļ����� ������������Ĳ����ӿ�
* ģ�� : ǰ�òɼ��ӿ�
* ����ʱ�� : 2009-06-08
* ���� : ����ƽ
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_PDT_DAC_MESSAGE_H_
#define	_PDT_DAC_MESSAGE_H_

#include "dac/dacshm.h"

namespace PDT
{
	class DAC_EXPORT CMessage
	{
	public:
		CMessage();
		CMessage(hInt32 route);
		virtual ~CMessage();

	public:
		//////////////////////////////���Ľ�����///////////////////////////////////////
		bool			init(hInt32 route);	
		bool			hasMsg()	const;
		bool			get(DAC_MESSAGE& msg);
		bool			put(const DAC_MESSAGE& msg );

	private:
		DAC_COM_PARA	*m_pComPara;	
		DAC_COM_INFO	*m_pComInfo;	

		//�������
		hUInt32			m_read;		//�������Ļ�������ָ��
		hUInt32			m_route;

	public:
		//���ⲿָ�������￪ʼ��ȡmsg
		hInt32			writePos(hInt32 route)	const;
		bool			hasMsg(hInt32 route,hUInt32 readPos)	const;
		bool			get(hInt32 route,hUInt32 readPos,DAC_MESSAGE& msg);
	};
}
#endif	//_PDT_DAC_MESSAGE_H_

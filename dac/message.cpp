/*==============================================================================
* �ļ����� : message.cpp
* ģ�� : ǰ�����ݷ��ʽӿ�
* ����ʱ�� : 2009-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#include "dac/message.h"

using namespace PDT;

CMessage::CMessage()
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

		m_route = -1;
		m_read = 0;
	}
}

CMessage::CMessage(hInt32 route)
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

		init(route);
	}
}

CMessage::~CMessage()
{
}

//////////////////////////////���Ľ�����///////////////////////////////////////
bool CMessage::init(hInt32 route)
{
	if ( route < 0 || route >= DAC_ROUTE_NUM ) 
	{
		m_route = -1;
		return false;
	}
	else
	{
		m_route = route;
		m_read = m_pComInfo->messageBuf[m_route].write;
		return true;
	}
}

bool CMessage::hasMsg()	const
{
	if ( m_route == -1 ) return false;
	if ( m_read == m_pComInfo->messageBuf[m_route].write ) return false;
	return true;
}

bool CMessage::get(DAC_MESSAGE& msg)
{
	if( !hasMsg() )	return	false;

	memcpy(&msg,&m_pComInfo->messageBuf[m_route].message[m_read],sizeof(DAC_MESSAGE));
	m_read = (m_read + 1) % (DAC_MSG_NUM);

	return true;
}

bool CMessage::put(const DAC_MESSAGE& msg )
{
	if ( m_route == -1 ) return false;

	hUInt32 write = m_pComInfo->messageBuf[m_route].write;
	DAC_MESSAGE* pLocalMsg = &m_pComInfo->messageBuf[m_route].message[write];

	//������Ϣ
	memcpy( pLocalMsg,&msg,sizeof(DAC_MESSAGE) ); 
	m_pComInfo->messageBuf[m_route].write = (write + 1) % (DAC_MSG_NUM); 

	return true;
}

//���ⲿָ�������￪ʼ��ȡmsg
hInt32 CMessage::writePos(hInt32 route) const
{
	if ( route < 0 || route >= DAC_ROUTE_NUM ) return -1;

	return m_pComInfo->messageBuf[route].write;
}

bool CMessage::hasMsg(hInt32 route,hUInt32 readPos) const
{
	if ( route < 0 || route >= DAC_ROUTE_NUM ) return false;
	if ( readPos < 0 || readPos >= DAC_MSG_NUM ) return false;

	if ( readPos == m_pComInfo->messageBuf[route].write ) return false;
	return true;
}

bool CMessage::get(hInt32 route,hUInt32 readPos,DAC_MESSAGE &msg)
{
	if ( !hasMsg(route,readPos) ) return false;

	ACE_OS::memcpy(&msg,&m_pComInfo->messageBuf[route].message[readPos],sizeof(DAC_MESSAGE));

	return true;
}

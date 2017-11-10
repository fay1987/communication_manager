/*==============================================================================
* �ļ����� : channel.cpp
* ģ�� : �������ݲɼ���ת��ƽ̨--ƽ̨����()--ͨ������
* ����ʱ�� : 2008-12-01 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#include "channel.h"

CChannel::CChannel(short channelId)
:m_channelId(channelId)
{
	m_eChannelType = Unknown;
	m_eChannelState = Closed;
}

CChannel::~CChannel(void)
{

}

CChannel::ChannelState CChannel::state()
{
	return m_eChannelState;
}

CChannel::ChannelType	CChannel::type() const
{
	return m_eChannelType;
}

void CChannel::setType(ChannelType type)
{
	m_eChannelType = type;
}

int CChannel::channelId() const
{
	return m_channelId;
}

bool CChannel::init()
{
	return true;
}

void CChannel::open()
{

}

void CChannel::close()
{

}

void CChannel::remove()
{

}

int CChannel::recv()
{
	return 0;
}

int CChannel::send()
{
	return 0;
}

void CChannel::onSendData()
{

}


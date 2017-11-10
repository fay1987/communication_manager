/*==============================================================================
* 文件名称 : channel.cpp
* 模块 : 电力数据采集与转发平台--平台进程()--通道基类
* 创建时间 : 2008-12-01 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
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


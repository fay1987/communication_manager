/*==============================================================================
* 文件名称 : channelmanager.cpp
* 模块 : 电力数据采集与转发平台--平台进程()--通道管理类类
* 创建时间 : 2008-12-01 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "channelmanager.h"
#include "dac/dacdef.h"
#include "channeltcpcli.h"
#include "channeltcpsrv.h"
#include "channeludp.h" //yhh20100804
#include "sys/blog.h"

using namespace PDT;

CChannelManager::CChannelManager()
{
	m_sendDataTimer = new QTimer(this);
	ACE_OS::memset(m_pTcpSrvVec,0,sizeof(m_pTcpSrvVec));
}

CChannelManager::~CChannelManager()
{
}

bool CChannelManager::init()
{
	m_checkTimeId = m_checkTimer.add(MGR_TIMER_CHECK);
	m_checkTimer.start();

	DAC_SYSTEM_INFO* pSystemInfo = m_commInf.systemInfo();
	if ( pSystemInfo == NULL ) return false;

	DAC_CHANNEL* pChannel = NULL;
	for (hUInt32 i=0;i<pSystemInfo->channelNum;i++)
	{
		pChannel = m_commInf.channel(i);
		if ( pChannel == NULL ) continue;

		addChannel(i,pChannel->type);

	}//end for

	return true;
}

void CChannelManager::addChannel(int chanNo,int type)
{
	if ( chanNo < 0 || chanNo > DAC_CHANNEL_NUM ) return;

	if ( type == CHANNEL_TYPE_TCPCLIENT )
	{
		CChannelTcpCli* pChannel = new CChannelTcpCli(chanNo);
		if ( pChannel == NULL ) return;
		pChannel->init();
		pChannel->open();
		addChannel(pChannel);
	}
	else if ( type == CHANNEL_TYPE_TCPSERVER )
	{
		m_pTcpSrvVec[chanNo] = new CChannelTcpSrv(chanNo);
		m_pTcpSrvVec[chanNo]->init();
		m_pTcpSrvVec[chanNo]->open();
	}
	else if ( type == CHANNEL_TYPE_UDP ) //
	{
		CChannelUdp* pChannel = new CChannelUdp(chanNo);
		if ( pChannel == NULL ) return;
		pChannel->init();
		pChannel->open();
		addChannel(pChannel);	
	}
	else if ( type == CHANNEL_TYPE_MULTICASTUDP ) //20141223
	{
		CChannelUdp* pChannel = new CChannelUdp(chanNo);
		if ( pChannel == NULL ) return;
		pChannel->init();
		pChannel->multiopen();
		//pChannel->open();
		addChannel(pChannel);	
	}
	else return;
}

//client--remove client; server--remove stream + server
void CChannelManager::delChannel(int chanNo,int type)
{	
	if ( chanNo < 0 || chanNo >= DAC_CHANNEL_NUM ) return ;

	if ( type == CHANNEL_TYPE_TCPCLIENT )
	{
		for (hInt32 i=0;i<m_channelList.size();i++) 
		{
			if ( m_channelList.at(i)->type() == CChannel::TcpClient && m_channelList.at(i)->channelId() == chanNo )
				m_channelList.at(i)->remove();
		}
	}
	else if ( type == CHANNEL_TYPE_TCPSERVER )
	{
		for (hInt32 i=0;i<m_channelList.size();i++) 
		{
			if ( m_channelList.at(i)->type() == CChannel::TcpStream && m_channelList.at(i)->channelId() == chanNo )
				m_channelList.at(i)->remove();
		}

		if ( m_pTcpSrvVec[chanNo] != NULL ) 
		{
			m_pTcpSrvVec[chanNo]->remove();
			m_pTcpSrvVec[chanNo] = NULL;
		}
	}
	else if ( type == CHANNEL_TYPE_UDP ) //
	{
		for (hInt32 i=0;i<m_channelList.size();i++) 
		{
			if ( m_channelList.at(i)->type() == CChannel::Udp && m_channelList.at(i)->channelId() == chanNo )
				m_channelList.at(i)->remove();
		}
	}
	else if ( type == CHANNEL_TYPE_MULTICASTUDP ) //
	{
		for (hInt32 i=0;i<m_channelList.size();i++) 
		{
			if ( m_channelList.at(i)->type() == CChannel::Udp && m_channelList.at(i)->channelId() == chanNo )
				m_channelList.at(i)->remove();
		}
	}
	else 
	{
	}
}

bool CChannelManager::findChannel(CChannel* pChannel)
{
	if ( m_channelList.indexOf(pChannel) == -1 )
		return false;
	return true;
}

void CChannelManager::addChannel(CChannel* pChannel)
{
	if ( !pChannel )
		return;
	m_rwListLock.lockForWrite();
	connect( this,SIGNAL(sendData()),pChannel,SLOT( onSendData() ) );
	m_channelList.push_back( pChannel );
	m_rwListLock.unlock();
}

void CChannelManager::delChannel(CChannel* pChannel)
{
	if ( !pChannel )
		return;
	m_rwListLock.lockForWrite();
	pChannel->disconnect();

	//wfp add at 20110413--解决东富山服务端拔网线无法重连问题
	if ( pChannel->type() ==  CChannel::TcpStream )
	{
		if ( pChannel->channelId() >= 0 && pChannel->channelId() < DAC_CHANNEL_NUM )
		{
			CChannelTcpSrv* pTcpSrv = (CChannelTcpSrv*)m_pTcpSrvVec[pChannel->channelId()];
			if ( pTcpSrv != NULL ) pTcpSrv->removeStream();	
		}
	}

	if ( m_channelList.removeOne( pChannel ) ) 
	{
		//printf("channel %d is deleted from it's manager\n",(int)pChannel);delete lj 
	}
	m_rwListLock.unlock();
}

void CChannelManager::onSendDataTimeout()
{
	//printf("CChannelManager::onSendDataTimeout() is call...\n");
	if ( m_checkTimer.time_out(m_checkTimeId) ) 
	{
		checkClientChange();
		checkServerChange();
		checkUdpChange();
	}

	emit sendData();
}

void CChannelManager::startSendDataTimer(int msec)
{
	connect(m_sendDataTimer,SIGNAL(timeout()),this,SLOT( onSendDataTimeout() ) );
	m_sendDataTimer->start( msec );
}

void CChannelManager::stopSendDataTimer()
{
	m_sendDataTimer->stop();
}

void CChannelManager::checkClientChange()
{
	DAC_SYSTEM_INFO* pSystemInfo = m_commInf.systemInfo();
	if ( pSystemInfo == NULL ) return;

	DAC_CHANNEL* pChannel = m_commInf.channel(0,false);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(0,false);
	for (hUInt32 i=0;i<pSystemInfo->channelNum;i++,pChannel++,pChannelInfo++)
	{
		if ( pChannelInfo->reset & CHANNEL_TASK_CLIENT )
		{
			if ( pChannel->valid && pChannel->type == CHANNEL_TYPE_TCPCLIENT && !pChannelInfo->manStop )
			{
				delChannel(i,CHANNEL_TYPE_TCPCLIENT);
				addChannel(i,CHANNEL_TYPE_TCPCLIENT);
			}
			else
			{
				delChannel(i,CHANNEL_TYPE_TCPCLIENT);
			}
			pChannelInfo->reset &= ~CHANNEL_TASK_CLIENT;
		}
		
	}//end for
}

//暂不支持manStop,人工启动
void CChannelManager::checkServerChange()
{
	DAC_SYSTEM_INFO* pSystemInfo = m_commInf.systemInfo();
	if ( pSystemInfo == NULL ) return;

	DAC_CHANNEL* pChannel = m_commInf.channel(0,false);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(0,false);
	for (hUInt32 i=0;i<pSystemInfo->channelNum;i++,pChannel++,pChannelInfo++)
	{
		if ( pChannelInfo->reset & CHANNEL_TASK_SERVER )
		{
			if ( pChannel->valid && pChannel->type == CHANNEL_TYPE_TCPSERVER )
			{
				delChannel(i,CHANNEL_TYPE_TCPSERVER);
				addChannel(i,CHANNEL_TYPE_TCPSERVER);
			}
			else
			{
				delChannel(i,CHANNEL_TYPE_TCPSERVER);
			}
			pChannelInfo->reset &= ~CHANNEL_TASK_SERVER;
		}

	}//end for
}

void CChannelManager::checkUdpChange()
{
	DAC_SYSTEM_INFO* pSystemInfo = m_commInf.systemInfo();
	if ( pSystemInfo == NULL ) return;

	DAC_CHANNEL* pChannel = m_commInf.channel(0,false);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(0,false);
	for (hUInt32 i=0;i<pSystemInfo->channelNum;i++,pChannel++,pChannelInfo++)
	{
		if ( pChannelInfo->reset & CHANNEL_TASK_UDP )
		{
			if ( pChannel->valid && pChannel->type == CHANNEL_TYPE_UDP && !pChannelInfo->manStop )
			{
				delChannel(i,CHANNEL_TYPE_UDP);
				addChannel(i,CHANNEL_TYPE_UDP);
			}
			else
			{
				delChannel(i,CHANNEL_TYPE_UDP);
			}
			pChannelInfo->reset &= ~CHANNEL_TASK_UDP;
		}

	}//end for
}

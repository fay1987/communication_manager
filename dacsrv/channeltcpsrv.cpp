/*==============================================================================
* 文件名称 : channeltcpsrv.cpp
* 模块 : 电力数据采集与转发平台--平台进程--TCP服务端通道类
* 创建时间 : 2008-12-01 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "channeltcpsrv.h"
#include "channelstream.h"
#include "channelmanager.h"
#include "sys/blog.h"
#include "dac/dacdef.h"
#include <ace/INET_Addr.h>

extern CChannelManager* g_pChannelMgr;

using namespace PDT;

CChannelTcpSrv::CChannelTcpSrv(int channelId)
:CChannel(channelId)
,m_pSocketDev(0)
{
	m_eChannelType = TcpServer;
	m_pChannel = NULL;

	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannelInfo != NULL ) pChannelInfo->state = CHANNEL_STATE_DOWN;
}

CChannelTcpSrv::~CChannelTcpSrv(void)
{
	//close();
	if ( m_pSocketDev )
	{
		//delete m_pSocketDev;			//wfp masked at 20101130
		m_pSocketDev->close();
		m_pSocketDev->deleteLater();	//wfp add at 20101130
		m_pSocketDev = NULL;
	}
}

void CChannelTcpSrv::open()
{
	if ( !m_pSocketDev )
	{
		m_pSocketDev = new QTcpServer;
		if ( m_pSocketDev )
		{
			connect(m_pSocketDev,SIGNAL(newConnection ()),this,SLOT( onNewConnection() ) );
			//connect(m_pSocketDev,SIGNAL(newConnection ()),this,SLOT( onNewConnection() ) );
			m_pSocketDev->setMaxPendingConnections( 5 );
		}
	}

	DAC_CHANNEL* pChannel = m_commInf.channel(m_channelId);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannel == NULL || pChannelInfo == NULL ) 
	{
		if ( m_pSocketDev->isListening() )   close();
		return;
	}

	if ( !m_pSocketDev->isListening() )
	{
		ACE_INET_Addr netAddr(pChannel->localAddr);

		if ( m_pSocketDev->listen( QHostAddress(netAddr.get_host_addr()),netAddr.get_port_number() ) )
		{
			m_eChannelState = Listening;
			pChannelInfo->state = CHANNEL_STATE_LISTEN;

			logprint(LOG_NET_SERVER_BASE, "通道号: %d ,Listening:%s,端口:%d...", 
										   m_channelId,netAddr.get_host_addr(),netAddr.get_port_number());
		}
		else
			logprint(LOG_NET_SERVER_BASE, "通道号: %d ,Listening 失败:%s,端口:%d...", 
										   m_channelId,netAddr.get_host_addr(),netAddr.get_port_number());
	}//end if
}

void CChannelTcpSrv::close()
{
	if ( m_pChannel != NULL )
	{
		//m_pChannel->remove();	//manager已经remove stream
		//delete m_pChannel;	//qt对象最后不直接delete
		m_pChannel = NULL;
	}

	if ( m_pSocketDev )
		m_pSocketDev->close();

	m_eChannelState = Closed;
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannelInfo != NULL ) pChannelInfo->state = CHANNEL_STATE_DOWN;

}

void CChannelTcpSrv::remove()
{
	close();
	deleteLater();
}

//wfp add at 20110413--解决东富山服务端拔网线无法重连问题
void CChannelTcpSrv::removeStream()
{
	m_pChannel = NULL;
}

void CChannelTcpSrv::onNewConnection()
{
	QTcpSocket* pNewSocket = m_pSocketDev->nextPendingConnection();
	if ( pNewSocket )
	{
		QString str = pNewSocket-> peerAddress ().toString();
		logprint(LOG_NET_SERVER_BASE,"通道号: %d ,收到一个连接：remote host=%s,port=%d",
			                          m_channelId,pNewSocket-> peerAddress ().toString().toAscii().constData(),pNewSocket->peerPort());

		//如果已有一个客户连接到本端口，则不接受该连接
		if ( g_pChannelMgr->findChannel(m_pChannel) )
		{
			logprint(LOG_NET_SERVER_BASE,"通道号: %d ,该连接已经存在，本系统不允许同个端口接受多个连接，正在拒绝!",m_channelId);
			pNewSocket->close();
			pNewSocket->deleteLater();	//wfp add at 20110726
			pNewSocket = NULL;			//wfp add at 20110726
			return;
		}

		CChannel* pChannelObj = new CChannelStream( m_channelId,pNewSocket);
		if ( pChannelObj && pChannelObj->init() )
		{
			pChannelObj->open();
			g_pChannelMgr->addChannel( pChannelObj );
			m_pChannel = pChannelObj;
		}
		else
		{
			delete pChannelObj;
			pChannelObj = NULL;
			m_pChannel = NULL;
		}
	}
}



/*==============================================================================
* �ļ����� : channeltcpsrv.cpp
* ģ�� : �������ݲɼ���ת��ƽ̨--ƽ̨����--TCP�����ͨ����
* ����ʱ�� : 2008-12-01 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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

			logprint(LOG_NET_SERVER_BASE, "ͨ����: %d ,Listening:%s,�˿�:%d...", 
										   m_channelId,netAddr.get_host_addr(),netAddr.get_port_number());
		}
		else
			logprint(LOG_NET_SERVER_BASE, "ͨ����: %d ,Listening ʧ��:%s,�˿�:%d...", 
										   m_channelId,netAddr.get_host_addr(),netAddr.get_port_number());
	}//end if
}

void CChannelTcpSrv::close()
{
	if ( m_pChannel != NULL )
	{
		//m_pChannel->remove();	//manager�Ѿ�remove stream
		//delete m_pChannel;	//qt�������ֱ��delete
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

//wfp add at 20110413--�������ɽ����˰������޷���������
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
		logprint(LOG_NET_SERVER_BASE,"ͨ����: %d ,�յ�һ�����ӣ�remote host=%s,port=%d",
			                          m_channelId,pNewSocket-> peerAddress ().toString().toAscii().constData(),pNewSocket->peerPort());

		//�������һ���ͻ����ӵ����˿ڣ��򲻽��ܸ�����
		if ( g_pChannelMgr->findChannel(m_pChannel) )
		{
			logprint(LOG_NET_SERVER_BASE,"ͨ����: %d ,�������Ѿ����ڣ���ϵͳ������ͬ���˿ڽ��ܶ�����ӣ����ھܾ�!",m_channelId);
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



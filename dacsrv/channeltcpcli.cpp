/*==============================================================================
* �ļ����� : channeltcpcli.cpp
* ģ�� : �������ݲɼ���ת��ƽ̨--ƽ̨����--TCP�ͻ���ͨ����
* ����ʱ�� : 2008-12-01 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#include "channeltcpcli.h"
#include "sys/blog.h"
#include "dac/dacdef.h"
#include <ace/OS.h>
#include <ace/INET_Addr.h>
#include "channelmanager.h"

using namespace PDT;
extern CChannelManager* g_pChannelMgr;

CChannelTcpCli::CChannelTcpCli(short channelId )
:CChannel(channelId),m_pSocketDev(0)
{
	m_eChannelType = TcpClient;
	m_channelId = channelId;
	m_curConnect = 0;
	m_eChannelState = Closed;

	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannelInfo != NULL ) pChannelInfo->state = CHANNEL_STATE_DOWN;

	m_rxBuffer.init(channelId);
	m_txBuffer.init(channelId,BUFFER_TX);

	m_reConnectDelay = 0;
	m_isRemoved = false;
}

CChannelTcpCli::~CChannelTcpCli(void)
{
	g_pChannelMgr->delChannel(this);

	//wfp masked at 20100521(��ֹ������ʱ����������������������ͨ��״̬���ظ�����Ϊdown)
	//close();	

	if ( m_pSocketDev )
	{
		m_pSocketDev->close();
		m_pSocketDev->deleteLater();	//wfp add at 20101130
		m_pSocketDev = NULL;
	}
}

void CChannelTcpCli::open()
{
	if ( !m_pSocketDev )
	{
		m_pSocketDev = new QTcpSocket;
		if ( m_pSocketDev )
		{
			connect(m_pSocketDev,SIGNAL(connected ()),this,SLOT(onConnected()) );
			connect(m_pSocketDev,SIGNAL(disconnected ()),this,SLOT(onDisconnected()) );
			connect(m_pSocketDev,SIGNAL(error (QAbstractSocket::SocketError)),this,SLOT(onError(QAbstractSocket::SocketError)) );
			connect(m_pSocketDev,SIGNAL( readyRead()),this,SLOT(onReadyRead()) );
			connect(m_pSocketDev,SIGNAL( stateChanged ( QAbstractSocket::SocketState) ),this,SLOT(onStateChanged( QAbstractSocket::SocketState)) );

		}
		else return;
	}

	if ( m_pSocketDev->state() == QTcpSocket::ConnectingState || m_pSocketDev->state() == QTcpSocket::HostLookupState )
		return;

	//�������û�е���
	if ( m_reConnectDelay > 0 && !m_timer.time_out(m_timeId) ) return;

	m_eChannelState = Connecting;

	DAC_CHANNEL* pChannel = m_commInf.channel(m_channelId);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannel == NULL || pChannelInfo == NULL ) return ;

	ACE_INET_Addr netAddr(pChannel->remoteAddr[m_curConnect]);
	if ( netAddr.get_ip_address() == INADDR_ANY )
	{
		connectError();
		logprint(LOG_NET_CLIENT_BASE,"ͨ����: %d ,����ַ<%s:%d>��Ч���޷�����,�ȴ�����������ַ",
									  m_channelId,netAddr.get_host_addr(),netAddr.get_port_number());
		return;
	}

	logprint(LOG_NET_CLIENT_BASE, "ͨ����: %d ,��������Զ�˷�����:%s,�˿�:%d...", 
								   m_channelId,netAddr.get_host_addr(),netAddr.get_port_number());
	m_pSocketDev->connectToHost( netAddr.get_host_addr(),netAddr.get_port_number() );
}

bool CChannelTcpCli::isOpen()
{
	if ( m_eChannelState == Connected ) return true;
	return false;
}

void CChannelTcpCli::close()
{
	if ( m_pSocketDev )
	{
		m_pSocketDev->close();
	}

	m_eChannelState = Closed;

	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannelInfo == NULL ) return;

	pChannelInfo->state = CHANNEL_STATE_DOWN;
	pChannelInfo->lastDownTime = (hInt32)ACE_OS::time(0);
}

void CChannelTcpCli::remove()
{
	close();
	deleteLater();
	m_isRemoved = true;
}

int	CChannelTcpCli::recv()
{
	if ( !m_pSocketDev || m_pSocketDev->state() != QTcpSocket::ConnectedState || m_pSocketDev->bytesAvailable() <= 0 )
		return 0;

	long nRead = 0;
	hUChar strRentBuf[DAC_RECV_BUFSIZE];
	memset(strRentBuf,0,DAC_RECV_BUFSIZE);

	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannelInfo == NULL ) return -1;

	nRead = m_pSocketDev->read((char*)strRentBuf,DAC_RECV_BUFSIZE );
	if ( nRead > 0 )
	{
		logprint(LOG_NET_CLIENT_RECV,"channel=%d,<tcp client>dac recv <%d> bytes",m_channelId,nRead);
		pChannelInfo->rxByteCnt += nRead;
		m_rxBuffer.put(strRentBuf,nRead);
		/*//wfp test
		logprint(9999,"read %d bytes",nRead);
		char msg[1024] = "";
		CString str;
		for (int i=0;i<nRead;i++)
		{
			ACE_OS::sprintf(msg," %02x",strRentBuf[i]);
			str += msg;
		}
		logprint(9999,str.c_str());
		//wfp test end*/
	}
	return nRead;
}

int	CChannelTcpCli::send()
{
	if ( !m_pSocketDev ||  m_pSocketDev->state() != QTcpSocket::ConnectedState )
		return 0;

	long nWritten = 0;

	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannelInfo == NULL ) return -1;

	if ( m_txBuffer.length() > 0 )
	{
		hUChar strRentBuf[DAC_SEND_BUFSIZE];
		int len = m_txBuffer.get( strRentBuf,m_txBuffer.length() );
		if ( len <= 0 ) return -1;

		nWritten = m_pSocketDev->write((char *)strRentBuf ,len);
		if ( nWritten > 0 )
		{
			logprint(LOG_NET_CLIENT_SEND,"channel=%d,<tcp client>dac send <%d> bytes",m_channelId,nWritten);
			pChannelInfo->txByteCnt += nWritten;
			m_pSocketDev->flush();
		}
		//ȡ��������û��ȫ������
		if ( nWritten != len )
		{
			m_txBuffer.back( (nWritten <= 0) ? len : (len-nWritten) );
		}
	}

	return nWritten;
}

void CChannelTcpCli::onConnected ()
{
	DAC_CHANNEL* pChannel = m_commInf.channel(m_channelId);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannel == NULL || pChannelInfo == NULL ) return ;

	m_eChannelState = Connected;
	pChannelInfo->state = CHANNEL_STATE_UP;
	pChannelInfo->lastUpTime = (hInt32)ACE_OS::time(0);
	m_reConnectDelay = 0;

	ACE_INET_Addr netAddr(pChannel->remoteAddr[m_curConnect]);
	logprint(LOG_NET_CLIENT_BASE, "ͨ����: %d ,����Զ�˷�����:%s,�˿�:%d! �ɹ���", 
								   m_channelId,netAddr.get_host_addr(),netAddr.get_port_number());
}

void CChannelTcpCli::onDisconnected ()
{
	close();
}

void CChannelTcpCli::onError ( QAbstractSocket::SocketError socketError )
{
	//printf("Channel %d : Socket error *** %s\n",m_channelId,m_pSocketDev->errorString().toAscii().data() );

	switch ( socketError )
	{
	case QAbstractSocket::SocketTimeoutError:		//socket������ʱ
	case QAbstractSocket::ConnectionRefusedError:	//���ӱ��ܾ�
	case QAbstractSocket::HostNotFoundError:		//�����Ҳ���
	case QAbstractSocket::RemoteHostClosedError:	//�Զ��Ѿ��ر�
	case QAbstractSocket::NetworkError:				//�������(������)
	case QAbstractSocket::SocketAccessError:		
	case QAbstractSocket::SocketResourceError:
	case QAbstractSocket::DatagramTooLargeError:
	case QAbstractSocket::AddressInUseError:
	case QAbstractSocket::SocketAddressNotAvailableError:
	case QAbstractSocket::UnsupportedSocketOperationError:
	case QAbstractSocket::ProxyAuthenticationRequiredError:
	case QAbstractSocket::SslHandshakeFailedError:
	case QAbstractSocket::UnknownSocketError:
	case QAbstractSocket::UnfinishedSocketOperationError:
	default:
		//open();
		break;
	}
}

void CChannelTcpCli::onReadyRead()
{
	//printf("Channel %d : Socket is ready read,ready bytes = %d\n",m_channelId,m_pSocketDev-> bytesAvailable () );
	recv();
}

void CChannelTcpCli::onSendData()
{
	//printf("CChannelTcpCli::onSendData()...\n");
	if ( m_isRemoved ) return;

	if ( !isOpen() )
		open();
	else 
		send();
}

void CChannelTcpCli::onStateChanged ( QAbstractSocket::SocketState socketState )
{
	switch(socketState)
	{
	case QAbstractSocket::UnconnectedState:
		//open();
		connectError();
		break;
	case QAbstractSocket::HostLookupState:
	case QAbstractSocket::ConnectingState:
	case QAbstractSocket::ConnectedState:
	case QAbstractSocket::BoundState:
	case QAbstractSocket::ClosingState:
	case QAbstractSocket::ListeningState:
	default:
		break;

	}
}

/*
* �������ܣ��޷����ӶԶ�ʱ�Ĵ���
* ����˵�������Ӽ������0-3-6-12....-60��ʱ�ӷ�ʽ
*/
void CChannelTcpCli::connectError()
{
	DAC_CHANNEL* pChannel = m_commInf.channel(m_channelId);
	if ( pChannel != NULL )
	{
		ACE_INET_Addr netAddr(pChannel->remoteAddr[m_curConnect]);
		if ( m_eChannelState == Connected ) 
			logprint(LOG_NET_CLIENT_BASE, "ͨ����: %d ,����<%s:%d>���ر�",m_channelId,netAddr.get_host_addr(),netAddr.get_port_number());
		else
			logprint(LOG_NET_CLIENT_BASE, "ͨ����: %d ,����<%s:%d>ʧ��",m_channelId,netAddr.get_host_addr(),netAddr.get_port_number());
	}

	//ȷ�����Ӱ��ռ��ִ��
	if ( m_reConnectDelay <= 0 ) m_reConnectDelay = INTV_RECONNECT_DEALY;
	else m_reConnectDelay *= 2;

	if ( m_reConnectDelay >= MAX_RECONNECT_DELAY ) m_reConnectDelay = MAX_RECONNECT_DELAY;

	m_timer.removeAll();
	m_timeId = m_timer.add(m_reConnectDelay*1000);
	m_timer.start();

	//�л���һ�����ӵĵ�ַ
	m_curConnect = (m_curConnect+1)%DAC_ADDR_NUM;
}
bool CChannelTcpCli::isAddrValid(const char* host,unsigned short port )
{
	QHostAddress addr(host);
	if ( addr.isNull() )
		return false;
	if ( port == 0 )
		return false;
	return true;
}


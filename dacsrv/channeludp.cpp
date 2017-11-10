/*==============================================================================
* 文件名称 : channeltcpcli.cpp
* 模块 : 电力数据采集与转发平台--平台进程--TCP客户端通道类
* 创建时间 : 2008-12-01 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "channeludp.h"
#include "sys/blog.h"
#include "dac/dacdef.h"
#include <ace/OS.h>
#include <ace/INET_Addr.h>
#include "channelmanager.h"

using namespace PDT;
extern CChannelManager* g_pChannelMgr;

CChannelUdp::CChannelUdp(short channelId )
:CChannel(channelId),m_pSocketDevRecv(0),m_pSocketDevSend(0)
{
	m_eChannelType = Udp;
	m_channelId = channelId;
	m_curConnect = 0;
	m_eChannelState = Closed;

	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannelInfo != NULL ) pChannelInfo->state = CHANNEL_STATE_DOWN;

	m_rxBuffer.init(channelId);
	m_txBuffer.init(channelId,BUFFER_TX);

	m_isRemoved = false;
}

CChannelUdp::~CChannelUdp(void)
{
	g_pChannelMgr->delChannel(this);

	if ( m_pSocketDevRecv )
	{
		m_pSocketDevRecv->close();
		m_pSocketDevRecv->deleteLater();		//wfp add at 20101130
		m_pSocketDevRecv = NULL;
	}

	if ( m_pSocketDevSend )
	{
		m_pSocketDevSend->close();
		m_pSocketDevSend->deleteLater();		//wfp add at 20101130
		m_pSocketDevSend = NULL;
	}
}

void CChannelUdp::open()
{
	DAC_CHANNEL* pChannel = m_commInf.channel(m_channelId);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannel == NULL || pChannelInfo == NULL )
		return ;

	ACE_INET_Addr netAddrLocal(pChannel->localAddr);

	if ( !m_pSocketDevRecv )
	{
		m_pSocketDevRecv = new QUdpSocket(this);

		if ( m_pSocketDevRecv )
		{
			if ( m_pSocketDevRecv->bind(QHostAddress( QString(netAddrLocal.get_host_addr()) ),netAddrLocal.get_port_number() ) )
			{
				logprint(LOG_NET_CLIENT_BASE, "通道号: %d ,为UDP通道,本端地址:%s,端口:%d ！", 
					m_channelId,netAddrLocal.get_host_addr(),netAddrLocal.get_port_number());

				connect(m_pSocketDevRecv,SIGNAL(error (QAbstractSocket::SocketError)),this,SLOT(onError(QAbstractSocket::SocketError)) );
				connect(m_pSocketDevRecv,SIGNAL( readyRead()),this,SLOT(onReadyRead()) );
			}
			else
			{
				delete m_pSocketDevRecv;
				m_pSocketDevRecv = NULL;
				//return;	yhh20101124 允许多个通道监听同一个端口 
			}
		}
		else
			return;
	}

	if ( !m_pSocketDevSend )
	{
		m_pSocketDevSend = new QUdpSocket(this);
		if ( m_pSocketDevSend )
		{
			
		}
		else
			return ;
	}

	m_eChannelState = Connected;
	pChannelInfo->state = CHANNEL_STATE_UP;
	pChannelInfo->lastUpTime = (hInt32)ACE_OS::time(0);
}

void CChannelUdp::multiopen()
{
	DAC_CHANNEL* pChannel = m_commInf.channel(m_channelId);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannel == NULL || pChannelInfo == NULL )
		return ;

	ACE_INET_Addr netAddrLocal(pChannel->localAddr);
	ACE_INET_Addr netAddrRemote(pChannel->remoteAddr[0]);
		
	if ( !m_pSocketDevRecv )
	{
		
#ifdef	WIN32
		//add for multicast udp by wlq 20121222
		int retVal;
		if((retVal = WSAStartup(MAKEWORD(2,2), &wsd)) != 0)
		{
			qDebug() << "WSAStartup() failed";
			delete m_pSocketDevRecv;
			m_pSocketDevRecv = NULL;
			return ;
		}
		SOCKET Fsocket = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP/*IPPROTO_IP*/);
		if(Fsocket == INVALID_SOCKET)
		{
			qDebug() << "WSAStartup() failed";
			return ;
		}
		struct sockaddr_in FAddr;
		memset(&FAddr,0,sizeof(FAddr));
		FAddr.sin_family = AF_INET;
		FAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		FAddr.sin_port = htons(netAddrLocal.get_port_number());//htons(1888);
		//FAddr.sin_port = htons(1888);
		if(bind(Fsocket,(sockaddr*)&FAddr,sizeof(FAddr))==SOCKET_ERROR)
		{
			qDebug() << "WSAStartup() failed";
			return ;
		}

		struct ip_mreq imreq;
		//imreq.imr_multiaddr.s_addr = inet_addr("236.8.8.8");
		imreq.imr_multiaddr.s_addr = inet_addr(netAddrRemote.get_host_addr());
		imreq.imr_interface.s_addr = htonl(INADDR_ANY);
		if(setsockopt(Fsocket,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&imreq,sizeof(ip_mreq))==SOCKET_ERROR)
		{
			qDebug() << "WSAStartup() failed";
			return ;
		}
		
		m_pSocketDevRecv = new QUdpSocket(this);
		if ( m_pSocketDevRecv )
		{
			m_pSocketDevRecv->setSocketDescriptor(Fsocket);

#else
		m_pSocketDevRecv = new QUdpSocket(this);
		if ( m_pSocketDevRecv )
		{
			m_pSocketDevRecv->bind(QHostAddress::Any, netAddrLocal.get_port_number(), QUdpSocket::ShareAddress);
			if (m_pSocketDevRecv->joinMulticastGroup(QHostAddress(netAddrRemote.get_host_addr())))
			{
				logprint(LOG_NET_CLIENT_BASE, "joinMuticastGroup failed");
			}

#endif
			logprint(LOG_NET_CLIENT_BASE, "通道号: %d ,为UDP组播通道,本端地址:%s ,端口:%d ！",
				m_channelId,netAddrLocal.get_host_addr(),netAddrLocal.get_port_number());
			logprint(LOG_NET_CLIENT_BASE, " 组播分组:%s, ！", netAddrRemote.get_host_addr());
			connect(m_pSocketDevRecv,SIGNAL(error (QAbstractSocket::SocketError)),this,SLOT(onError(QAbstractSocket::SocketError)) );
			connect(m_pSocketDevRecv,SIGNAL( readyRead()),this,SLOT(onReadyRead()) );
		}
		else
		{
			delete m_pSocketDevRecv;
			m_pSocketDevRecv = NULL;
			//return;	yhh20101124 允许多个通道监听同一个端口 
		}
		//add end 

	}

	if ( !m_pSocketDevSend )
	{
		m_pSocketDevSend = new QUdpSocket(this);
		if ( m_pSocketDevSend )
		{
			
		}
		else
			return ;
	}

	m_eChannelState = Connected;
	pChannelInfo->state = CHANNEL_STATE_UP;
	pChannelInfo->lastUpTime = (hInt32)ACE_OS::time(0);
}

bool CChannelUdp::isOpen()
{
	if ( m_eChannelState == Connected )
		return true;
	else
		return false;
}

void CChannelUdp::close()
{
	if ( m_pSocketDevRecv )
	{
		m_pSocketDevRecv->close();
	}

	if ( m_pSocketDevSend )
	{
		m_pSocketDevSend->close();
	}

	m_eChannelState = Closed;

	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannelInfo == NULL ) return;

	pChannelInfo->state = CHANNEL_STATE_DOWN;
	pChannelInfo->lastDownTime = (hInt32)ACE_OS::time(0);
}

void CChannelUdp::remove()
{
	close();
	deleteLater();
	m_isRemoved = true;
}

int	CChannelUdp::recv()
{
	if ( !m_pSocketDevRecv )
		return 0;

	if ( !m_pSocketDevRecv->hasPendingDatagrams() )
		return 0;


	long nRead = 0;
	hUChar strRentBuf[DAC_RECV_BUFSIZE];
	memset(strRentBuf,0,DAC_RECV_BUFSIZE);


	//yhh20101124>>>-------------------------------begin
	//DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	//if ( pChannelInfo == NULL ) return -1;

	QHostAddress remoteAddr;
	quint16 remotePort=0;
	//yhh20101124<<<-------------------------------end

	nRead = m_pSocketDevRecv->readDatagram((char*)strRentBuf,DAC_RECV_BUFSIZE,&remoteAddr,&remotePort );
	if ( nRead > 0 )
	{
		logprint(LOG_NET_CLIENT_RECV,"channel=%d,<udp>dac recv <%d> bytes",m_channelId,nRead);
		
		//yhh20101124>>>-------------------------------begin
		for (int i=0;i<m_commInf.systemInfo()->channelNum;i++ )
		{
			DAC_CHANNEL* pChannel =  m_commInf.channel(i);
			DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(i);
			if ( pChannel==NULL || pChannelInfo == NULL ) 
				continue;

			ACE_INET_Addr netAddrLocal( pChannel->localAddr );
			ACE_INET_Addr netAddrRemote(pChannel->remoteAddr[0]);

			if ( pChannel->type == CHANNEL_TYPE_MULTICASTUDP)
			{
				logprint(LOG_NET_CLIENT_RECV,"OK:: 数据属于组播通道：channel=%d ,组播组=%s，来源IP=%s ",i,netAddrRemote.get_host_addr(),remoteAddr.toString().toAscii().data() );
				PDT::CBuffer tmpBuf(i);
				pChannelInfo->rxByteCnt += nRead;
				tmpBuf.put(strRentBuf,nRead);
				break;
			}
			else
			{
				if ( QString(netAddrRemote.get_host_addr()) == remoteAddr.toString() ) //&& netAddrLocal.get_port_number() == remotePort 
				{
					logprint(LOG_NET_CLIENT_RECV,"OK:: 数据属于通道：channel=%d ,字节: %d，来源IP：%s ",i,nRead,remoteAddr.toString().toAscii().data() );
					PDT::CBuffer tmpBuf(i);
					pChannelInfo->rxByteCnt += nRead;
					tmpBuf.put(strRentBuf,nRead);
					break;
				}
				else
				{
					logprint(LOG_NET_CLIENT_RECV,"####Error:: 比较通道：channel=%d ,IP=%s，来源IP=%s ",i,netAddrRemote.get_host_addr(),remoteAddr.toString().toAscii().data() );
				}
			}
		}
		//yhh20101124<<<-------------------------------end

	}
	return nRead;
}

int	CChannelUdp::send()
{
	if ( !m_pSocketDevSend )
		return 0;

	long nWritten = 0;

	DAC_CHANNEL* pChannel = m_commInf.channel(m_channelId);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannelInfo == NULL ) return -1;

	if ( m_txBuffer.length() > 0 )
	{
		hUChar strRentBuf[DAC_SEND_BUFSIZE];
		int len = m_txBuffer.get( strRentBuf,m_txBuffer.length() );
		if ( len <= 0 ) return -1;

		ACE_INET_Addr netAddrRemote(pChannel->remoteAddr[m_curConnect]);

		nWritten = m_pSocketDevSend->writeDatagram((char *)strRentBuf ,len,QHostAddress( QString(netAddrRemote.get_host_addr()) ) ,netAddrRemote.get_port_number() );
		if ( nWritten > 0 )
		{
			logprint(LOG_NET_CLIENT_SEND,"channel=%d,<tcp client>dac send <%d> bytes",m_channelId,nWritten);
			pChannelInfo->txByteCnt += nWritten;
			m_pSocketDevSend->flush();
		}
		else if ( nWritten < 0 )
		{
			m_curConnect = (m_curConnect+1)%DAC_ADDR_NUM;
			ACE_INET_Addr netAddrRemote2(pChannel->remoteAddr[m_curConnect]);
			nWritten = m_pSocketDevSend->writeDatagram((char *)strRentBuf ,len,QHostAddress( netAddrRemote2.get_host_addr()),netAddrRemote2.get_port_number() );
			if ( nWritten > 0 )
			{
				logprint(LOG_NET_CLIENT_SEND,"channel=%d,<tcp client>dac send <%d> bytes",m_channelId,nWritten);
				pChannelInfo->txByteCnt += nWritten;
				m_pSocketDevSend->flush();
			}	
		}

		//取出的数据没有全部发送
		if ( nWritten != len )
		{
			m_txBuffer.back( (nWritten <= 0) ? len : (len-nWritten) );
		}
	}

	return nWritten;
}

void CChannelUdp::onError ( QAbstractSocket::SocketError socketError )
{
	switch ( socketError )
	{
	case QAbstractSocket::SocketTimeoutError:		//socket操作超时
	case QAbstractSocket::ConnectionRefusedError:	//连接被拒绝
	case QAbstractSocket::HostNotFoundError:		//主机找不到
	case QAbstractSocket::RemoteHostClosedError:	//对端已经关闭
	case QAbstractSocket::NetworkError:				//网络错误(拔网线)
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
		break;
	}
}

void CChannelUdp::onReadyRead()
{
	recv();
}

void CChannelUdp::onSendData()
{
	if ( m_isRemoved ) 
		return;

	if ( !isOpen() )
		open();
	else 
		send();
}

void CChannelUdp::onStateChanged ( QAbstractSocket::SocketState socketState )
{
	switch(socketState)
	{
	case QAbstractSocket::UnconnectedState:
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

bool CChannelUdp::isAddrValid(const char* host,unsigned short port )
{
	QHostAddress addr(host);
	if ( addr.isNull() )
		return false;
	if ( port == 0 )
		return false;
	return true;
}


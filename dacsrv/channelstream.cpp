/*==============================================================================
* 文件名称 : channelstream.cpp
* 模块 : 电力数据采集与转发平台--动态接受的TCP通道类
* 创建时间 : 2008-12-01 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "channelstream.h"
#include "channelmanager.h"
#include "sys/blog.h"
#include "dac/comminf.h"
#include "dac/dacdef.h"
#include <ace/OS.h>

using namespace PDT;

extern CChannelManager* g_pChannelMgr;

CChannelStream::CChannelStream(int channelIdRef,QTcpSocket* socket)
:CChannel(channelIdRef)
,m_channelIdRef(channelIdRef)
,m_pSocketDev( socket )
{
	m_eChannelType = TcpStream;

	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannelInfo != NULL ) 
	{
		pChannelInfo->state = CHANNEL_STATE_UP;
		pChannelInfo->lastUpTime = (hInt32)ACE_OS::time(0);
	}

	m_rxBuffer.init(m_channelId);
	m_txBuffer.init(m_channelId,BUFFER_TX);
	m_isRemoved = false;
}

CChannelStream::CChannelStream(int channelId,int channelIdRef,QTcpSocket* socket)
:CChannel(channelId)
,m_channelIdRef(channelIdRef)
,m_pSocketDev( socket )
{
	m_eChannelType = TcpStream;

	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannelInfo != NULL ) 
	{
		pChannelInfo->state = CHANNEL_STATE_UP;
		pChannelInfo->lastUpTime = (hInt32)ACE_OS::time(0);
	}

	m_rxBuffer.init(m_channelId);
	m_txBuffer.init(m_channelId,BUFFER_TX);
	m_isRemoved = false;
}

CChannelStream::~CChannelStream(void)
{
	g_pChannelMgr->delChannel(this);
	close();	
}

bool CChannelStream::init()
{
	bool ret = CChannel::init();
	if ( m_pSocketDev )
	{
		connect(m_pSocketDev,SIGNAL(disconnected()),this,SLOT(onDisconnected()) );
		connect(m_pSocketDev,SIGNAL(error( QAbstractSocket::SocketError)),this,SLOT(onError( QAbstractSocket::SocketError)) );
		connect(m_pSocketDev,SIGNAL(readyRead()),this,SLOT(onReadyRead( )) );
		return ret;
	}
	else
		return false;
}

void CChannelStream::close()
{
	if ( m_pSocketDev ) 
	{
		if ( m_pSocketDev->isOpen() )
			m_pSocketDev->close();

		delete m_pSocketDev;
		m_pSocketDev = NULL;
	}

	m_eChannelState = Closed;
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannelInfo == NULL ) return;

	pChannelInfo->state = CHANNEL_STATE_LISTEN;
	pChannelInfo->lastDownTime = (hInt32)ACE_OS::time(0);
}

void CChannelStream::remove()
{
	close();
	deleteLater();
	m_isRemoved = true;
}

int CChannelStream::recv()
{
	if ( !m_pSocketDev || !m_pSocketDev->isValid() || m_pSocketDev->bytesAvailable() <= 0 )
		return 0;

	long nRead = 0;
	unsigned char strRentBuf[DAC_RECV_BUFSIZE];
	memset(strRentBuf,0,DAC_RECV_BUFSIZE);

	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channelId);
	if ( pChannelInfo == NULL ) return -1;

	nRead = m_pSocketDev->read((char*)strRentBuf,DAC_RECV_BUFSIZE );
	if ( nRead > 0 )
	{
		logprint(LOG_NET_SERVER_RECV,"<tcp server>chan=%d,dac recv <%d> bytes",m_channelId,nRead);
		pChannelInfo->rxByteCnt += nRead;
		m_rxBuffer.put(strRentBuf,nRead);
	}
	return nRead;
}

int CChannelStream::send()
{
	if ( !m_pSocketDev || !m_pSocketDev->isValid() )
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
			logprint(LOG_NET_SERVER_SEND,"<tcp server>chan=%d,dac send <%d> bytes",m_channelId,nWritten);
			pChannelInfo->txByteCnt += nWritten;
			m_pSocketDev->flush();
		}

		//取出的数据没有全部发送
		if ( nWritten != len )
		{
			m_txBuffer.back( (nWritten <= 0) ? len : (len-nWritten) );
		}
	}

	return nWritten;
}

void CChannelStream::onDisconnected ()
{
	m_pSocketDev->disconnect();
	deleteLater();
}

void CChannelStream::onError ( QAbstractSocket::SocketError socketError )
{
	switch ( socketError )
	{
	case QAbstractSocket::SocketTimeoutError:		//socket操作超时
	case QAbstractSocket::ConnectionRefusedError:	//连接被拒绝
	case QAbstractSocket::HostNotFoundError:		//主机找不到
		break;
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
		//printf("Channel %d : Socket error *** %s\n",m_channelId,m_pSocketDev->errorString().toAscii().data() );
		//请求QT disconnect 之前注册的事件，并删除本对象
		deleteLater();
		break;
	default:
		deleteLater();
		break;
	}
}

void CChannelStream::onReadyRead()
{
	//printf("Channel %d : Socket is ready read,ready bytes = %d\n",m_channelId,m_pSocketDev-> bytesAvailable () );
	recv();
}

void CChannelStream::onSendData()
{
	//printf("CChannelStream::onSendData()...\n");
	if ( m_isRemoved ) return;

	send();
}



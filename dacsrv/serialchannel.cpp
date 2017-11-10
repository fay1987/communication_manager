#include "serialchannel.h"
#include "dac/dac.h"
#include "sys/blog.h"


using namespace PDT;

CSerialChannel::CSerialChannel(int channel)
:m_channel(channel)
{
	m_remoteAddrOrder = 0;
	m_openErrNum = 0;
	m_lastOpenTime = ACE_OS::gettimeofday();

	m_rxBuffer.init(m_channel);
	m_txBuffer.init(m_channel,BUFFER_TX);
}

CSerialChannel::~CSerialChannel()
{
	if ( isOpen() ) close();
}

bool CSerialChannel::init()
{
	DAC_CHANNEL* pChannel = m_commInf.channel(m_channel);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channel);
	if ( pChannel == NULL || pChannelInfo == NULL ) return false;

	if ( pChannel->type == CHANNEL_TYPE_SERIAL )
	{
		pChannelInfo->reset &= ~CHANNEL_TASK_SERIAL;
	}
	return true;
}

int CSerialChannel::open()
{
	DAC_CHANNEL* pChannel = m_commInf.channel(m_channel);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channel);
	if ( pChannel == NULL || pChannelInfo == NULL ) return -1;
	if ( isOpen() ) return 0;

	if ( m_openErrNum > 0 )
	{
		ACE_Time_Value curTime = ACE_OS::gettimeofday();
		if ( (curTime - m_lastOpenTime).sec() < SERIAL_REOPEN_DELAY ) return 0;
	}
	m_lastOpenTime = ACE_OS::gettimeofday();

	logprint(LOG_SERIAL_BASE,"<channel=%d>, begin open serial<%s>!",m_channel,pChannel->remoteAddr[m_remoteAddrOrder]);
	CSerial::open( pChannel->remoteAddr[m_remoteAddrOrder] );
	m_remoteAddrOrder = (m_remoteAddrOrder+1) % DAC_ADDR_NUM;

	if ( isOpen() ) 
	{
		logprint(LOG_SERIAL_BASE,"<channel=%d>,open sucess!",m_channel);
		pChannelInfo->state = CHANNEL_STATE_UP;
		m_openErrNum = 0;
		return 0;
	}

	++m_openErrNum;
	pChannelInfo->state = CHANNEL_STATE_DOWN;

	return -1;
}

int CSerialChannel::close()
{
	CSerial::close();
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channel);
	if ( pChannelInfo == NULL ) return -1;
	pChannelInfo->state = CHANNEL_STATE_DOWN;
	return 0;
}

int CSerialChannel::readData()
{
	hInt32 recvBytes = 0;
	ACE_OS::memset(m_buffer,0,DAC_RECV_BUFSIZE);

	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channel);
	if ( pChannelInfo == NULL ) return -1;

	recvBytes = read(m_buffer, sizeof(m_buffer));
	if ( recvBytes <= 0 ) 
	{
		//logprint(LOG_SERIAL_RECV,"<serial recv(channel=%d)> none",m_channel);
		return -1;
	}
	logprint(LOG_SERIAL_RECV,"<serial(channel=%d)>dac recv <%d> bytes : %s",m_channel,recvBytes, m_buffer);
	//yff
	char msg[1024] = "";
	CString strRecv;
	for (int i=0;i<recvBytes;i++)
	{
		ACE_OS::sprintf(msg," %02x",m_buffer[i]);
		strRecv += msg;
	}
	//yff end
	pChannelInfo->rxByteCnt += recvBytes;
	m_rxBuffer.put(m_buffer,recvBytes);

	return 0;
}

int CSerialChannel::writeData()
{
	hInt32 sendBytes = 0;
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channel);
	if ( pChannelInfo == NULL ) return -1;

	if ( m_txBuffer.length() > 0 )
	{
		hUChar sendBuf[DAC_SEND_BUFSIZE];
		int len = m_txBuffer.get( sendBuf,m_txBuffer.length() );
		if ( len <= 0 ) return 0;

		sendBytes = write_n( sendBuf,len );
		if ( sendBytes <= 0 ) 
		{
			m_txBuffer.back( sendBytes );
			//logprint(LOG_SERIAL_SEND,"<serial send>channel=%d none",m_channel);
			return -1;
		}
		//add for test by wlq 20170106
		char msg[1024] = "";
		CString str;
		for (int i=0;i<sendBytes;i++)
		{
			ACE_OS::sprintf(msg," %02x",sendBuf[i]);
			str += msg;
		}
				
		//add end
		CDateTime tNow = CDateTime::currentDateTime();
		logprint(22000+m_channel, "SEND -- %02d:%02d:%02d  %03d, 通道%d     %s",tNow.hour(), tNow.minute(), tNow.second(), tNow.microsec()/1000, m_channel ,str.c_str());

		logprint(LOG_SERIAL_SEND,"<serial ms = %d >chanel=%d,curRouteOrder = %d,dac send <%d> bytes , msg = %s",ACE_OS::gettimeofday().msec(),m_channel,(int)(pChannelInfo->curRouteOrder),sendBytes,str.c_str());
		pChannelInfo->txByteCnt += sendBytes;

		//取出的数据没有全部发送
		if ( sendBytes != len )
		{
			m_txBuffer.back( (sendBytes <= 0) ? len : (len-sendBytes) );
		}
	}//end if

	return 0;
}

void CSerialChannel::run()
{
	checkChange();

	readData();
	writeData();
}

void CSerialChannel::checkChange()
{
	DAC_CHANNEL* pChannel = m_commInf.channel(m_channel);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channel);
	if ( pChannel == NULL || pChannelInfo == NULL ) return;
	if ( !isOpen() ) return;

	if ( pChannelInfo->reset & CHANNEL_TASK_SERIAL )
	{
		close();
		pChannelInfo->reset &= ~CHANNEL_TASK_SERIAL;
	}
	
	if ( pChannelInfo->manStop )
		close();
}

#include "serialchannelwireless.h"
#include "dac/dac.h"
#include "sys/blog.h"
#ifndef WIN32
#include <linux/ioctl.h>
#include <time.h>
#include "sys/blog.h"
#endif

using namespace PDT;

#ifndef WIN32
#define DEV_GPIO "/dev/am335x_gpio"
#define GPIO_IOC_MAGIC   'G'
#define IOCTL_GPIO_SETOUTPUT              _IOW(GPIO_IOC_MAGIC, 0, int)   
#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))
#endif

CSerialChannelWireless::CSerialChannelWireless(int channel)
:m_channel(channel)
{
	m_remoteAddrOrder = 0;
	m_openErrNum = 0;
	m_lastOpenTime = ACE_OS::gettimeofday();

	m_rxBuffer.init(m_channel);
	m_txBuffer.init(m_channel,BUFFER_TX);
}

CSerialChannelWireless::~CSerialChannelWireless()
{
	if ( isOpen() ) close();
}

bool CSerialChannelWireless::init()
{
	DAC_CHANNEL* pChannel = m_commInf.channel(m_channel);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channel);
	if ( pChannel == NULL || pChannelInfo == NULL ) return false;

	if ( pChannel->type == CHANNEL_TYPE_SERIALWIRELESS )
	{
		pChannelInfo->reset &= ~CHANNEL_TASK_SERIAL;
	}
	return true;
}

int CSerialChannelWireless::open()
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
	parse(pChannel->remoteAddr[m_remoteAddrOrder]);
	CString strPot = getPortStr();
	resetSerial(strPot);
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

int CSerialChannelWireless::close()
{
	CSerial::close();
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channel);
	if ( pChannelInfo == NULL ) return -1;
	pChannelInfo->state = CHANNEL_STATE_DOWN;
	return 0;
}

int CSerialChannelWireless::readData()
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
	CDateTime tNow = CDateTime::currentDateTime();
	logprint(23000+m_channel, "  RECEIVE -- %02d:%02d:%02d  %03d, 通道%d     %s",tNow.hour(), tNow.minute(), tNow.second(), tNow.microsec()/1000, m_channel ,strRecv.c_str());

	pChannelInfo->rxByteCnt += recvBytes;
	m_rxBuffer.put(m_buffer,recvBytes);

	return 0;
}

int CSerialChannelWireless::writeData()
{
	logprint(LOG_SERIAL_SEND,"<serial send>channel=%d 0",m_channel);

	hInt32 sendBytes = 0;
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_channel);
	if ( pChannelInfo == NULL ) return -1;

	logprint(LOG_SERIAL_SEND,"<serial send>channel=%d 1",m_channel);


	if ( m_txBuffer.length() > 0 )
	{
		logprint(LOG_SERIAL_SEND,"<serial send>channel=%d 2",m_channel);

		hUChar sendBuf[DAC_SEND_BUFSIZE];
		int len = m_txBuffer.get( sendBuf,m_txBuffer.length() );
		if ( len <= 0 ) return 0;

		sendBytes = write_n( sendBuf,len );
		if ( sendBytes <= 0 ) 
		{
			m_txBuffer.back( sendBytes );
			logprint(LOG_SERIAL_SEND,"<serial send>channel=%d none",m_channel);
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
		logprint(23000+m_channel, "    \n     ");

		pChannelInfo->txByteCnt += sendBytes;

		//取出的数据没有全部发送
		if ( sendBytes != len )
		{
			m_txBuffer.back( (sendBytes <= 0) ? len : (len-sendBytes) );
		}
	}//end if

	logprint(LOG_SERIAL_SEND,"<serial send>channel=%d 3",m_channel);

	return 0;
}

void CSerialChannelWireless::run()
{
	checkChange();

	//readData();
	//writeData();
}

void CSerialChannelWireless::checkChange()
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


void CSerialChannelWireless::resetSerial(PDT::CString& portStr)
{

	hUInt8	nResetGroup = 0;
	hUInt8	nResetNo = 0;

	if (portStr.compare("ttyO1") == 0)
	{
		nResetGroup = 1;
		nResetNo = 12;
	}
	else if (portStr.compare("ttyO2") == 0)
	{
		nResetGroup = 1;
		nResetNo = 13;
	}
	else if (portStr.compare("ttyO3") == 0)
	{
		nResetGroup = 1;
		nResetNo = 14;
	}
	else if (portStr.compare("ttyO4") == 0)
	{
		nResetGroup = 1;
		nResetNo = 15;
	}
	else if (portStr.compare("ttyO5") == 0)
	{
		nResetGroup = 0;
		nResetNo = 27;
	}

	//hUInt8 resetGroup[5] = {1, 1, 1, 1, 0};
	//hUInt8 resetNo[5] = {12, 13, 14, 15, 27};


#ifndef WIN32

	int	fd = ::open(DEV_GPIO, O_RDWR);
	if (fd < 0) 
	{
		printf("Error device open fail! %d\n", fd);
		return;
	}

	am335x_gpio_arg arg;
	
	//for (int i = 0; i < 5; i++)
	//{
	//	arg.pin = GPIO_TO_PIN(resetGroup[i], resetNo[i]);
		arg.pin = GPIO_TO_PIN(nResetGroup, nResetNo);
		arg.data = 0;
		ioctl(fd, IOCTL_GPIO_SETOUTPUT, &arg);
		logprint(20059, "%s : group: %d  no: %d  电平位置低位", portStr.c_str(), nResetGroup, nResetNo);

	//}

	ACE_Time_Value tv(0,70000);  //（秒  微秒 )    70毫秒
	ACE_OS::sleep(tv);  //等70ms

	//int lastTime = 0;
	//int now=clock()/CLOCKS_PER_SEC * 1000;
	//for(int i=0;i < 100;)//完成100毫秒定时
	//{
	//	lastTime=clock()/CLOCKS_PER_SEC * 1000;
	//	i=lastTime-now;
	//}


	//for (int i = 0; i < 5; i++)
	//{
		//arg.pin = GPIO_TO_PIN(resetGroup[i], resetNo[i]);
		arg.pin = GPIO_TO_PIN(nResetGroup, nResetNo);
		arg.data = 1;
		ioctl(fd, IOCTL_GPIO_SETOUTPUT, &arg);
		logprint(20059, "%s : group: %d  no: %d  电平位置高位", portStr.c_str(), nResetGroup, nResetNo);

	//}

	::close(fd);

#endif
}


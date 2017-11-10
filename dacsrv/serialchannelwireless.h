#ifndef	_PDT_DAC_SERIALCHANNELWIRELESS_H_
#define _PDT_DAC_SERIALCHANNELWIRELESS_H_

#include "dac/comminf.h"
#include "dac/buffer.h"
#include "serial.h"

#define SERIAL_REOPEN_DELAY	3		//串口重新打开延时

typedef struct 
{
	int pin;
	int data;
}am335x_gpio_arg;


class CSerialChannelWireless : public CSerial
{
public:
	CSerialChannelWireless(int channel);
	virtual ~CSerialChannelWireless();
public:
	int					open();
	int					close();
	void				run();
	int					readData();
	int					writeData();
private:
	bool				init();
	void				checkChange();

	void				resetSerial(PDT::CString& portStr);

private:
	PDT::CCommInf		m_commInf;
	int					m_channel;
	hUChar				m_buffer[DAC_RECV_BUFSIZE];
	PDT::CBuffer		m_rxBuffer;
	PDT::CBuffer		m_txBuffer;
	int					m_remoteAddrOrder;
	int					m_openErrNum;		//连续open错误次数
	ACE_Time_Value		m_lastOpenTime;
};

#endif //_PDT_DAC_SERIALCHANNEL_H_

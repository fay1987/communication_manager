#ifndef	_PDT_DAC_SERIALCHANNEL_H_
#define _PDT_DAC_SERIALCHANNEL_H_

#include "dac/comminf.h"
#include "dac/buffer.h"
#include "serial.h"

#define SERIAL_REOPEN_DELAY	3		//串口重新打开延时
class CSerialChannel : public CSerial
{
public:
	CSerialChannel(int channel);
	virtual ~CSerialChannel();
public:
	int					open();
	int					close();
	void				run();
private:
	bool				init();
	void				checkChange();
	int					readData();
	int					writeData();
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

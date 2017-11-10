#ifndef	_PDT_DAC_SERIALWTASK_H_
#define	_PDT_DAC_SERIALWTASK_H_

#include "ace/OS.h"
#include <ace/Task.h>
#include "dac/comminf.h"
#include "serialchannel.h"
#include "utl/intervaltimerset.h"
#include "serialchannelwireless.h"
#include <QMap>

using namespace PDT;

class CSerialWTask : public ACE_Task<ACE_NULL_SYNCH>
{
public:
	CSerialWTask();
	virtual ~CSerialWTask();
	bool	init();
	hInt32	start();
	void	stop();
	hUInt32	serialChannelNumber() const;	//获取有效通道数目
	//主处理过程
	hInt32	svc();
public:
	//hInt32	lockSerialChannel();					//得到空闲链路并锁定
	//void	unLockSerialChannel(hInt32 channel);	//解锁链路
	
	void	setTimeOutFlag(hBool bFlag){m_bTimeOut = bFlag;}

private:
	//bool	isSerialChannel(hInt32 channel) const;
	bool	isWirelessSerialChannel(hInt32 channel) const;

	void	checkObject();							//检测对象是否存在
private:
	CCommInf			m_commInf;
	ACE_Thread_Mutex	m_mutex;
	hUInt32				m_curChanNo;
	//struct	_serial_channel_
	//{
	//	bool			lock;
	//	CSerialChannelWireless*	pObj;
	//}m_serialChannel[DAC_CHANNEL_NUM];

	QMap<int, CSerialChannelWireless*>  m_mapSerialChannel;

	//int m_nReceiveNum;	//记录收到数据的通道数
	int m_timeOutID;		//超时切换时间
	//int m_recOutID;
	//nBool  m_bflag;
	PDT::CIntervalTimerSet			m_tmSet;					/*时间定时器					*/
	
	hBool	m_bTimeOut;
};

#endif //_PDT_DAC_SERIALTASK_H_

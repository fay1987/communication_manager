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
	hUInt32	serialChannelNumber() const;	//��ȡ��Чͨ����Ŀ
	//���������
	hInt32	svc();
public:
	//hInt32	lockSerialChannel();					//�õ�������·������
	//void	unLockSerialChannel(hInt32 channel);	//������·
	
	void	setTimeOutFlag(hBool bFlag){m_bTimeOut = bFlag;}

private:
	//bool	isSerialChannel(hInt32 channel) const;
	bool	isWirelessSerialChannel(hInt32 channel) const;

	void	checkObject();							//�������Ƿ����
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

	//int m_nReceiveNum;	//��¼�յ����ݵ�ͨ����
	int m_timeOutID;		//��ʱ�л�ʱ��
	//int m_recOutID;
	//nBool  m_bflag;
	PDT::CIntervalTimerSet			m_tmSet;					/*ʱ�䶨ʱ��					*/
	
	hBool	m_bTimeOut;
};

#endif //_PDT_DAC_SERIALTASK_H_

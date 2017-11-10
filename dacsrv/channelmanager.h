/*==============================================================================
* �ļ����� : channelmanager.h
* ģ�� : �������ݲɼ���ת��ƽ̨--ƽ̨����(pdat.exe)--ͨ����������
* ����ʱ�� : 2008-12-01 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef __PDAT_CHANNELMANAGER_H__
#define __PDAT_CHANNELMANAGER_H__

#include "channel.h"
#include "dac/comminf.h"
#include "utl/intervaltimerset.h"

#define MGR_TIMER_CHECK		1000	//ms

class CChannelManager : public QObject 
{
	Q_OBJECT
public:
	CChannelManager();
	virtual ~CChannelManager();

	bool		init();

	void		addChannel(int chanNo,int type);
	void		delChannel(int chanNo,int type);


	bool		findChannel(CChannel* pChannel);
	void		addChannel(CChannel* pChannel);
	void		delChannel(CChannel* pChannel); 


	void		startSendDataTimer(int msec=10);
	void		stopSendDataTimer();
	void		checkClientChange();
	void		checkServerChange();
	void		checkUdpChange();
protected Q_SLOTS:
	void		onSendDataTimeout();
Q_SIGNALS:
	void		sendData();
private:
	QList<CChannel*>		m_channelList;	//���tcplcient&stream
	QReadWriteLock			m_rwListLock;
	QTimer*					m_sendDataTimer;
	PDT::CCommInf			m_commInf;
	CChannel*				m_pTcpSrvVec[DAC_CHANNEL_NUM];
	PDT::CIntervalTimerSet	m_checkTimer;
	int						m_checkTimeId;
};
#endif	//__PDAT_CHANNELMANAGER_H__


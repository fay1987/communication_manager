#ifndef _DAC_STATE_TASK_H
#define _DAC_STATE_TASK_H

#include <ace/Task.h>
#include "dac/comminf.h"
#include "dac/datainf.h"
#include "dac/dacevent.h"
#include "utl/string.h"
#include "utl/intervaltimerset.h"
#include "sys/blog.h"

#define ROUTE_NO_OK_DATA_TIME	15		//·������û����ȷ�����ݵ����ʱ��(s)	
#define QUALITY_CHECK_TIME		10		//����������(s)

class CStateTask : public ACE_Task<ACE_NULL_SYNCH>
{
public:
	CStateTask();
	virtual ~CStateTask();

	int			start();
	void		stop();
	int			svc();

	void		checkChannelState();
	void		checkGroupState();
	void		checkRouteState();
	void		checkDeviceState();
private:
	hUInt8		mainRouteState(int routNo);			//У����·��״̬
	hUInt8		routeStateWithChanUp(int routeNo);	//����·����������Chan Upʱ��״̬
	void		checkRouteScanCycle(int routeNo);	//����·��ɨ������
	void		refreshCounter();					//������ˢ��
	int			findRouteInGroup(int groupNo,int routeState,int filterRouteNo = -1);	//����һ��ָ��״̬��·��
	hUInt32		lastestUpdateTime(hInt32 route);
	void		checkQuality();						//���ʵʱ����������
	void		checkQualityRoute(int route);
	void		checkResetChannel();
	void		checkRouteFree();					//�ж�·��������״̬��ʱ(����&ֹͣ)
private:
	PDT::CCommInf			m_commInf;
	PDT::CDataInf			m_dataInf;
	PDT::CDacEvent			m_dacEvent;
	//CMainRouteManager		m_mainRouteMgr;
	hUInt8					m_channelState[DAC_CHANNEL_NUM];
	bool					m_channelStateFirst[DAC_CHANNEL_NUM];	//wfp add
private:
	PDT::CIntervalTimerSet	m_timerSet;
	int						m_oneSecondTimeId;		//һ�붨ʱ��
	int						m_secondCount;			//�����
	int						m_qualityCheckTimeId;	//ʵʱ�����������鶨ʱ��ID
	int						m_stateCheckTimeId;		//״̬��ⶨʱ��ID
	int						m_channelResetTimeId;	//ͨ�����ö�ʱ��ID
	hUInt32					m_channelRxBytes[DAC_CHANNEL_NUM];		//ͨ����һ�ν����ֽ��� 
	hUInt32					m_channelRecvTime[DAC_CHANNEL_NUM];		//ͨ����һ�ν�������ʱ��
	//���г�ʱ�ж�
	hInt16					m_routeFreeCount[DAC_ROUTE_NUM];		//·�������ݴ���(����ֹͣ&����״̬��-1Ϊ���ж�)
};


#endif //_FDC_STATE_TASK_H

#ifndef _DAC_STATE_TASK_H
#define _DAC_STATE_TASK_H

#include <ace/Task.h>
#include "dac/comminf.h"
#include "dac/datainf.h"
#include "dac/dacevent.h"
#include "utl/string.h"
#include "utl/intervaltimerset.h"
#include "sys/blog.h"

#define ROUTE_NO_OK_DATA_TIME	15		//路径允许没有正确的数据的最大时间(s)	
#define QUALITY_CHECK_TIME		10		//质量码检查间隔(s)

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
	hUInt8		mainRouteState(int routNo);			//校验主路径状态
	hUInt8		routeStateWithChanUp(int routeNo);	//更新路径所关联的Chan Up时的状态
	void		checkRouteScanCycle(int routeNo);	//更新路径扫描周期
	void		refreshCounter();					//计数器刷新
	int			findRouteInGroup(int groupNo,int routeState,int filterRouteNo = -1);	//查找一条指定状态的路径
	hUInt32		lastestUpdateTime(hInt32 route);
	void		checkQuality();						//检查实时数据质量码
	void		checkQualityRoute(int route);
	void		checkResetChannel();
	void		checkRouteFree();					//判定路径无数据状态超时(空闲&停止)
private:
	PDT::CCommInf			m_commInf;
	PDT::CDataInf			m_dataInf;
	PDT::CDacEvent			m_dacEvent;
	//CMainRouteManager		m_mainRouteMgr;
	hUInt8					m_channelState[DAC_CHANNEL_NUM];
	bool					m_channelStateFirst[DAC_CHANNEL_NUM];	//wfp add
private:
	PDT::CIntervalTimerSet	m_timerSet;
	int						m_oneSecondTimeId;		//一秒定时器
	int						m_secondCount;			//秒计数
	int						m_qualityCheckTimeId;	//实时数据质量码检查定时器ID
	int						m_stateCheckTimeId;		//状态监测定时器ID
	int						m_channelResetTimeId;	//通道重置定时器ID
	hUInt32					m_channelRxBytes[DAC_CHANNEL_NUM];		//通道上一次接收字节数 
	hUInt32					m_channelRecvTime[DAC_CHANNEL_NUM];		//通道上一次接收数据时间
	//空闲超时判定
	hInt16					m_routeFreeCount[DAC_ROUTE_NUM];		//路径无数据次数(包括停止&空闲状态，-1为不判定)
};


#endif //_FDC_STATE_TASK_H

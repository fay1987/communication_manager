#include "ace/OS.h"
#include "dac/dacdef.h"
#include "dac/stateformat.h"
#include "statetask.h"
#include "config.h"

using namespace PDT;

CStateTask::~CStateTask()
{

}

CStateTask::CStateTask()
{
	m_secondCount = 1;

	hUInt32 curTime = (hUInt32)ACE_OS::time(NULL);
	for (int i=0;i<DAC_CHANNEL_NUM;i++)
	{
		m_channelRxBytes[i] = 10000;	//防止通道启动时就被重置
		m_channelRecvTime[i] = curTime;
	}
}		

int CStateTask::start()
{
	logprint(LOG_DACSRV_STATETASK,"<stateTask>:创建一个线程!");
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

void CStateTask::stop()
{
	if (thr_count() > 0) 
	{
		thr_mgr()-> cancel_task(this);
		wait();
	}
}

int CStateTask::svc()
{
	m_oneSecondTimeId = m_timerSet.add(1000);	//1000ms==1s
	m_stateCheckTimeId = m_timerSet.add(300);	//300ms
	if ( CONFIG::instance()->qualityInvalidInterval() > 0 ) m_qualityCheckTimeId = m_timerSet.add(QUALITY_CHECK_TIME*1000);
	m_channelResetTimeId = m_timerSet.add(3000);//3s
	m_timerSet.start();

	ACE_OS::memset(m_channelState,0,sizeof(m_channelState));
	ACE_OS::memset(m_channelStateFirst,1,sizeof(m_channelStateFirst));
	ACE_OS::memset(m_routeFreeCount,0,sizeof(m_routeFreeCount));

	//ACE_Time_Value	tvSleep(0,10000);			//10ms
	ACE_Time_Value	tvSleep(0,100000);			//100ms
	ACE_Thread_Manager *mgr = this->thr_mgr ();
	DAC_SYSTEM_INFO* pSystemInfo = m_commInf.systemInfo();

	while(true)
	{
		if (mgr->testcancel (mgr->thr_self ()))
			break;

		ACE_OS::sleep(tvSleep); 

		if ( PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_CHANNEL)
			||PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_GROUP)
			||PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_ROUTE) )
		{
			continue;
		}

		refreshCounter();

		//检查状态
		if (m_timerSet.time_out(m_stateCheckTimeId) )
		{
			checkChannelState();	//通道状态
			checkGroupState();		//RTU状态
			checkRouteState();		//路径状态			
		}

		//实时数据质量码检查
		if ( CONFIG::instance()->qualityInvalidInterval() > 0 && m_timerSet.time_out(m_qualityCheckTimeId) )
		{
			checkQuality();
		}

		//通道重置检测
		if ( m_timerSet.time_out(m_channelResetTimeId) )
		{
			checkResetChannel();
			checkRouteFree();
		}

	}//end while

	m_timerSet.stop();
	logprint(LOG_DACSRV_STATETASK,"threadId = %d , stateTask 线程退出!",ACE_Thread::self());

	return 0;
}

void CStateTask::checkChannelState()
{
	DAC_CHANNEL*		pChannel = NULL;
	DAC_CHANNEL_INFO*	pChannelInfo = NULL;
	hUInt8			oldState,newState;

	if ( PDT_BIT_ENABLED(m_commInf.systemInfo()->loadParaFlag,LOADPARA_CHANNEL) ) return;
	if ( CONFIG::instance()->filterChannel() ) return;

	for(hUInt32 i=0;i<m_commInf.systemInfo()->channelNum;i++)
	{
		pChannel = m_commInf.channel(i,false);
		pChannelInfo = m_commInf.channelInfo(i,false);

		oldState = m_channelState[i];
		newState = pChannelInfo->state;

		//通道状态发生变化的处理
		if ( oldState != newState)
		{
			m_channelState[i] = newState;
			logprint(LOG_DACSRV_STATETASK,"Channel状态:  ChannelNo = %d , %s , %s---->%s",i,pChannel->name,
										   CStateFormat::channelStateDesc(oldState).c_str(),CStateFormat::channelStateDesc(newState).c_str());
			if ( CONFIG::instance()->filterBegin() && m_channelStateFirst[i] && (newState == CHANNEL_STATE_UP) )
			{
				m_channelStateFirst[i] = false;
			}
			else
				m_dacEvent.channelChange(i,oldState,newState);	//产生事项

		}//end if

	}//end for 
}

void CStateTask::checkGroupState()
{
	DAC_GROUP*		pGroup;
	DAC_GROUP_INFO* pGroupInfo;
	hUInt8			oldState,newState;

	if ( PDT_BIT_ENABLED(m_commInf.systemInfo()->loadParaFlag,LOADPARA_GROUP) ) return;

	for(hUInt32 i=0;i<m_commInf.systemInfo()->groupNum;i++)
	{
		pGroup = m_commInf.group(i,false);
		pGroupInfo = m_commInf.groupInfo(i,false);

		oldState = pGroupInfo->state;
		newState = pGroupInfo->state;

		//判断不存在
		if( !pGroup->valid )
			newState = GROUP_STATE_DISABLED;
		else if ( pGroupInfo->manStop )
			newState = GROUP_STATE_DOWN;
		else
		{
			//add by wlq 20150526,首先找主路径状态，如果主路径状态为非UP，这找其他路径，如果找到有UP，则将该路径设为主路径
			//首先判断主备路径，然后在判断数据组状态
			//主备路径首先获取该数据组的所有路径，从这些路径中找主路径状态，如果为UP，则将其他的UP设置为STANDBY，
			//如果当前的主路径非UP，则从其他的路径中找到UP为主，其他的设为备用，并且把其他的UP设置为STANDBY，如果没有UP则找STANDBY，并更改为UP
			//如果仍然找不到，或者通道路径数为1，则始终主路径不变；
			if(pGroupInfo->routeNum > 1)
			{
				DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(pGroupInfo->mainRoute);
				if(pRouteInfo)
				{
					if(pRouteInfo->state != ROUTE_STATE_UP)
					{
						int ok_route = findRouteInGroup(i,ROUTE_STATE_UP);
						if( ok_route == -1 ) ok_route = findRouteInGroup(i,ROUTE_STATE_STANDBY);
						if( ok_route != -1 )
						{
							pGroupInfo->mainRoute = ok_route;
							for( hUInt32 r = 0 ; r < pGroupInfo->routeNum ; r ++ )
							{
								DAC_ROUTE_INFO* pRouteOther = m_commInf.routeInfo( pGroupInfo->routes[r] );
								if( pGroupInfo->routes[r] == pGroupInfo->mainRoute )
								{
									pRouteOther->mainFlag = true;
									if(pRouteOther->state == ROUTE_STATE_STANDBY) pRouteOther->state = ROUTE_STATE_UP;
								}
								else
								{
									pRouteOther->mainFlag = false;
									if(pRouteOther->state == ROUTE_STATE_UP) pRouteOther->state = ROUTE_STATE_STANDBY;
								}
							}
						}
					}
					else
					{
						if( !pRouteInfo->mainFlag ) pRouteInfo->mainFlag = true;
						for( hUInt32 r = 0 ; r < pGroupInfo->routeNum ; r ++ )
						{
							if( pGroupInfo->routes[r] == pGroupInfo->mainRoute ) continue;
							DAC_ROUTE_INFO* pRouteOther = m_commInf.routeInfo( pGroupInfo->routes[r] );
							if( pRouteOther->mainFlag ) pRouteOther->mainFlag = false;
						}
					}
				}
			}
			//else
			//{
				DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(pGroupInfo->mainRoute);
				if(!pRouteInfo) newState = GROUP_STATE_DOWN;
				else
				{
					if( !pRouteInfo->mainFlag ) pRouteInfo->mainFlag = true;
					if(pRouteInfo->state != ROUTE_STATE_UP)
						newState = GROUP_STATE_DOWN;
					else newState = GROUP_STATE_UP;
				}
			//}
			//modify end by wlq 20150526
			/*DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(pGroupInfo->mainRoute);
			if(pRouteInfo->state != ROUTE_STATE_UP )
			{
				int ok_route = findRouteInGroup(i,ROUTE_STATE_UP);
				if (  ok_route != -1)
				{
					pGroupInfo->mainRoute = ok_route;
					pRouteInfo->mainFlag = false;
					pRouteInfo = m_commInf.routeInfo(ok_route);					
					pRouteInfo->mainFlag = true;
					newState = GROUP_STATE_UP;
					pGroupInfo->dayOkTime = pGroupInfo->dayOkTime + 1;
				}
				else 
				{
					newState = GROUP_STATE_DOWN;
					if(!pRouteInfo->mainFlag) pRouteInfo->mainFlag = true;
				}
			}
			else
			{
				if(!pRouteInfo->mainFlag) pRouteInfo->mainFlag = true;
				newState = GROUP_STATE_UP;
			}*/
			
		}

		//数据组状态发生变化的处理
		if ( oldState != newState)
		{
			//屏蔽第一次数据组启动告警 wfp add at 20110228
			if ( CONFIG::instance()->filterBegin() && pGroupInfo->lastUpTime == 0 && (newState == GROUP_STATE_UP) )
			{
				pGroupInfo->lastUpTime = 1;						//防止一直没up的告警
			}
			else if ( !CONFIG::instance()->filterGroup() )
			{
				m_dacEvent.groupChange(i,oldState,newState);	//产生事项
			}

			//nonUp to up
			if ( (oldState == GROUP_STATE_DISABLED || oldState == GROUP_STATE_DOWN)
				&&(newState == GROUP_STATE_UP) )				//wfp changed from pGroupInfo->state at 20110228
			{
				pGroupInfo->lastUpTime = (hUInt32)ACE_OS::time(0);
			}
			else if (  (oldState == GROUP_STATE_UP)				//wfp changed from pGroupInfo->state at 20110228
				&&(newState == GROUP_STATE_DISABLED || newState == GROUP_STATE_DOWN) )
			{
				pGroupInfo->lastDownTime = (hUInt32)ACE_OS::time(0);
			}

			//更新状态并设置变化数据区
			m_commInf.setGroupState(i,newState);
			logprint(LOG_DACSRV_STATETASK,"Group状态:  GroupNo = %d , %s , %s---->%s",i,pGroup->name,
					 CStateFormat::groupStateDesc(oldState).c_str(),CStateFormat::groupStateDesc(newState).c_str());

		}//end if

	}//end for 
}

void CStateTask::checkRouteState()
{
	DAC_ROUTE*		pRoute;
	DAC_ROUTE_INFO* pRouteInfo;
	DAC_CHANNEL_INFO*  pChannelInfo;
	hUInt8			oldState,newState;

	if ( PDT_BIT_ENABLED(m_commInf.systemInfo()->loadParaFlag,LOADPARA_ROUTE) ) 
		return;

	for(hUInt32 i=0;i<m_commInf.systemInfo()->routeNum;i++)
	{
		pRoute = m_commInf.route(i,false);
		pRouteInfo = m_commInf.routeInfo(i,false);

		oldState = newState = pRouteInfo->state;

		//无效路径
		if ( !m_commInf.isRouteValid(i) 
			|| !m_commInf.isGroupValid(pRoute->group) 
			|| !m_commInf.isChannelValid(pRoute->channel) )
		{
			newState = ROUTE_STATE_DOWN;	//ROUTE_STATE_DISABLE;
		}
		//有效路径
		else
		{
			if (pRouteInfo->manStop == true)	//人工停止
			{
				newState = ROUTE_STATE_DISABLED;
			}
			else								//自动判断
			{
				pChannelInfo = m_commInf.channelInfo(pRoute->channel);

				if ( pChannelInfo->state == CHANNEL_STATE_UP ) //关联的通道处于连接状态
				{
					newState = routeStateWithChanUp(i);
				}
				else								  //关联的通道处于未连接状态
				{
					newState = ROUTE_STATE_DOWN;	
				}
			}
		}//end else

		//判断状态是否改变
		if( oldState != newState )
		{
			//屏蔽第一次路径启动告警 wfp add at 20110228
			if ( CONFIG::instance()->filterBegin() && pRouteInfo->lastUpTime == 0 
				 && (newState == ROUTE_STATE_UP || newState == ROUTE_STATE_STANDBY) )
			{
				pRouteInfo->lastUpTime = 1;						//防止一直没up的告警
			}
			else if ( !CONFIG::instance()->filterRoute() )
			{
				m_dacEvent.routeChange(i,oldState,newState);	//产生事项
			}
			//停止-->启动状态
			if ( (oldState == ROUTE_STATE_DISABLED || oldState == ROUTE_STATE_DOWN || oldState == ROUTE_STATE_FREE)
				&&(newState == ROUTE_STATE_UP || newState == ROUTE_STATE_STANDBY) )
			{
				pRouteInfo->lastUpTime = (hUInt32)ACE_OS::time(0);
			}
			//启动-->停止状态
			else if ( (oldState == ROUTE_STATE_UP || oldState == ROUTE_STATE_STANDBY)
				&&(newState == ROUTE_STATE_DISABLED || newState == ROUTE_STATE_DOWN || newState == ROUTE_STATE_FREE) )
			{
				pRouteInfo->lastDownTime = (hUInt32)ACE_OS::time(0);
			}

			m_commInf.setRouteState(i,newState);	//更新路径状态并设置变化区

			//产生报警
			logprint(LOG_DACSRV_STATETASK,"Route状态:  RouteNo = %d , %s , %s---->%s\n",i,pRoute->name,
					 CStateFormat::routeStateDesc(oldState).c_str(),CStateFormat::routeStateDesc(newState).c_str());

		}

		checkRouteScanCycle(i);		//设置标志

	} //end for
}

void CStateTask::checkRouteScanCycle(int routeNo)
{
	DAC_ROUTE* pRoute = m_commInf.route(routeNo);
	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(routeNo);
	if (pRoute == NULL || pRouteInfo == NULL) return ;

	//if ( pRoute->timeSyncFlag == true )
	{
		if( pRoute->scanInterval.timesync > 0 && m_secondCount%(pRoute->scanInterval.timesync)== 0 )
			pRouteInfo->cmd.timesync = true;
	}
	//else 
	//	pRouteInfo->cmd.timesync = false;

	if( pRoute->scanInterval.all_data > 0 &&m_secondCount%(pRoute->scanInterval.all_data) == 0 )		
		pRouteInfo->cmd.all_data = true;
	if( pRoute->scanInterval.change_data > 0 && m_secondCount%(pRoute->scanInterval.change_data) == 0 )		
		pRouteInfo->cmd.change_data = true;
	if( pRoute->scanInterval.all_yc > 0 && m_secondCount%(pRoute->scanInterval.all_yc) == 0 )		
		pRouteInfo->cmd.all_yc = true;
	if( pRoute->scanInterval.all_yx > 0 && m_secondCount%(pRoute->scanInterval.all_yx) == 0 )			
		pRouteInfo->cmd.all_yx = true;
	if( pRoute->scanInterval.all_kwh > 0 && m_secondCount%(pRoute->scanInterval.all_kwh) == 0 )		
		pRouteInfo->cmd.all_kwh = true;
	if( pRoute->scanInterval.soe > 0 && m_secondCount%(pRoute->scanInterval.soe) == 0 )	
		pRouteInfo->cmd.soe = true;
	if( pRoute->scanInterval.change_yc > 0 && m_secondCount%(pRoute->scanInterval.change_yc) == 0 )
		pRouteInfo->cmd.change_yc = true;
	if( pRoute->scanInterval.change_yx > 0 && m_secondCount%(pRoute->scanInterval.change_yx) == 0 )	
		pRouteInfo->cmd.change_yx = true;
	if( pRoute->scanInterval.change_kwh > 0 && m_secondCount%(pRoute->scanInterval.change_kwh) == 0 )	
		pRouteInfo->cmd.change_kwh = true;
}

/*
* 函数功能:检查主路径的状态
* 函数返回:路径状态
*/
hUInt8 CStateTask::mainRouteState(int routNo)
{
	DAC_ROUTE* pRoute = m_commInf.route(routNo);
	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(routNo);
	if (pRoute == NULL || pRouteInfo == NULL) return ROUTE_STATE_DOWN;

	hUInt32	okCount,errCount;	//正确,错误帧计数
	hFloat  errRate = 0;;		//误码率
	bool	isErrRateOk = true;	//标识误码率是否正常
	hUInt8	newState = pRouteInfo->state;

	//计算误码率
	okCount = pRouteInfo->okFrmCnt;
	errCount = pRouteInfo->errFrmCnt;

	if(okCount + errCount > 0)
	{
		errRate = (hFloat)errCount/(okCount+errCount) * 100;
		if( errRate >= pRoute->errRateLimit )
		{
			//高误码率报警
			pRouteInfo->errTime = pRouteInfo->errTime + 1;

			if ( pRouteInfo->errTime >= pRoute->errTimeLimit )	//高误码率持续时间过长
			{
				//高误码率持续时间过长报警
				isErrRateOk = false;
			}
		}
		else
			pRouteInfo->errTime = 0;
	}

	if ( isErrRateOk )
	{
		if( newState <  ROUTE_STATE_UP || newState > ROUTE_STATE_STANDBY )
			newState = ROUTE_STATE_UP;		//误码率OK，则为UP
	}
	else
	{
		newState = ROUTE_STATE_FREE;	//误码率ERR，则为FREE(空闲)
	}

	return newState;
}

/*
* 函数功能:获取channel连接状态下的路径状态
* 函数返回:路径状态
*/
hUInt8 CStateTask::routeStateWithChanUp(int routeNo)
{
	DAC_ROUTE* pRoute = m_commInf.route(routeNo);
	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(routeNo);
	if ( pRoute == NULL || pRouteInfo == NULL ) return ROUTE_STATE_DISABLED;

	hUInt8 newState = pRouteInfo->state;

	if ( pRouteInfo->state == ROUTE_STATE_DISABLED || pRouteInfo->state == ROUTE_STATE_DOWN )	//由未连接到连接
		newState = ROUTE_STATE_FREE;

	//转发路径
	if ( pRoute->type == ROUTE_TYPE_TX )
	{
		newState = ROUTE_STATE_UP;
		return newState;
	}

	DAC_CHANNEL *pChannel = m_commInf.channel(pRoute->channel);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(pRoute->channel);
	if ( pChannel == NULL || pChannelInfo == NULL ) return ROUTE_STATE_DISABLED;

	//无正确数据时间超出
	if ( pChannel->timeout != 0
		&&(time(0) - pRouteInfo->lastDataOkTime) >= pChannel->timeout  /*&& newState == ROUTE_STATE_UP*/)//ROUTE_NO_OK_DATA_TIME )	 //wfp changed
	{
		//通讯故障报警
		logprint(LOG_DACSRV_STATETASK,"Route状态:通讯故障报警  RouteNo = %d , %s , ---->%s\n",routeNo,pRoute->name,
					 CStateFormat::routeStateDesc(newState).c_str());
		newState = ROUTE_STATE_FREE;
		
		//add by yaoff on 20170922

		//处理虚拟遥信
		DAC_GROUP* pGroup = m_commInf.group(pRoute->group);
		int yxno = -1;
		DAC_YX *pYx = NULL;
		for (int i = 0; i < pGroup->yxNum; i++)
		{
			pYx = m_dataInf.yxPara(pRoute->group, i);
			if (pYx->type == 1)
			{
				yxno = i;
				pYx = m_dataInf.yxPara(pRoute->group, i);
			}
		}
		
		DAC_YX_DATA data;
		if (pYx && pYx->calc && m_dataInf.yxMainRoute(pRoute->group, yxno, data))
		{
			//通讯故障，设置数据为无效
			if (pRouteInfo->state != ROUTE_STATE_FREE)
			{
				checkQualityRoute(routeNo);
			}

			data.val = 0;
			data.quality = 0;
			data.updateTime = time(0);
			m_dataInf.setYx(routeNo, yxno, data);
		}
		//add end.
		return newState;
	}
	else
	{
		logprint(LOG_DACSRV_STATETASK,"yx=1  RouteNo = %d , %s , ---->%s\n",routeNo,pRoute->name,
			CStateFormat::routeStateDesc(newState).c_str());
		//处理虚拟遥信
		DAC_GROUP* pGroup = m_commInf.group(pRoute->group);
		int yxno = -1;
		DAC_YX *pYx = NULL;
		for (int i = 0; i < pGroup->yxNum; i++)
		{
			pYx = m_dataInf.yxPara(pRoute->group, i);
			if (pYx->type == 1)
			{
				yxno = i;
				pYx = m_dataInf.yxPara(pRoute->group, i);
			}
		}
		DAC_YX_DATA data;
		if (pYx && pYx->calc && m_dataInf.yxMainRoute(pRoute->group, yxno, data))
		{
			data.val = 1;
			data.quality = 0;
			data.updateTime = time(0);
			m_dataInf.setYx(routeNo, yxno, data);
		}
		//add end.
	}

	//wfp new add

	//modify by wlq 20150526
	newState = mainRouteState(routeNo);
	
	//有数据流通
	//if ( pRouteInfo->mainFlag == true )	//本路径是主路径
	//{
	//	newState = mainRouteState(routeNo);
	//}
	//else									//本路径是备路径
	//{	
	//	//清空误码信息
	//	//pRouteInfo->okFrmCnt = 0;
	//	//pRouteInfo->errFrmCnt = 0;

	//	newState = pRouteInfo->state; //ROUTE_STATE_FREE;

	//	//若本路径所在组中的主路径是否已经UP,则为standby
	//	DAC_ROUTE_INFO* pMainRouteInfo;
	//	DAC_GROUP_INFO* pGroupInfo = m_commInf.groupInfo(pRoute->group);
	//	if (pGroupInfo != NULL)
	//	{
	//		pMainRouteInfo = m_commInf.routeInfo(pGroupInfo->mainRoute);
	//		if (pMainRouteInfo != NULL)
	//		{
	//			if (pMainRouteInfo->state == ROUTE_STATE_UP)
	//				newState = ROUTE_STATE_STANDBY;
	//		}
	//	}//end if

	//}//end else	
	

	return newState;
}

/*
* 函数功能:查找指定组中指定状态的路径
* 函数输入:groupNo--数据组号; state--需要查找的路径状态; filterRouteNo--需要过滤的路径号(默认-1为不过滤)
* 函数返回:路径号
*/
int CStateTask::findRouteInGroup(int groupNo,int routeState,int filterRouteNo /* = -1 */)
{
	DAC_GROUP_INFO* pGroupInfo = m_commInf.groupInfo(groupNo);
	if (pGroupInfo == NULL) return -1;

	DAC_ROUTE* pRoute = NULL;
	DAC_ROUTE_INFO* pRouteInfo = NULL;

	for (hUInt32 i=0;i<pGroupInfo->routeNum;i++)
	{
		pRoute = m_commInf.route(pGroupInfo->routes[i]);
		pRouteInfo = m_commInf.routeInfo(pGroupInfo->routes[i]);

		//路径无效,则寻找下一条
		if (pRoute == NULL || pRouteInfo == NULL) continue;

		if (filterRouteNo != -1)
		{
			//过滤本路径
			if( pGroupInfo->routes[i] == filterRouteNo ) continue;
		}

		if(pRouteInfo->state == routeState)
		{
			return pGroupInfo->routes[i];	
		}

	}//end for

	return -1;
}

/*
* 函数功能:更新秒计数器
*/
void CStateTask::refreshCounter()
{
	if ( m_timerSet.time_out(m_oneSecondTimeId) )
	{
		if (m_secondCount >= INT_MAX/2) m_secondCount = 0;

		++m_secondCount;
	}
}

void CStateTask::checkQuality()
{
	DAC_ROUTE*		pRoute = NULL;
	DAC_ROUTE_INFO* pRouteInfo = NULL;
	DAC_GROUP *pGroup = NULL;
	hUInt32 quality = 0;
	hUInt32 invalidInterval = CONFIG::instance()->qualityInvalidInterval();	//质量码无效间隔
	if ( invalidInterval == 0 ) return;

	if ( PDT_BIT_ENABLED(m_commInf.systemInfo()->loadParaFlag,LOADPARA_ROUTE)
		||PDT_BIT_ENABLED(m_commInf.systemInfo()->loadParaFlag,LOADPARA_GROUP)
		||PDT_BIT_ENABLED(m_commInf.systemInfo()->loadParaFlag,LOADPARA_YC)
		||PDT_BIT_ENABLED(m_commInf.systemInfo()->loadParaFlag,LOADPARA_YX)
		||PDT_BIT_ENABLED(m_commInf.systemInfo()->loadParaFlag,LOADPARA_KWH)) 
		return;

	hUInt32 curTime = ACE_OS::time(NULL);

	DAC_YC_DATA ycData;
	DAC_YX_DATA yxData;
	DAC_KWH_DATA kwhData;
	for(hUInt32 i=0;i<m_commInf.systemInfo()->routeNum;i++)
	{
		pRoute = m_commInf.route(i);
		pRouteInfo = m_commInf.routeInfo(i);
		if ( pRoute == NULL || pRouteInfo == NULL ) continue;

		pGroup = m_commInf.group(pRoute->group);
		if ( pGroup == NULL ) continue;

		//置遥测数据质量码无效
		for (hUInt32 ycPointNo=0;ycPointNo<pGroup->ycNum;ycPointNo++)
		{
			if ( !m_dataInf.yc(i,ycPointNo,ycData) ) continue;

			if ( curTime - ycData.updateTime > invalidInterval )
			{
				quality = ycData.quality;
				if ( PDT_BIT_ENABLED(quality,QUALITY_BIT_IV) ) continue;

				PDT_SET_BITS(quality,QUALITY_BIT_IV);
				ycData.quality = quality;
				ycData.updateTime = 0;
				m_dataInf.setYc(i,ycPointNo,ycData,false);
			}

		}//end for yc

		//置遥信数据质量码无效
		for (hUInt32 yxPointNo=0;yxPointNo<pGroup->yxNum;yxPointNo++)
		{
			if ( !m_dataInf.yx(i,yxPointNo,yxData) ) continue;

			if ( curTime - yxData.updateTime > invalidInterval )
			{
				quality = yxData.quality;
				if ( PDT_BIT_ENABLED(quality,QUALITY_BIT_IV) ) continue;

				PDT_SET_BITS(quality,QUALITY_BIT_IV);
				yxData.quality = quality;
				yxData.updateTime = 0;
				m_dataInf.setYx(i,yxPointNo,yxData,false);
			}

		}//end for yx

		//置电度数据质量码无效
		for (hUInt32 kwhPointNo=0;kwhPointNo<pGroup->kwhNum;kwhPointNo++)
		{
			if ( !m_dataInf.kwh(i,kwhPointNo,kwhData) ) continue;

			if ( curTime - kwhData.updateTime > invalidInterval )
			{
				quality = kwhData.quality;
				if ( PDT_BIT_ENABLED(quality,QUALITY_BIT_IV) ) continue;

				PDT_SET_BITS(quality,QUALITY_BIT_IV);
				kwhData.quality = quality;
				kwhData.updateTime = 0;
				m_dataInf.setKwh(i,kwhPointNo,kwhData,false);
			}
		}//end for kwh

	}//end for

}


void CStateTask::checkQualityRoute(int routeno)
{
	DAC_ROUTE*		pRoute = NULL;
	DAC_GROUP*		pGroup = NULL;
	hUInt32 quality = 0;

	DAC_YC_DATA ycData;
	DAC_YX_DATA yxData;
	DAC_KWH_DATA kwhData;

	pRoute = m_commInf.route(routeno);
	if ( pRoute == NULL) return;

	pGroup = m_commInf.group(pRoute->group);
	if ( pGroup == NULL ) return;

	//置遥测数据质量码无效
	for (hUInt32 ycPointNo=0;ycPointNo<pGroup->ycNum;ycPointNo++)
	{
		if ( !m_dataInf.yc(routeno,ycPointNo,ycData) ) continue;

		quality = ycData.quality;
		if ( PDT_BIT_ENABLED(quality,QUALITY_BIT_IV) ) continue;

		PDT_SET_BITS(quality,QUALITY_BIT_IV);
		ycData.quality = quality;
		ycData.updateTime = 0;
		m_dataInf.setYc(routeno,ycPointNo,ycData,false);
		

	}//end for yc

	//置遥信数据质量码无效
	for (hUInt32 yxPointNo=0;yxPointNo<pGroup->yxNum;yxPointNo++)
	{
		if ( !m_dataInf.yx(routeno,yxPointNo,yxData) ) continue;

		quality = yxData.quality;
		if ( PDT_BIT_ENABLED(quality,QUALITY_BIT_IV) ) continue;

		PDT_SET_BITS(quality,QUALITY_BIT_IV);
		yxData.quality = quality;
		yxData.updateTime = 0;
		m_dataInf.setYx(routeno,yxPointNo,yxData,false);

	}//end for yx

	//置电度数据质量码无效
	for (hUInt32 kwhPointNo=0;kwhPointNo<pGroup->kwhNum;kwhPointNo++)
	{
		if ( !m_dataInf.kwh(routeno,kwhPointNo,kwhData) ) continue;

		quality = kwhData.quality;
		if ( PDT_BIT_ENABLED(quality,QUALITY_BIT_IV) ) continue;

		PDT_SET_BITS(quality,QUALITY_BIT_IV);
		kwhData.quality = quality;
		kwhData.updateTime = 0;
		m_dataInf.setKwh(routeno,kwhPointNo,kwhData,false);
	}//end for kwh

}

/*
 * 函数说明：通道无接收数据时长超channel.timeout则重置该通道，以防止沉默对端问题.
 * 函数改进: 后期可考虑dac_channel_info增加上一次接收数据时间，以提高判断的准确性.
 */
void CStateTask::checkResetChannel()
{
	DAC_SYSTEM_INFO* pSystemInfo = m_commInf.systemInfo();
	if ( pSystemInfo == NULL ) return;
	hUInt32 curTime = (hUInt32)ACE_OS::time(NULL);

	DAC_CHANNEL* pChannel = m_commInf.channel(0,false);
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(0,false);
	for (hUInt32 i=0;i<pSystemInfo->channelNum;i++,pChannel++,pChannelInfo++)
	{
		if ( !pChannel->valid ) continue;
		if ( !pChannel->timeout ) continue;
		if ( pChannelInfo->state != CHANNEL_STATE_UP ) continue;

		if ( m_channelRxBytes[i] != pChannelInfo->rxByteCnt )
		{
			m_channelRxBytes[i] = pChannelInfo->rxByteCnt;
			m_channelRecvTime[i] = curTime;
		}

		if ( curTime - m_channelRecvTime[i] > pChannel->timeout )
		{
			switch (pChannel->type)
			{
			case CHANNEL_TYPE_SERIAL:
			case CHANNEL_TYPE_SERIALWIRELESS:
				pChannelInfo->reset = CHANNEL_TASK_SERIAL;
				break;
			case CHANNEL_TYPE_TCPCLIENT:
				pChannelInfo->reset = CHANNEL_TASK_CLIENT;
				break;
			case CHANNEL_TYPE_TCPSERVER:
				pChannelInfo->reset = CHANNEL_TASK_SERVER;
				break;
			case CHANNEL_TYPE_UDP:
				pChannelInfo->reset = CHANNEL_TASK_UDP;
				break;
			default:
				break;
			}//end switch

			m_channelRxBytes[i] = 10000;
			m_channelRecvTime[i] = curTime;
		}
			
	}//end for

}

/*
* 函数说明：路径长时间处于无数据状态(空闲&停止)时，报告一次路径状态，以提醒用户设备数据中断.
*/
void CStateTask::checkRouteFree()
{
	if ( CONFIG::instance()->filterRoute() ) return;

	DAC_SYSTEM_INFO* pSystemInfo = m_commInf.systemInfo();
	if ( pSystemInfo == NULL ) return;

	DAC_CHANNEL* pChannel = NULL;
	DAC_ROUTE* pRoute = m_commInf.route(0,false);
	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(0,false);
	for (hUInt32 i=0;i<pSystemInfo->routeNum;i++,pRoute++,pRouteInfo++)
	{
		if ( !pRoute->valid ) continue;

		pChannel = m_commInf.channel(pRoute->channel);
		if ( !pChannel || !pChannel->timeout || pChannel->type == CHANNEL_TYPE_VIRTUAL ) continue;

		if ( (pRouteInfo->state != ROUTE_STATE_FREE) && (pRouteInfo->state != ROUTE_STATE_DOWN) ) 
		{
			m_routeFreeCount[i] = 0;
			continue;
		}
		
		//已报出过空闲，且状态未变化，则不告
		if ( m_routeFreeCount[i] == -1 ) continue;

		++m_routeFreeCount[i];
		if ( m_routeFreeCount[i] > 200 )		//与调度周期相关:600s=10min
		{
			m_routeFreeCount[i] = -1;

			//产生报警
			logprint(LOG_DACSRV_STATETASK,"Route状态:  RouteNo = %d , %s , %s---->%s\n",i,pRoute->name,
										   CStateFormat::routeStateDesc(pRouteInfo->state).c_str(),CStateFormat::routeStateDesc(pRouteInfo->state).c_str());
			m_dacEvent.routeChange(i,pRouteInfo->state,pRouteInfo->state);	//产生事项
		}

	}//end for

}

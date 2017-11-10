/*==============================================================================
* 文件名称 : dbsavetask.cpp
* 模块 : 数据库存盘任务
* 创建时间 : 2009-07-22 15:55:23
* 作者 : wfp
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "limittask.h"
#include <ace/OS.h>
#include <ace/CDR_Stream.h>
#include "sys/blog.h"
#include "scddef.h"
#include "dac/dac.h"
#include "dac/dacdef.h"
#include "event/type.h"

using namespace PDT;

CLimitTask::CLimitTask()
{
	m_isLimitTypeChange = false;
}

CLimitTask::~CLimitTask()
{

}

bool CLimitTask::init()
{
	return true;
}

int CLimitTask::start()
{
	logprint( LOG_SCD_LIMIT_TASK,"<CLimitTask>:创建一个线程!" );
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

void CLimitTask::stop()
{
	if (thr_count() > 0) 
	{
		thr_mgr()-> cancel_task(this);
		wait();
	}
}

int CLimitTask::svc()
{
	m_timeId = m_timer.add(1000);		//1000ms
	m_timer.start();

	ACE_Time_Value tv(0,10000);			//10ms
	while ( true )
	{
		if ( thr_mgr()->testcancel( thr_mgr()->thr_self() ) )
			break;
		
		ACE_OS::sleep( tv );

		if ( m_timer.time_out(m_timeId) )
		{
			ycCheck();
			yxEvent();
		}


	}//end while
	return 0;
}

void CLimitTask::ycCheck()
{
	m_curCycleTime = (hUInt32)ACE_OS::time(0);

	DAC_ROUTE* pRoute = NULL;
	DAC_YC_CHG_DATA chgData;
	for (hUInt32 route=0;route<m_commInf.systemInfo()->routeNum;route++)
	{
		pRoute = m_commInf.route(route);
		if ( pRoute == NULL ) continue;

		while ( m_dataInf.chgYc(route,chgData) )
		{
			m_isLimitTypeChange = false;
			overLimitCheck(pRoute->group,chgData);
			overLimitEventProc(pRoute->group,chgData);
		}
	}//end for
}

/*************************************************************
void  CLimitTask::overLimitCheck(void)
函数功能：越限检查
变量：	  无
处理：    根据上下限值进行越限检查
返回：	  无
说明：	  只对有变化的数据进行越限处理。（若数据无变化，越限级别也不变）
**************************************************************/
void  CLimitTask::overLimitCheck(hInt32 group,const DAC_YC_CHG_DATA& chgData)
{
	DAC_YC* pPara = m_dataInf.ycPara(group,chgData.no);
	if ( pPara == NULL || !pPara->valid ) return;
	if ( !pPara->isEvent ) return;

	//零值
	/*wfp masked at 20101008(取消上下限判定时的零值过滤)
	if( PDT_ISZERO(chgData.data.val) ) 
	{
		if( pPara->limitType != YC_OVLIMIT_NORMAL ) 
		{
			m_isLimitTypeChange = true;
			pPara->limitType = YC_OVLIMIT_NORMAL;
			pPara->eventStartTime = m_curCycleTime;
		}
		return;	//零值数据认为线路停运,不处理越限
	}
	*/

	//********过滤掉相邻两次的越限级别相同的情况********START
	if( pPara->limitType == YC_OVLIMIT_NORMAL ) //上次的越限级别为“正常”；
	{
		if( chgData.data.val >= pPara->lowLimit && chgData.data.val <= pPara->highLimit ) return;
		//(NORMAL-->LOW、HIGH、LOWLOW、HIGHHIGH)须处理。
	}
	else if( pPara->limitType == YC_OVLIMIT_HIGH )  //上次的越限级别为“越上限”；
	{
		//两级处理时，HIGH-->NORMAL、HIGHHIGH、LOW、LOWLOW），须处理。
		//一级处理时，HIGH-->NORMAL,LOW须处理。
		if ( chgData.data.val > pPara->highLimit 
			&& chgData.data.val <= pPara->hihiLimit ) return;
	}
	else if( pPara->limitType == YC_OVLIMIT_LOW ) //上次的越限级别为“越下限”；
	{
		//两级处理时，LOW-->NORMAL,HIGH,HIGHHIGH,LOWLOW须处理。
		//一级处理时，LOW-->NORMAL,HIGH须处理
		if ( chgData.data.val < pPara->lowLimit
			&& chgData.data.val >= pPara->lolowLimit ) return;
	}
	else if( pPara->limitType == YC_OVLIMIT_HIHI )
	{
		//HIGHHIGH-->HIGH,NORMAL,LOW,LOWLOW须处理。
		if ( chgData.data.val > pPara->hihiLimit ) return;
	}
	else if( pPara->limitType==YC_OVLIMIT_LOLO ) 
	{
		//LOWLOW-->LOW,NORMAL,HIGH,HIGHHIGH须处理
		if ( chgData.data.val < pPara->lolowLimit ) return;
	}
	//********过滤掉相邻两次的越限级别相同的情况********ENDS

	//越限级别有了变化（包括越限恢复的情况），须置越限报警标志。
	m_isLimitTypeChange = true;

	//更新变化了越限级别-------START
	//当前属HIGH，LOW则置越限标志，越限级别；否则属越限恢复。
	if( chgData.data.val > pPara->highLimit )
		pPara->limitType = YC_OVLIMIT_HIGH;
	else if( chgData.data.val < pPara->lowLimit )
		pPara->limitType = YC_OVLIMIT_LOW;
	else   
	{
		pPara->limitType = YC_OVLIMIT_NORMAL;
	}
	pPara->eventStartTime = m_curCycleTime;

	//要求处理两级越限时
	if ( chgData.data.val > pPara->hihiLimit ) 
		pPara->limitType = YC_OVLIMIT_HIHI;
	else if ( chgData.data.val < pPara->lolowLimit )
		pPara->limitType = YC_OVLIMIT_LOLO;

	//更新变化了越限级别-------END
	return ;
}

//处理越限事项：若越限，则产生适当的越限事项，反之亦然。
void  CLimitTask::overLimitEventProc(hInt32 group,const DAC_YC_CHG_DATA& chgData)
{
	DAC_YC* pPara = m_dataInf.ycPara(group,chgData.no);
	if ( pPara == NULL || !pPara->valid ) return;

	if( m_isLimitTypeChange ) //越限级别发生变化（含越限恢复）
	{
		if( pPara->limitType >= YC_OVLIMIT_HIHI )	//其他--->HIHI,LOLO，须立即产生事项。
		{
			pPara->hasPutEvent = true;				//生成越限事项，置事项已产生标志，返回
			createYcEvent(group,pPara->limitType,chgData);
			return;
		}

		if( pPara->limitType > YC_OVLIMIT_NORMAL ) 
			//(1)NORMAL->HI,LO(2)LO->HI, HI->LO, HIHI->HI,LO, LOLO->HI,LO
		{
			//越限级别变为HI，或LO，若没有延时要求，直接生成事项，并置标志，返回。
			//越限级别变为HI，或LO，若上次的事项已生成，本次因级别发生了变化，仍须重新报警。
			//（若越限后，已报警，其后发生的越限级别变化包括恢复正常，应予以随时跟踪报警）
			if( pPara->hasPutEvent || pPara->eventDelay == 0 )
			{
				pPara->hasPutEvent = true;
				createYcEvent(group,pPara->limitType,chgData);
				return;
			}
		}
		else if( pPara->limitType == YC_OVLIMIT_NORMAL ) 
		{
			//本次越限恢复时，若上次越限事项已生成，则须生成恢复事项，然后清事项已生成标志，返回。
			//若上次越限因延时而未生成事项，本次恢复，则不须生成事项，清事项已生成标志，直接返回。
			if( pPara->hasPutEvent || pPara->eventDelay == 0 )	//wfp changed at 20110801
			{
				createYcEvent(group,pPara->limitType,chgData);
			}
			pPara->hasPutEvent = true;
			return;
		}
	}//end if m_isLimitTypeChange

	//剩余的情况为：
	//1、相邻两次越同一类型限（有延时上次有事项, 有延时上次无事项,无延时上次有事项,无延时上次无事项）	
	//2、或两次均正常
	//3、正常-->越(上下)限,有延时要求
	//4  越限(因延时未生成事项)->越不同类型(上下)限
	//情况2可排除, 
	if( pPara->limitType > YC_OVLIMIT_NORMAL && pPara->eventDelay > 0 ) 
	{
		if( !pPara->hasPutEvent ) //上次事项尚未生成: 1有无，3、4
			//（3、上次正常，1、4、或越限但延时未满），本次越限，须判断。
			//若上次事项已生成1、4（上次为越限），本次仍为越限，且有延时要求，则不须再生成事项，直到
			//越限恢复，才报事项
		{
			if ( m_curCycleTime - pPara->eventStartTime > pPara->eventDelay )	//已超过延时时间
			{
				pPara->hasPutEvent = true;
				createYcEvent(group,pPara->limitType,chgData);
				pPara->eventStartTime = m_curCycleTime;
			}
		}
	}//end if
}

bool CLimitTask::createYcEvent(hInt32 group,hInt32 limitType,const DAC_YC_CHG_DATA& chgData)
{
	DAC_YC* pPara = m_dataInf.ycPara(group,chgData.no);
	if ( pPara == NULL || !pPara->valid ) return false;

	DAC_GROUP* pGroup = m_commInf.group(group);
	if ( pGroup == NULL ) return false;

	DAC_STATION* pStation = m_commInf.station(pGroup->station);

	nSysEvent	sysEvent;
	ACE_OS::memset(&sysEvent,0,sizeof(nSysEvent));
	sysEvent.m_eventSort = EVENT_SORT_YC;
	sysEvent.m_eventType = EVENT_YC;
	if ( pStation != NULL)
	{
		ACE_OS::strsncpy(sysEvent.m_stationCode,pStation->code,SHORTNAME_LEN);
		ACE_OS::strsncpy(sysEvent.m_stationName,pStation->name,SHORTNAME_LEN);
	}
	ACE_OS::strsncpy(sysEvent.m_code,pPara->code,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_name,pPara->name,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_parentCode,pGroup->code,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_parentName,pGroup->name,SHORTNAME_LEN);

	CDateTime dateTime = CDateTime::currentDateTime();
	sysEvent.m_ymd = dateTime.year()*100*100 + dateTime.month()*100 + dateTime.day();
	sysEvent.m_hmsms= dateTime.hour()*100*100*1000 + dateTime.minute()*100*1000 + dateTime.second()*1000 + dateTime.microsec()/1000;

	sysEvent.m_security = pPara->eventLevel;
	sysEvent.m_isRevert	= false;
	sysEvent.PrintOut = 1;
	sysEvent.SmsOut = 0;
	sysEvent.m_floatValue = chgData.data.val;
	sysEvent.m_floatValue1 = chgData.lastVal;

	//wfp masked at 20110616-去掉厂站、数据组描述
	if ( limitType == YC_OVLIMIT_NORMAL )
	{//越限恢复
		sysEvent.m_isRevert = true;
		ACE_OS::sprintf(sysEvent.m_charDesc,"%s 越限恢复: 从 %.3f %s 恢复到 %.3f %s",pPara->name,
											(hFloat)chgData.lastVal,pPara->unit,(hFloat)chgData.data.val,pPara->unit);
	}
	else if ( limitType == YC_OVLIMIT_HIGH )
	{//上限
		sysEvent.SmsOut = pPara->isMsgAlarm;
		ACE_OS::sprintf(sysEvent.m_charDesc,"%s 越上限: 从 %.3f %s 到 %.3f %s",pPara->name,
											(hFloat)chgData.lastVal,pPara->unit,(hFloat)chgData.data.val,pPara->unit);
	}
	else if ( limitType == YC_OVLIMIT_LOW )
	{
		sysEvent.SmsOut = pPara->isMsgAlarm;
		ACE_OS::sprintf(sysEvent.m_charDesc,"%s 越下限: 从 %.3f %s 到 %.3f %s",pPara->name,
											(hFloat)chgData.lastVal,pPara->unit,(hFloat)chgData.data.val,pPara->unit);
	}
	else if ( limitType == YC_OVLIMIT_HIHI )
	{
		sysEvent.SmsOut = pPara->isMsgAlarm;
		sysEvent.m_security += 1;
		sysEvent.m_eventType = EVENT_YC + 1;	//wfp add at 20110916
		ACE_OS::sprintf(sysEvent.m_charDesc,"%s 越上上限: 从 %.3f %s 到 %.3f %s",pPara->name,
											(hFloat)chgData.lastVal,pPara->unit,(hFloat)chgData.data.val,pPara->unit);
	}
	else if ( limitType == YC_OVLIMIT_LOLO )
	{
		sysEvent.SmsOut = pPara->isMsgAlarm;
		sysEvent.m_security += 1;
		sysEvent.m_eventType = EVENT_YC + 1;	//wfp add at 20110916
		ACE_OS::sprintf(sysEvent.m_charDesc,"%s 越下下限: 从 %.3f %s 到 %.3f %s",pPara->name,
											(hFloat)chgData.lastVal,pPara->unit,(hFloat)chgData.data.val,pPara->unit);
	}

	return (bool)m_event.insertEvent(sysEvent);
}

void CLimitTask::yxEvent()
{
	DAC_ROUTE* pRoute = NULL;
	DAC_YX_CHG_DATA chgData;
	DAC_YX* pPara = NULL;

	for (hUInt32 route=0;route<m_commInf.systemInfo()->routeNum;route++)
	{
		pRoute = m_commInf.route(route);
		if ( pRoute == NULL ) continue;

		while ( m_dataInf.chgYx(route,chgData) )
		{ 
			pPara = m_dataInf.yxPara(pRoute->group,chgData.no);
			if ( pPara == NULL || !pPara->valid ) continue;
			if ( !pPara->isEvent ) continue;

			/*wfp masked at 20110622
			//第一次收到与正常状态不同的值，则告警
			if ( PDT_BIT_ENABLED((hUInt32)chgData.data.quality,QUALITY_BIT_INIT) )
			{
				if ( chgData.data.val != pPara->normal )				//与正常状态不同
					createYxEvent(pRoute->group,chgData);
				continue;
			}
			*/

			if ( PDT_BIT_ENABLED((hUInt32)chgData.data.quality,QUALITY_BIT_INIT) )
			{
				if ( chgData.data.val != pPara->normal )				//与正常状态不同
					createYxEvent(pRoute->group,chgData);
				else
				{
					//目前系统初始值为0，因此若正常值配置为非0，则第一次的事项会比较多
					if ( chgData.lastVal != pPara->normal )			//本次正常,上次异常
						createYxEvent(pRoute->group,chgData);
				}
				continue;
			}
			
			//非第一次
			if ( chgData.data.val == chgData.lastVal ) continue;	//值没有变化，不产生事项
			createYxEvent(pRoute->group,chgData);
		}//end while

	}//end for
}

//遥信事项
bool CLimitTask::createYxEvent(hInt32 group,const DAC_YX_CHG_DATA& chgData)
{
	DAC_YX* pPara = m_dataInf.yxPara(group,chgData.no);
	if ( pPara == NULL || !pPara->valid ) return false;

	DAC_GROUP* pGroup = m_commInf.group(group);
	if ( pGroup == NULL ) return false;

	DAC_STATION* pStation = m_commInf.station(pGroup->station);

	nSysEvent	sysEvent;
	ACE_OS::memset(&sysEvent,0,sizeof(nSysEvent));
	sysEvent.m_eventSort = EVENT_SORT_YX;
	sysEvent.m_eventType = pPara->eventType;
	if ( pStation != NULL)
	{
		ACE_OS::strsncpy(sysEvent.m_stationCode,pStation->code,SHORTNAME_LEN);
		ACE_OS::strsncpy(sysEvent.m_stationName,pStation->name,SHORTNAME_LEN);
	}
		
	ACE_OS::strsncpy(sysEvent.m_code,pPara->code,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_name,pPara->name,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_parentCode,pGroup->code,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_parentName,pGroup->name,SHORTNAME_LEN);

	CDateTime dateTime = CDateTime::currentDateTime();
	sysEvent.m_ymd = dateTime.year()*100*100 + dateTime.month()*100 + dateTime.day();
	sysEvent.m_hmsms= dateTime.hour()*100*100*1000 + dateTime.minute()*100*1000 + dateTime.second()*1000 + dateTime.microsec()/1000;

	sysEvent.m_security = pPara->eventLevel;
	
	//判断是否是恢复
	if ( chgData.data.val == pPara->normal )		//与正常状态相同
	{
		sysEvent.m_isRevert = true;
		sysEvent.SmsOut = 0;
	}
	else 
	{
		sysEvent.m_isRevert	= false;
		sysEvent.SmsOut = pPara->isMsgAlarm;		
	}

	sysEvent.PrintOut = 1;
	sysEvent.m_stateValue = chgData.data.val;
	sysEvent.m_stateValue1 = chgData.lastVal;

	return (bool)m_event.insertEvent(sysEvent);

}


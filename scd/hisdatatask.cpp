/*==============================================================================
* 文件名称 : dbsavetask.cpp
* 模块 : 数据库存盘任务
* 创建时间 : 2009-07-22 15:55:23
* 作者 : wfp
* 版本 : v1.0

* 一次修改后版本 : 1.1
* 修改摘要 :  对函数putStaRsData进行修改
* 修改者 :  hll
* 修改日期 : 2017-01-18 09:30:12
==============================================================================*/
#include "hisdatatask.h"
#include <ace/OS.h>
#include "sys/blog.h"
#include "scddef.h"
#include "dbsavetask.h"
#include "dac/dacdef.h"
#include "config.h"
#include "loadtask.h"

using namespace PDT;

CHisDataTask::CHisDataTask()
{
	m_timeMask = 0;
}

CHisDataTask::~CHisDataTask()
{

}

bool CHisDataTask::init()
{
	return true;
}

int CHisDataTask::start()
{
	logprint( LOG_SCD_DBSAVE_TASK,"<CHisDataTask>:创建一个线程!" );
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

void CHisDataTask::stop()
{
	if (thr_count() > 0) 
	{
		thr_mgr()-> cancel_task(this);
		wait();
	}
}

int CHisDataTask::svc()
{
	m_lastTime = CDateTime::currentDateTime();
	m_timeId = m_timer.add(1000);		//1000ms
	//m_staticTimeId = m_timer.add( CONFIG::instance()->staticSaveInterval() * 1000 ); //static*1000ms
	m_timer.start();

	DAC_SYSTEM_INFO* pSystemInfo = m_commInf.systemInfo();
	ACE_Time_Value tv(0,10000);			//10ms
	while (true)
	{
		if ( thr_mgr()->testcancel( thr_mgr()->thr_self() ) )
			break;
		ACE_OS::sleep( tv );

		if (  PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_GROUP)
			||PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_ROUTE)
			||PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_YC)
			||PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_YX))
		{
			continue;
		}

		//m_timeMask = INTERVAL_ALL; //wfp_test

		if ( m_timer.time_out(m_timeId) )
		{
			//定时存盘监测
			checkTimeInterval();

			//阀值存盘
			if ( CONFIG::instance()->isSaveWater() )
			{
				procWaterData();
			}
			////统计存盘
			//procStaData();

			////处理蓄电池
			//if ( CONFIG::instance()->hasBattery() )
			//{
			//	//蓄电池历史状态表
			//	if ( TASK_LOAD::instance()->batteryGroupNum() > 0 )
			//		procBatteryYx();

			//	//静态放电数据存充放电表(wfp masked at 20100723)
			//	//if ( !CONFIG::instance()->isSaveWater() || TASK_LOAD::instance()->batteryGroupNum() > 0 )
			//	//{
			//	//	if ( m_timer.time_out(m_staticTimeId) )
			//	//	{
			//	//		procBatteryChargeData();
			//	//	}
			//	//}
			//}

		}//end if timeout

	}
	return 0;
}

void CHisDataTask::checkTimeInterval()
{
	CDateTime curtime = CDateTime::currentDateTime();
	if ( curtime.month() != m_lastTime.month() )
	{
		PDT_SET_BITS(m_timeMask,INTERVAL_1_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_5_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_10_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_15_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_30_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_HOUR);
		PDT_SET_BITS(m_timeMask,INTERVAL_DAY);
		PDT_SET_BITS(m_timeMask,INTERVAL_MONTH);
	}
	else if ( curtime.day() != m_lastTime.day() )
	{
		PDT_SET_BITS(m_timeMask,INTERVAL_1_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_5_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_10_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_15_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_30_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_HOUR);
		PDT_SET_BITS(m_timeMask,INTERVAL_DAY);
	}
	else if ( curtime.hour() != m_lastTime.hour() )
	{
		PDT_SET_BITS(m_timeMask,INTERVAL_1_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_5_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_10_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_15_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_30_MIN);
		PDT_SET_BITS(m_timeMask,INTERVAL_HOUR);
	}
	else if ( curtime.minute() != m_lastTime.minute() )
	{
		PDT_SET_BITS(m_timeMask,INTERVAL_1_MIN);
		if ( curtime.minute() % 30 == 0 )
		{
			PDT_SET_BITS(m_timeMask,INTERVAL_1_MIN);
			PDT_SET_BITS(m_timeMask,INTERVAL_5_MIN);
			PDT_SET_BITS(m_timeMask,INTERVAL_10_MIN);
			PDT_SET_BITS(m_timeMask,INTERVAL_15_MIN);
			PDT_SET_BITS(m_timeMask,INTERVAL_30_MIN);
		}
		else if ( curtime.minute() % 15 == 0 )
		{
			PDT_SET_BITS(m_timeMask,INTERVAL_1_MIN);
			PDT_SET_BITS(m_timeMask,INTERVAL_5_MIN);
			PDT_SET_BITS(m_timeMask,INTERVAL_15_MIN);
		}
		else if ( curtime.minute() % 10 == 0 )
		{
			PDT_SET_BITS(m_timeMask,INTERVAL_1_MIN);
			PDT_SET_BITS(m_timeMask,INTERVAL_5_MIN);
			PDT_SET_BITS(m_timeMask,INTERVAL_10_MIN);
		}
		else if ( curtime.minute() % 5 == 0 )
		{
			PDT_SET_BITS(m_timeMask,INTERVAL_1_MIN);
			PDT_SET_BITS(m_timeMask,INTERVAL_5_MIN);
		}
		else 
		{
			PDT_SET_BITS(m_timeMask,INTERVAL_1_MIN);
		}
	}//end else if

	if ( m_timeMask != 0 )
	{
		procIntervalData();
		m_lastTime = curtime;
		m_timeMask = 0;	//清空所有码值
	}
	
}

//定时存盘
void CHisDataTask::procIntervalData()
{
	CDateTime curtime = CDateTime::currentDateTime();
	curtime.second(0);	//确保准点存盘

	DAC_ROUTE* pRoute = NULL;
	DAC_GROUP* pGroup = NULL;
	hUInt8 dataType = 0;

	for (hUInt32 routeNo=0;routeNo<m_commInf.systemInfo()->routeNum;routeNo++)
	{
		pRoute = m_commInf.route(routeNo);
		if ( pRoute == NULL ) continue;
		pGroup = m_commInf.group(pRoute->group);
		if ( pGroup == NULL ) continue;

		DAC_YC* pYcPara = NULL;
		DAC_YC_DATA val;
		for (hUInt32 pointNo=0;pointNo<pGroup->ycNum;pointNo++)
		{
			pYcPara = m_dataInf.ycPara(pRoute->group,pointNo);
			if ( pYcPara == NULL || !pYcPara->valid ) continue;
			
			//间隔为0:不存盘
			if ( pYcPara->saveInterval == 0 ) continue;

			if (  pYcPara->saveInterval == DAC_INTERVAL_1_MIN_VAL  && PDT_BIT_ENABLED(m_timeMask,INTERVAL_1_MIN)
				||pYcPara->saveInterval == DAC_INTERVAL_5_MIN_VAL  && PDT_BIT_ENABLED(m_timeMask,INTERVAL_5_MIN) 
				||pYcPara->saveInterval == DAC_INTERVAL_10_MIN_VAL && PDT_BIT_ENABLED(m_timeMask,INTERVAL_10_MIN) 
				||pYcPara->saveInterval == DAC_INTERVAL_15_MIN_VAL && PDT_BIT_ENABLED(m_timeMask,INTERVAL_15_MIN)
				||pYcPara->saveInterval == DAC_INTERVAL_30_MIN_VAL && PDT_BIT_ENABLED(m_timeMask,INTERVAL_30_MIN)
				||pYcPara->saveInterval == DAC_INTERVAL_HOUR_VAL   && PDT_BIT_ENABLED(m_timeMask,INTERVAL_HOUR)
				||pYcPara->saveInterval == DAC_INTERVAL_DAY_VAL    && PDT_BIT_ENABLED(m_timeMask,INTERVAL_DAY)
				||pYcPara->saveInterval == DAC_INTERVAL_MONTH_VAL  && PDT_BIT_ENABLED(m_timeMask,INTERVAL_MONTH) )
			{
				if ( !m_dataInf.yc(routeNo,pointNo,val) )
					continue;

				val.updateTime = (hUInt32)curtime.toTimeT();		//确保准点存盘
				dataType = dataTypeFromQuality(val,pYcPara);

				if ( CONFIG::instance()->hasBattery() 
					&& TASK_LOAD::instance()->isBatteryValid(pRoute->group) )
				{//处理蓄电池组
					putBatteryYcData(pRoute->group,pointNo,dataType,val);
				}
				else
				{
					//if (dataType != HIS_TYPE_INVALID_REAL)
					{
						putYcData(pRoute->group,pointNo,dataType,pYcPara->type,val);
					}		
				}
			}
		}//end for
	}//end for

}

//阀值存盘
void CHisDataTask::procWaterData()
{
	DAC_ROUTE* pRoute = NULL;
	DAC_GROUP* pGroup = NULL;
	hUInt8 dataType = 0;

	for (hUInt32 routeNo=0;routeNo<m_commInf.systemInfo()->routeNum;routeNo++)
	{
		pRoute = m_commInf.route(routeNo);
		if ( pRoute == NULL ) continue;
		pGroup = m_commInf.group(pRoute->group);
		if ( pGroup == NULL ) continue;

		DAC_YC* pYcPara = NULL;
		SCD_REAL_YC* pRealVal = NULL;
		DAC_YC_CHG_DATA val;
		while ( m_dataInf.chgYc(routeNo,val) )
		{
			pYcPara = m_dataInf.ycPara(pRoute->group,val.no);
			if ( pYcPara == NULL || !pYcPara->valid ) continue;

			//阀值小于0，不进行阀值存盘
			if ( pYcPara->minChgVal <= -0.000001 ) continue;

			//历史存盘数据类型
			dataType = dataTypeFromQuality(val.data,pYcPara);

			pRealVal = m_readDataInf.ycData(routeNo,val.no);
			if ( pRealVal == NULL ) continue;

			//wfp add at 20110816 for 非微网状态下处理蓄电池充放电状态下，不考虑阀值，直接存
			if ( CONFIG::instance()->hasBattery() 
				&& TASK_LOAD::instance()->isBatteryValid(pRoute->group)
				&& !CONFIG::instance()->microNet() )
			{
				int batState = batteryState(pRoute->group);

				if (  batState == BAT_STATE_DISCHARGE || batState == BAT_STATE_EQUAL )
				{//充放电状态
					putBatteryYcData(pRoute->group,val.no,dataType,val.data);
					continue;
				}

			}

			//第一帧存盘&阀值为0直接存盘&超阀值存盘
			if( !pRealVal->valid
				||PDT_ISZERO(pYcPara->minChgVal)
				||fabs(val.data.val - pRealVal->lastSaveVal) >= pYcPara->minChgVal )
			{
				//修改实时库值
				pRealVal->valid = true;
				pRealVal->lastSaveVal = val.data.val;

				if ( CONFIG::instance()->hasBattery() 
					&& TASK_LOAD::instance()->isBatteryValid(pRoute->group) )
				{//处理蓄电池组
					putBatteryYcData(pRoute->group,val.no,dataType,val.data);
				}
				else 
					putYcData(pRoute->group,val.no,dataType,val.data);
			}
		}//end while

	}//end for
}

//统计数据存盘
void CHisDataTask::procStaData()
{
	//wfp add at 20100813
	if ( TASK_DBSAVE::instance()->isFull() ) return;

	DAC_GROUP* pGroup = NULL;
	HIS_STA_DATA val;
	HIS_STA_RS_DATA rsval;

	for (hUInt32 groupNo=0;groupNo<m_commInf.systemInfo()->groupNum;groupNo++)
	{
		pGroup = m_commInf.group(groupNo);
		if ( pGroup == NULL ) continue;

		while ( m_hisStaInf.get(groupNo,val) )
		{	
			putStaData(groupNo,val);

		}//end while

	}//end for

	while ( m_hisStaInf.rsget(rsval) )
	{
		putStaRsData(rsval);
	}
}

//静态放电+充电数据
void CHisDataTask::procBatteryChargeData()
{
	CDateTime curtime = CDateTime::currentDateTime();

	DAC_ROUTE* pRoute = NULL;
	DAC_GROUP* pGroup = NULL;
	hUInt8 dataType = 0;
	int batState = -1;

	for (hUInt32 routeNo=0;routeNo<m_commInf.systemInfo()->routeNum;routeNo++)
	{
		pRoute = m_commInf.route(routeNo);
		if ( pRoute == NULL ) continue;
		pGroup = m_commInf.group(pRoute->group);
		if ( pGroup == NULL ) continue;

		if ( !TASK_LOAD::instance()->isBatteryValid(pRoute->group) ) continue;

		batState = batteryState(pRoute->group);
		if (  batState != BAT_STATE_DISCHARGE && batState != BAT_STATE_EQUAL ) continue;

		DAC_YC* pYcPara = NULL;
		DAC_YC_DATA val;
		for (hUInt32 pointNo=0;pointNo<pGroup->ycNum;pointNo++)
		{
			pYcPara = m_dataInf.ycPara(pRoute->group,pointNo);
			if ( pYcPara == NULL || !pYcPara->valid ) continue;

			if ( !m_dataInf.yc(routeNo,pointNo,val) )
				continue;

			val.updateTime = (hUInt32)curtime.toTimeT();		//确保准点存盘
			dataType = dataTypeFromQuality(val,pYcPara);

			putBatteryChargeData(pRoute->group,pointNo,dataType,val,batState);

		}//end for
	}//end for
}

//存指定路径的静态放电+充电数据
void CHisDataTask::procBatteryChargeData(int routeNo,int batState,hUInt32 saveTime)
{
	DAC_ROUTE* pRoute = m_commInf.route(routeNo);
	if ( pRoute == NULL ) return;
	DAC_GROUP* pGroup = m_commInf.group(pRoute->group);
	if ( pGroup == NULL ) return;
	
	if (  batState != BAT_STATE_DISCHARGE && batState != BAT_STATE_EQUAL ) return;
	if ( !TASK_LOAD::instance()->isBatteryValid(pRoute->group) ) return;

	hUInt8 dataType = 0;
	DAC_YC* pYcPara = NULL;
	DAC_YC_DATA val;

	for (hUInt32 pointNo=0;pointNo<pGroup->ycNum;pointNo++)
	{
		pYcPara = m_dataInf.ycPara(pRoute->group,pointNo);
		if ( pYcPara == NULL || !pYcPara->valid ) continue;

		if ( !m_dataInf.yc(routeNo,pointNo,val) )
			continue;

		//间隔为0:不存盘
		if ( pYcPara->saveInterval == 0 ) continue;	//wfp add at 20110726

		val.updateTime = saveTime;
		dataType = dataTypeFromQuality(val,pYcPara);

		putBatteryChargeData(pRoute->group,pointNo,dataType,val,batState);

	}//end for
}

//蓄电池yx变化缓冲区存蓄电池历史状态表--蓄电池状态+各个单体电池维护状态
void CHisDataTask::procBatteryYx()
{
	DAC_ROUTE* pRoute = NULL;
	DAC_YX_CHG_DATA chgData;
	DAC_YX* pPara = NULL;

	if ( TASK_LOAD::instance()->batteryGroupNum() <= 0 ) return;

	for (hUInt32 route=0;route<m_commInf.systemInfo()->routeNum;route++)
	{
		pRoute = m_commInf.route(route);
		if ( pRoute == NULL ) continue;

		if ( !TASK_LOAD::instance()->isBatteryValid(pRoute->group) ) continue;

		while ( m_dataInf.chgYx(route,chgData) )
		{
			pPara = m_dataInf.yxPara(pRoute->group,chgData.no);
			if ( pPara == NULL || !pPara->valid ) continue;

			//防止仅时间变化而值不变的情况写入历史库
			if ( chgData.data.val == chgData.lastVal ) continue;	//wfp changed at 20110726

			//蓄电池状态
			if ( chgData.no == 0 )
			{
				//存充放电起始帧数据到this_charge表
				if (  chgData.data.val == BAT_STATE_DISCHARGE || chgData.data.val == BAT_STATE_EQUAL )
				{
					if ( !CONFIG::instance()->microNet() )			//wfp add at 20110726
						procBatteryChargeData(route,chgData.data.val,chgData.data.updateTime);
				}

				putBatteryStateData(pRoute->group,chgData);
			}
			else if ( pPara->code[0] == 'w' && pPara->code[1] == 'h' )
			{//维护状态
				putBatteryStateData(pRoute->group,chgData);
			}

		}
	}//end for
}

//将遥测历史数据放入CDbSaveTask.messageQueue
/*
 * 函数功能：
 * 函数参数：groupNo:数据组序号；no:测点序号；dataType:历史数据类型；与SCD_DATA_YC结构对应
 */
bool CHisDataTask::putYcData(hInt32 groupNo,hInt32 no,hUInt32 dataType,DAC_YC_DATA& val)
{
	ACE_Message_Block* mblk = new ACE_Message_Block(sizeof(SCD_DATA_HEAD) + sizeof(SCD_DATA_YC)+1);
	if ( mblk == NULL ) return false;

	SCD_DATA_HEAD* pHead = (SCD_DATA_HEAD*)mblk->wr_ptr();
	SCD_DATA_YC* pData = (SCD_DATA_YC*)(mblk->wr_ptr() + sizeof(SCD_DATA_HEAD));

	pHead->dataclass = HIS_CLASS_YC;
	pData->group = groupNo;
	pData->no = no;
	pData->dataType = dataType;
	pData->data.val = val.val;
	pData->data.updateTime = val.updateTime;
	pData->data.quality = val.quality;
	mblk->wr_ptr(sizeof(SCD_DATA_HEAD) + sizeof(SCD_DATA_YC));
	
	//wfp changed at 20100805 from nonblock(0)-->blockTime(2)
	ACE_Time_Value blockTime(QUEUE_BLOCK_TIME,0);	
	if ( TASK_DBSAVE::instance()->put(mblk,&blockTime) == -1 ) 
	{
		logprint(LOG_SCD_HISDATA_TASK,"<his yc>can not put the msg");
		mblk->release();
		return false;
	}

	return true;
}


bool CHisDataTask::putYcData(hInt32 groupNo,hInt32 no,hUInt32 dataType,hUInt32 ycdataType,DAC_YC_DATA& val)
{
	ACE_Message_Block* mblk = new ACE_Message_Block(sizeof(SCD_DATA_HEAD) + sizeof(SCD_DATA_YC)+1);
	if ( mblk == NULL ) return false;

	SCD_DATA_HEAD* pHead = (SCD_DATA_HEAD*)mblk->wr_ptr();
	SCD_DATA_YC* pData = (SCD_DATA_YC*)(mblk->wr_ptr() + sizeof(SCD_DATA_HEAD));

	pHead->dataclass = HIS_CLASS_YC;
	pData->group = groupNo;
	pData->no = no;
	pData->dataType = dataType;
	pData->ycdataType = ycdataType;
	pData->data.val = val.val;
	pData->data.updateTime = val.updateTime;
	pData->data.quality = val.quality;
	mblk->wr_ptr(sizeof(SCD_DATA_HEAD) + sizeof(SCD_DATA_YC));

	//wfp changed at 20100805 from nonblock(0)-->blockTime(2)
	ACE_Time_Value blockTime(QUEUE_BLOCK_TIME,0);	
	if ( TASK_DBSAVE::instance()->put(mblk,&blockTime) == -1 ) 
	{
		logprint(LOG_SCD_HISDATA_TASK,"<his yc>can not put the msg");
		mblk->release();
		return false;
	}

	return true;
}

bool CHisDataTask::putBatteryYcData(hInt32 groupNo,hInt32 no,hUInt32 dataType,DAC_YC_DATA& val)
{
	int batState = batteryState(groupNo);

	//wfp changed at 20110715
	if ( batState == -1 ) return false;
	else if (  batState == BAT_STATE_DISCHARGE || batState == BAT_STATE_EQUAL )
	{//充放电状态
		return putBatteryChargeData(groupNo,no,dataType,val,batState);
	}
	else
	{//非充放电状态
		return putBatteryRunData(groupNo,no,dataType,val,batState);
	}

	return true;
}

/*
* 函数功能：组成运行监测历史数据
* 函数参数：groupNo:数据组序号；no:测点序号；dataType:历史数据类型；与SCD_DATA_YC结构对应
*/
bool CHisDataTask::putBatteryRunData(hInt32 groupNo,hInt32 no,hUInt32 dataType,DAC_YC_DATA& val,int batState)
{
	HIS_STA_DATA hisData;
	hisData.dataclass = HIS_CLASS_RUN;
	hisData.no = no;
	hisData.type = dataType;
	hisData.val = val.val;

	//获取值状态
	hisData.valstate = batState;
	hisData.saveTime = val.updateTime;

	return putStaData(groupNo,hisData);
}

/*
* 函数功能：组成静态放电+充电历史数据
* 函数参数：groupNo:数据组序号；no:测点序号；dataType:历史数据类型；与SCD_DATA_YC结构对应
*/
bool CHisDataTask::putBatteryChargeData(hInt32 groupNo,hInt32 no,hUInt32 dataType,DAC_YC_DATA& val,int batState)
{
	HIS_STA_DATA hisData;
	hisData.dataclass = HIS_CLASS_CHARGE;
	hisData.no = no;
	hisData.type = dataType;
	hisData.val = val.val;

	//获取值状态
	hisData.valstate = batState;
	hisData.saveTime = val.updateTime;

	return putStaData(groupNo,hisData);
}

/*
* 函数功能：组成蓄电池历史状态数据
* 函数参数：groupNo:数据组序号；no:测点序号
*/
bool CHisDataTask::putBatteryStateData(hInt32 groupNo,DAC_YX_CHG_DATA& chgVal)
{
	HIS_STA_DATA hisData;
	hisData.dataclass = HIS_CLASS_STATE;
	hisData.no = chgVal.no;
	hisData.valstate = chgVal.data.val;		//遥信值即state
	hisData.saveTime = chgVal.data.updateTime;

	return putStaData(groupNo,hisData);
}

//将统计数据+蓄电池数据放入CDbSaveTask.messageQueue
bool CHisDataTask::putStaData(hInt32 groupNo,HIS_STA_DATA& val)
{
	//--获取存盘类型begin--
	//int type = saveType(val.type);
	//if ( type == -1 ) 
	//{
	//	logprint(LOG_SCD_HISDATA_TASK,"<存盘数据>type=%d 无效",(hUInt32)val.type);
	//	return false;
	//}
	//--获取存盘类型end--

	//整点值保存在yc历史表
	if ( val.dataclass == HIS_CLASS_YC )
	{
		DAC_YC_DATA data;
		data.quality = 0;
		data.updateTime = val.saveTime;
		data.val = val.val;
		return putYcData(groupNo,val.no,val.type,data);
	}

	ACE_Message_Block* mblk = new ACE_Message_Block(sizeof(SCD_DATA_HEAD) + sizeof(SCD_DATA_STA)+1);
	if ( mblk == NULL ) return false;

	SCD_DATA_HEAD* pHead = (SCD_DATA_HEAD*)mblk->wr_ptr();
	SCD_DATA_STA* pData = (SCD_DATA_STA*)(mblk->wr_ptr() + sizeof(SCD_DATA_HEAD));

	pHead->dataclass = val.dataclass;
	pData->group = groupNo;
	ACE_OS::memcpy(&pData->data,&val,sizeof(HIS_STA_DATA));
	mblk->wr_ptr(sizeof(SCD_DATA_HEAD) + sizeof(SCD_DATA_STA));

	//wfp changed at 20100805 from nonblock(0)-->blockTime(2)
	ACE_Time_Value blockTime(QUEUE_BLOCK_TIME,0);	
	if ( TASK_DBSAVE::instance()->put(mblk,&blockTime) == -1 ) 
	{
		logprint(LOG_SCD_HISDATA_TASK,"<统计数据>can not put the msg");
		mblk->release();
		return false;
	}

	return true;
}

//将统计数据+蓄电池数据放入CDbSaveTask.messageQueue      2017/1/18 09:30:12 by hll 
bool CHisDataTask::putStaRsData(HIS_STA_RS_DATA& rsval)
{
	ACE_Message_Block* mblk = new ACE_Message_Block(sizeof(SCD_DATA_HEAD) + sizeof(HIS_STA_RS_DATA)+1);
	if ( mblk == NULL ) return false;

	SCD_DATA_HEAD* pHead = (SCD_DATA_HEAD*)mblk->wr_ptr();
	HIS_STA_RS_DATA* pData = (HIS_STA_RS_DATA*)(mblk->wr_ptr() + sizeof(SCD_DATA_HEAD));


	pHead->dataclass = rsval.dataclass;
	ACE_OS::memcpy(pData,&rsval,sizeof(HIS_STA_RS_DATA));
	mblk->wr_ptr(sizeof(SCD_DATA_HEAD) + sizeof(HIS_STA_RS_DATA));

	//wfp changed at 20100805 from nonblock(0)-->blockTime(2)
	ACE_Time_Value blockTime(QUEUE_BLOCK_TIME,0);	
	if ( TASK_DBSAVE::instance()->put(mblk,&blockTime) == -1 ) 
	{
		logprint(LOG_SCD_HISDATA_TASK,"<区域厂站统计数据>can not put the msg");
		mblk->release();
		return false;
	}

	return true;
}

/*
 * 函数说明：根据值类型获取存盘类型
 * 函数返回：>0:存盘类型;-1:获取存盘类型失败
 */
int CHisDataTask::saveType(hUInt32 valType) const
{
	int type = -1;
	if ( valType >= HIS_TYPE_REAL && valType <= HIS_TYPE_HOUR_VAL )		//处理整点值等
		type = HIS_CLASS_YC;
	else if ( valType > HIS_TYPE_DAY_START && valType < HIS_TYPE_DAY_END )
		type = HIS_CLASS_DAY_STA;
	else if ( valType > HIS_TYPE_MONTH_START && valType < HIS_TYPE_MONTH_END )
		type = HIS_CLASS_MONTH_STA;
	else if ( valType > HIS_TYPE_SEASON_START && valType < HIS_TYPE_SEASON_END )
		type = HIS_CLASS_SEASON_STA;
	else if ( valType > HIS_TYPE_YEAR_START && valType < HIS_TYPE_YEAR_END )
		type = HIS_CLASS_YEAR_STA;
	else
	{
		type = -1;
	}

	return type;
}

/*
* 函数功能：根据值、质量码、值参数，获取遥测历史数据存盘类型
*/
hUInt8 CHisDataTask::dataTypeFromQuality(const DAC_YC_DATA& data,DAC_YC* pPara) const
{
	//无效值
	if ( PDT_BIT_ENABLED(data.quality,QUALITY_BIT_IV) )
	{
		return HIS_TYPE_INVALID_REAL;
	}
	
	//有效值再次划分
	if ( pPara != NULL )
	{
		if ( !PDT_ISZERO(pPara->highLimit) && data.val >= pPara->highLimit )
			return HIS_TYPE_HIGH;
		else if ( !PDT_ISZERO(pPara->lowLimit) && data.val <= pPara->lowLimit )
			return HIS_TYPE_LOW;
	}
	return HIS_TYPE_REAL;
}

/*
* 函数功能：根据遥信第一个点，获取蓄电池状态
*/
int CHisDataTask::batteryState(int groupNo)
{
	DAC_GROUP_INFO* pGroupInfo = m_commInf.groupInfo(groupNo);
	if  ( pGroupInfo == NULL ) return -1;

	if ( pGroupInfo->routeNum <= 0 ) return -1;

	int routeNo = pGroupInfo->routes[0];
	DAC_ROUTE* pRoute = m_commInf.route(routeNo);
	if ( pRoute == NULL ) return -1;

	DAC_YX_DATA val;
	if ( !m_dataInf.yx(routeNo,0,val) ) return -1;

	//质量码无效，返回错误-wfp add at 20110526
	if ( PDT_BIT_ENABLED((int)val.quality,QUALITY_BIT_IV) ) return -1;
	
	return val.val;
}

/*
* 函数功能：判断该yx测点是否是维护状态测点
* 函数输入：batNum--蓄电池节数；pointNo--yx测点序号
* 函数返回：无;
*/
bool CHisDataTask::isMaintainPoint(int batteryNum,int pointNo) const
{
	if ( batteryNum <= 0 ) return false;

	if ( pointNo >= (10+3*batteryNum) && pointNo < (10+4*batteryNum) )
		return true;

	return false;
}


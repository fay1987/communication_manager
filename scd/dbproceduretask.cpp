/*==============================================================================
* 文件名称 : dbproceduretask.cpp
* 模块 : 执行统计存储过程任务
* 创建时间 : 2010-03-09 15:55:23
* 作者 : wfp
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
* 功能说明 ：处理日月季年统计，该功能可移植到scd进程
				1.当日日统计--考虑到衢州数据库处理性能低，因而不统计，以后可添加为每小时统计
				2.当月月统计--每天3点
				3.当季季统计--每天3点
				4.当年年统计--每天3点
				5.上一日的日统计--换日时1点，即每天1点
				6.上一月的月统计--换月时4点，即每月1号4点
				7.上一季的季统计--换季时5点
				8.上一年的年统计--换年时6点
==============================================================================*/
#include "dbproceduretask.h"
#include <ace/OS.h>
#include "sys/blog.h"
#include "config.h"
#include "scddef.h"

using namespace PDT;

CDbProcedureTask::CDbProcedureTask()
{
	m_pRdbOp = NULL;
	m_pRdbOp = new CRdbOp(CRdbOp::PingDirect);
	if ( !m_pRdbOp )
		logprint(LOG_SCD_DBPROCEDURE_TASK,"CDbProcedureTask::CDbProcedureTask() : can't new CRdbOp object");
}

CDbProcedureTask::~CDbProcedureTask()
{
	if ( m_pRdbOp != NULL )
	{
		delete m_pRdbOp;
		m_pRdbOp = NULL;
	}
}

bool CDbProcedureTask::init()
{
	return true;
}

int CDbProcedureTask::start()
{
	logprint( LOG_SCD_DBPROCEDURE_TASK,"<CDbProcedureTask>:创建一个线程!" );
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

void CDbProcedureTask::stop()
{
	if (thr_count() > 0) 
	{
		thr_mgr()-> cancel_task(this);
		wait();
	}
}

int CDbProcedureTask::svc()
{
	//启动时先统计一次当天+当月
	doDaySta( CDateTime::currentDateTime() );
	//doMonthSta( CDateTime::currentDateTime() );

	//设定并启动定时器
	m_timeId = m_timer.add(1000*INTERVAL_CHECK_STA);			//10s
	m_timer.start();

	ACE_Time_Value tv(0,10000);									//10ms
	ACE_Time_Value nonblock (0);
	CDateTime lastHourStaTime = CDateTime::currentDateTime();	//上一小时统计的统计时间
	CDateTime lastDayStaTime = CDateTime::currentDateTime();	//上一日统计的统计时间
	CDateTime lastMonthStaTime = CDateTime::currentDateTime();	//上一月统计的统计时间
	CDateTime lastSeasonStaTime = CDateTime::currentDateTime();	//上一季统计的统计时间
	CDateTime lastYearStaTime = CDateTime::currentDateTime();	//上一年统计的统计时间
	CDateTime curStaTime = CDateTime::currentDateTime();		//当前统计(当月，当季，当年)

	while ( true )
	{
		if ( thr_mgr()->testcancel( thr_mgr()->thr_self() ) )
			break;

		ACE_OS::sleep( tv );

		if ( m_timer.time_out(m_timeId) )
		{
			CDateTime curDateTime = CDateTime::currentDateTime();

			//每个小时的第2分钟，上个小时的小时统计
			if( curDateTime.hour() != lastHourStaTime.hour() && curDateTime.minute() == 2 )
			{
				ACE_Time_Value timeVal = curDateTime.toTimeValue() - ACE_Time_Value(1*60*60);
				doHourSta(CDateTime(timeVal));
				lastHourStaTime = curDateTime;
			}
			//每天1点--上一日的日统计
			if ( curDateTime.day() != lastDayStaTime.day() && curDateTime.hour() == 1 )
			{
				//昨天的时间=减一天
				ACE_Time_Value timeVal = curDateTime.toTimeValue() - ACE_Time_Value(24*60*60);
				doDaySta(CDateTime(timeVal));
				lastDayStaTime = curDateTime;
			}

			//每天3点--当前的月、季、年统计
			if ( curDateTime.day() != curStaTime.day() && curDateTime.hour() == 3 )
			{
				doMonthSta(curDateTime);
				doSeasonSta(curDateTime);
				doYearSta(curDateTime);
				curStaTime = curDateTime;
			}

			//换月日4点--上月的月统计
			if ( curDateTime.day() != lastMonthStaTime.day() && curDateTime.hour() == 4 )
			{
				//上个月的时间=当前时间减一天
				ACE_Time_Value timeVal = curDateTime.toTimeValue() - ACE_Time_Value(24*60*60);
				doMonthSta(CDateTime(timeVal));
				lastMonthStaTime = curDateTime;
			}

			//换季日4点--上季的季统计
			if ( curDateTime.day() != lastSeasonStaTime.day() && curDateTime.hour() == 5 )
			{
				//上季的时间=当前时间减一天
				ACE_Time_Value timeVal = curDateTime.toTimeValue() - ACE_Time_Value(24*60*60);
				doSeasonSta(CDateTime(timeVal));
				lastSeasonStaTime = curDateTime;
			}

			//换年日4点--上年的年统计
			if ( curDateTime.day() != lastYearStaTime.day() && curDateTime.hour() == 6 )
			{
				//上一年的时间=当前时间减一天
				ACE_Time_Value timeVal = curDateTime.toTimeValue() - ACE_Time_Value(24*60*60);
				doYearSta(CDateTime(timeVal));
				lastYearStaTime = curDateTime;
			}
		}

	}//end while

	return 0;
}

bool CDbProcedureTask::doHourSta(const CDateTime& dateTime)
{
	//以下增加电量日统计，统计每小时用电情况，放到日统计表
	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s %02d:00:00\'",
		PROCEDURE_KWH_DAY_STA,dateTime.toString(DT_FMT_STA).c_str(),dateTime.hour());

	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBPROCEDURE_TASK,"<dokwhdaySta> time=%s %02d:00:00",dateTime.toString(DT_FMT_STA).c_str(),dateTime.hour());
	return true;
}

bool CDbProcedureTask::doDaySta(const CDateTime& dateTime)
{
	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\',%d",
		PROCEDURE_DAY_STA,dateTime.toString(DT_FMT_STA).c_str(),CONFIG::instance()->saveMode());

	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		//return false;
		logprint(LOG_SCD_DBPROCEDURE_TASK,"<doDaySta> fail!!! time=%s",dateTime.toString(DT_FMT_STA).c_str());
	}
	else
		logprint(LOG_SCD_DBPROCEDURE_TASK,"<doDaySta> sucess !time=%s",dateTime.toString(DT_FMT_STA).c_str());

	//以下增加电量月统计，统计每天用电情况，放到月统计表
	//sqlStr = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\'",
		PROCEDURE_KWH_MONTH_STA,dateTime.toString(DT_FMT_STA).c_str());

	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBPROCEDURE_TASK,"<dokwhmonthSta> time=%s",dateTime.toString(DT_FMT_STA).c_str());

	return true;
}

bool CDbProcedureTask::doMonthSta(const CDateTime& dateTime)
{
	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\',%d",
		PROCEDURE_MONTH_STA,dateTime.toString(DT_FMT_STA).c_str(),CONFIG::instance()->saveMode());

	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		//return false;
		logprint(LOG_SCD_DBPROCEDURE_TASK,"<doMonthSta> failed !!! time=%s",dateTime.toString(DT_FMT_STA).c_str());
	}
	else
		logprint(LOG_SCD_DBPROCEDURE_TASK,"<doMonthSta> success ! time=%s",dateTime.toString(DT_FMT_STA).c_str());

	//以下增加电量年统计，统计每月用电情况，放到年统计表
	//sqlStr = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\'",
		PROCEDURE_KWH_YEAR_STA,dateTime.toString(DT_FMT_STA).c_str());

	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBPROCEDURE_TASK,"<dokwhyearSta> time=%s",dateTime.toString(DT_FMT_STA).c_str());

	return true;
}

bool CDbProcedureTask::doSeasonSta(const CDateTime& dateTime)
{
	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\',%d",
		PROCEDURE_SEASON_STA,dateTime.toString(DT_FMT_STA).c_str(),CONFIG::instance()->saveMode());

	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBPROCEDURE_TASK,"<doSeasonSta> time=%s",dateTime.toString(DT_FMT_STA).c_str());
	return true;
}

bool CDbProcedureTask::doYearSta(const CDateTime& dateTime)
{
	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\',%d",
		PROCEDURE_YEAR_STA,dateTime.toString(DT_FMT_STA).c_str(),CONFIG::instance()->saveMode());

	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBPROCEDURE_TASK,"<doYearSta> time=%s",dateTime.toString(DT_FMT_STA).c_str());
	return true;
}

int CDbProcedureTask::season(const CDateTime& dateTime)
{
	int curSeason = 1;
	int month = dateTime.month();

	if ( month >= 1 && month <= 3  )
		curSeason = 1;
	else if ( month >= 4 && month <= 6 )
		curSeason = 2;
	else if ( month >= 7 && month <= 9 )
		curSeason = 3;
	else if ( month >= 10 && month <= 12 )
		curSeason = 4;
	else curSeason = 0;

	return curSeason;
}

/*==============================================================================
* �ļ����� : dbproceduretask.cpp
* ģ�� : ִ��ͳ�ƴ洢��������
* ����ʱ�� : 2010-03-09 15:55:23
* ���� : wfp
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
* ����˵�� ���������¼���ͳ�ƣ��ù��ܿ���ֲ��scd����
				1.������ͳ��--���ǵ��������ݿ⴦�����ܵͣ������ͳ�ƣ��Ժ�����ΪÿСʱͳ��
				2.������ͳ��--ÿ��3��
				3.������ͳ��--ÿ��3��
				4.������ͳ��--ÿ��3��
				5.��һ�յ���ͳ��--����ʱ1�㣬��ÿ��1��
				6.��һ�µ���ͳ��--����ʱ4�㣬��ÿ��1��4��
				7.��һ���ļ�ͳ��--����ʱ5��
				8.��һ�����ͳ��--����ʱ6��
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
	logprint( LOG_SCD_DBPROCEDURE_TASK,"<CDbProcedureTask>:����һ���߳�!" );
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
	//����ʱ��ͳ��һ�ε���+����
	doDaySta( CDateTime::currentDateTime() );
	//doMonthSta( CDateTime::currentDateTime() );

	//�趨��������ʱ��
	m_timeId = m_timer.add(1000*INTERVAL_CHECK_STA);			//10s
	m_timer.start();

	ACE_Time_Value tv(0,10000);									//10ms
	ACE_Time_Value nonblock (0);
	CDateTime lastHourStaTime = CDateTime::currentDateTime();	//��һСʱͳ�Ƶ�ͳ��ʱ��
	CDateTime lastDayStaTime = CDateTime::currentDateTime();	//��һ��ͳ�Ƶ�ͳ��ʱ��
	CDateTime lastMonthStaTime = CDateTime::currentDateTime();	//��һ��ͳ�Ƶ�ͳ��ʱ��
	CDateTime lastSeasonStaTime = CDateTime::currentDateTime();	//��һ��ͳ�Ƶ�ͳ��ʱ��
	CDateTime lastYearStaTime = CDateTime::currentDateTime();	//��һ��ͳ�Ƶ�ͳ��ʱ��
	CDateTime curStaTime = CDateTime::currentDateTime();		//��ǰͳ��(���£�����������)

	while ( true )
	{
		if ( thr_mgr()->testcancel( thr_mgr()->thr_self() ) )
			break;

		ACE_OS::sleep( tv );

		if ( m_timer.time_out(m_timeId) )
		{
			CDateTime curDateTime = CDateTime::currentDateTime();

			//ÿ��Сʱ�ĵ�2���ӣ��ϸ�Сʱ��Сʱͳ��
			if( curDateTime.hour() != lastHourStaTime.hour() && curDateTime.minute() == 2 )
			{
				ACE_Time_Value timeVal = curDateTime.toTimeValue() - ACE_Time_Value(1*60*60);
				doHourSta(CDateTime(timeVal));
				lastHourStaTime = curDateTime;
			}
			//ÿ��1��--��һ�յ���ͳ��
			if ( curDateTime.day() != lastDayStaTime.day() && curDateTime.hour() == 1 )
			{
				//�����ʱ��=��һ��
				ACE_Time_Value timeVal = curDateTime.toTimeValue() - ACE_Time_Value(24*60*60);
				doDaySta(CDateTime(timeVal));
				lastDayStaTime = curDateTime;
			}

			//ÿ��3��--��ǰ���¡�������ͳ��
			if ( curDateTime.day() != curStaTime.day() && curDateTime.hour() == 3 )
			{
				doMonthSta(curDateTime);
				doSeasonSta(curDateTime);
				doYearSta(curDateTime);
				curStaTime = curDateTime;
			}

			//������4��--���µ���ͳ��
			if ( curDateTime.day() != lastMonthStaTime.day() && curDateTime.hour() == 4 )
			{
				//�ϸ��µ�ʱ��=��ǰʱ���һ��
				ACE_Time_Value timeVal = curDateTime.toTimeValue() - ACE_Time_Value(24*60*60);
				doMonthSta(CDateTime(timeVal));
				lastMonthStaTime = curDateTime;
			}

			//������4��--�ϼ��ļ�ͳ��
			if ( curDateTime.day() != lastSeasonStaTime.day() && curDateTime.hour() == 5 )
			{
				//�ϼ���ʱ��=��ǰʱ���һ��
				ACE_Time_Value timeVal = curDateTime.toTimeValue() - ACE_Time_Value(24*60*60);
				doSeasonSta(CDateTime(timeVal));
				lastSeasonStaTime = curDateTime;
			}

			//������4��--�������ͳ��
			if ( curDateTime.day() != lastYearStaTime.day() && curDateTime.hour() == 6 )
			{
				//��һ���ʱ��=��ǰʱ���һ��
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
	//�������ӵ�����ͳ�ƣ�ͳ��ÿСʱ�õ�������ŵ���ͳ�Ʊ�
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

	//�������ӵ�����ͳ�ƣ�ͳ��ÿ���õ�������ŵ���ͳ�Ʊ�
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

	//�������ӵ�����ͳ�ƣ�ͳ��ÿ���õ�������ŵ���ͳ�Ʊ�
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

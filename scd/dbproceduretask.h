#ifndef _PDT_SCD_DBPROCEDURE_TASK_H_
#define _PDT_SCD_DBPROCEDURE_TASK_H_

#include "utl/intervaltimerset.h"
#include "ace/Task.h"
#include "ace/Thread_Manager.h"
#include "rdbop/rdbop.h"
#include "utl/datetime.h"

#define PROCEDURE_DAY_STA		"phis_staticsday "		//日统计存储过程
#define PROCEDURE_MONTH_STA		"phis_staticsmonth"		//月统计存储过程
#define PROCEDURE_SEASON_STA	"phis_staticsseason"	//季统计存储过程
#define PROCEDURE_YEAR_STA		"phis_staticsyear"		//年统计存储过程
#define PROCEDURE_KWH_DAY_STA		"phis_daykwh"		//日电量统计存储过程
#define PROCEDURE_KWH_MONTH_STA		"phis_monthkwh"		//月电量统计存储过程
#define PROCEDURE_KWH_YEAR_STA		"phis_yearkwh"		//年电量统计存储过程

#define INTERVAL_CHECK_STA		10						//统计存储过程检查间隔(10s)

#define	DT_FMT_STA			"yyyy-MM-dd"				//统计时间格式

class CDbProcedureTask : public ACE_Task<ACE_NULL_SYNCH>
{
public:
	CDbProcedureTask();
	virtual ~CDbProcedureTask();
public:
	bool		init();
	int			start();
	void		stop();
	int			svc();
protected:
	bool		doHourSta(const PDT::CDateTime& dateTime);
	bool		doDaySta(const PDT::CDateTime& dateTime);
	bool		doMonthSta(const PDT::CDateTime& dateTime);
	bool		doSeasonSta(const PDT::CDateTime& dateTime);
	bool		doYearSta(const PDT::CDateTime& dateTime);

	//季节转换
	int			season(const PDT::CDateTime& dateTime);
private:
	PDT::CIntervalTimerSet	m_timer;
	int						m_timeId;
	PDT::CRdbOp*			m_pRdbOp;
};

typedef ACE_Singleton<CDbProcedureTask, ACE_Null_Mutex> TASK_DBPROCEDURE;

#endif	//_PDT_SCD_DBPROCEDURE_TASK_H_

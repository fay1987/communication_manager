#ifndef _PDT_SCD_DBPROCEDURE_TASK_H_
#define _PDT_SCD_DBPROCEDURE_TASK_H_

#include "utl/intervaltimerset.h"
#include "ace/Task.h"
#include "ace/Thread_Manager.h"
#include "rdbop/rdbop.h"
#include "utl/datetime.h"

#define PROCEDURE_DAY_STA		"phis_staticsday "		//��ͳ�ƴ洢����
#define PROCEDURE_MONTH_STA		"phis_staticsmonth"		//��ͳ�ƴ洢����
#define PROCEDURE_SEASON_STA	"phis_staticsseason"	//��ͳ�ƴ洢����
#define PROCEDURE_YEAR_STA		"phis_staticsyear"		//��ͳ�ƴ洢����
#define PROCEDURE_KWH_DAY_STA		"phis_daykwh"		//�յ���ͳ�ƴ洢����
#define PROCEDURE_KWH_MONTH_STA		"phis_monthkwh"		//�µ���ͳ�ƴ洢����
#define PROCEDURE_KWH_YEAR_STA		"phis_yearkwh"		//�����ͳ�ƴ洢����

#define INTERVAL_CHECK_STA		10						//ͳ�ƴ洢���̼����(10s)

#define	DT_FMT_STA			"yyyy-MM-dd"				//ͳ��ʱ���ʽ

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

	//����ת��
	int			season(const PDT::CDateTime& dateTime);
private:
	PDT::CIntervalTimerSet	m_timer;
	int						m_timeId;
	PDT::CRdbOp*			m_pRdbOp;
};

typedef ACE_Singleton<CDbProcedureTask, ACE_Null_Mutex> TASK_DBPROCEDURE;

#endif	//_PDT_SCD_DBPROCEDURE_TASK_H_

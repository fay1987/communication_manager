#ifndef _PDT_SCD_LIMIT_TASK_H_
#define _PDT_SCD_LIMIT_TASK_H_

#include "utl/intervaltimerset.h"
#include "ace/Task.h"
#include "ace/Thread_Manager.h"
#include "dac/comminf.h"
#include "dac/datainf.h"
#include "event/csea.h"

class CLimitTask : public ACE_Task<ACE_MT_SYNCH>
{
public:
	CLimitTask();
	virtual ~CLimitTask();
public:
	bool		init();
	int			start();
	void		stop();
	int			svc();
protected:
	//yc
	void		ycCheck();
	void		overLimitCheck(hInt32 group,const DAC_YC_CHG_DATA& chgData);
	void		overLimitEventProc(hInt32 group,const DAC_YC_CHG_DATA& chgData);
	bool		createYcEvent(hInt32 group,hInt32 limitType,const DAC_YC_CHG_DATA& chgData);
	//yx
	void		yxEvent();
	bool		createYxEvent(hInt32 group,const DAC_YX_CHG_DATA& chgData);
private:
	PDT::CIntervalTimerSet	m_timer;
	int						m_timeId;

	PDT::CCommInf			m_commInf;
	PDT::CDataInf			m_dataInf;
	bool					m_isLimitTypeChange;	//限值类型是否变化,如:normal-->high
	hUInt32					m_curCycleTime;			//本循环的当前的时间
	PDT::CSysEvent			m_event;

};

typedef ACE_Singleton<CLimitTask, ACE_Null_Mutex> TASK_LIMIT;

#endif	//_PDT_SCD_LIMIT_TASK_H_

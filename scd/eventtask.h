#ifndef _PDT_SCD_EVENT_TASK_H_
#define _PDT_SCD_EVENT_TASK_H_

#include "utl/intervaltimerset.h"
#include "ace/Task.h"
#include "ace/Thread_Manager.h"
#include "dac/comminf.h"
#include "dac/datainf.h"
#include "event/csea.h"

class CEventTask : public ACE_Task<ACE_MT_SYNCH>
{
public:
	CEventTask();
	virtual ~CEventTask();
public:
	bool		init();
	int			start();
	void		stop();
	int			svc();
protected:
	void		checkSaveEvent();
	bool		putEvent(const PDT::nSysEvent& data);
private:
	PDT::CIntervalTimerSet	m_timer;
	int						m_timeId;
	PDT::CCommInf			m_commInf;
	PDT::CDataInf			m_dataInf;
	PDT::CSysEvent			m_event;
};

typedef ACE_Singleton<CEventTask, ACE_Null_Mutex> TASK_EVENT;

#endif	//_PDT_SCD_EVENT_TASK_H_

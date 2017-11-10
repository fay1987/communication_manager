#ifndef _PDT_SCD_H_
#define _PDT_SCD_H_

#include "sys/blog.h"
#include "sys/procman.h"
#include "utl/intervaltimerset.h"

#define PERIOD_CHECK_EISD	1		//¼ì²âeisdÖÜÆÚ(1s)

class CScada
{
public:
	CScada();
	virtual ~CScada();
	bool	init();
	bool	run();
private:
	PDT::CProcessManage		m_procMan;
	PDT::CIntervalTimerSet	m_timer;
	int						m_timeId;
};

#endif	//_PDT_SCD_H_

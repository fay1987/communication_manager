#ifndef	_PDT_DAC_LOADTASK_H_
#define	_PDT_DAC_LOADTASK_H_

#include "dac/comminf.h"
#include "dac/datainf.h"
#include "dac/loadpara.h"
#include "sys/procman.h"

#include <ace/Time_Value.h>
#include <ace/Task.h>
#include <ace/Thread_Mutex.h>
#include "statetask.h"
#include "ctrltask.h"
#include "linktaskpool.h"
#include "serialtaskpool.h"
#include "utl/intervaltimerset.h"
#include "serialwirelesstask.h"

using namespace PDT;

class CLoadTask : public ACE_Task<ACE_NULL_SYNCH>
{
public:
	CLoadTask();
	virtual ~CLoadTask();
	bool	init();
	int		start();
	void	stop();

	//���������
	hInt32	svc();
protected:
	void	doEveryDay();	//ÿ��0��0��0�����һЩ����
private:
	CCommInf		m_commInf;
	CDataInf		m_dataInf;
	CLoadPara		m_loadPara;
	CDateTime		m_curDateTime;

	CSerialTaskPool	m_serialTaskPool;
	CSerialWTask	m_serialWTask;
	CLinkTaskPool	m_linkTaskPool;
	CStateTask		m_stateTask;
	CCtrlTask		m_ctrlTask;

	//��Լ�̳߳�heart�ж�
	CIntervalTimerSet	m_timer;
	int					m_timeId;

	int m_timeOutID;
};

#endif //_PDT_DAC_LOADTASK_H_

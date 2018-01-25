/*==============================================================================
* �ļ����� : processchecktask.cpp
* ģ�� : ��EISD�Ǽ�ע��
* ����ʱ�� : 2009-07-22 15:55:23
* ���� : BBB
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#include "processchecktask.h"
#include <ace/OS.h>
#include "sys/blog.h"
#include "dac/dacdef.h"
#include <QtCore/QCoreApplication>

using namespace PDT;

CProcessCheckTask::CProcessCheckTask()
{
#ifdef PDT_DEBUG
	m_isDebug = true;
#else
	m_isDebug = false;
#endif
}

CProcessCheckTask::~CProcessCheckTask()
{
	if ( !m_isDebug ) m_procMan.endProc();
}

bool CProcessCheckTask::init(const char* procName,const char* procDesc,int procHwd,int logId)
{
	if ( procName == NULL ) return false;

	m_procName = procName;
	m_procDesc = procDesc;
	m_procHwd = procHwd;
	m_logId = logId;

	if ( m_isDebug ) return true;

	if( m_procMan.isProcExist(procName) == TRUE )
	{
		logprint(m_logId,"<%s> �Ѿ�����!",procName);
		return false;
	}

	//֪ͨeisd��������
	#ifdef	WIN32
		m_procMan.startProc(procName,procDesc,(HWND)procHwd);	//10001
	#else
		m_procMan.startProc(procName,procDesc);
	#endif

	return true;
}

void CProcessCheckTask::beginInitSystem()
{
	if ( !m_isDebug ) m_procMan.beginInitProc();
}

void CProcessCheckTask::endInitSystem()
{
	if ( !m_isDebug ) m_procMan.endInitProc();
}

int CProcessCheckTask::start()
{
	if ( m_isDebug ) return 0;

	logprint( m_logId,"<%s> <CProcessCheckTask>:����һ���߳�!",m_procName.c_str() );
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

void CProcessCheckTask::stop()
{
	if (thr_count() > 0) 
	{
		thr_mgr()-> cancel_task(this);
		wait();
	}
}

int CProcessCheckTask::svc()
{
	m_timeId = m_timer.add(1000*PROCESS_CHECK_TIME);	//1000ms
	m_timer.start();

	ACE_Time_Value tv(0,10000);							//10ms
	while (true)
	{
		if ( thr_mgr()->testcancel( thr_mgr()->thr_self() ) )
			break;
		ACE_OS::sleep( tv );

		if ( m_timer.time_out(m_timeId) )
		{
			if ( m_isDebug ) continue;
			if ( m_procMan.checkEisd() == FALSE )
			{
				logprint(m_logId,"checkEisdʧ�ܣ�%s �����˳�!",m_procName.c_str());
				qApp->quit();
				break;
			}
		}//end if(m_timer)
	}//end while

	logprint(m_logId,"%s threadid = %d ,processCheckTask �߳��˳�!",m_procName.c_str(),ACE_Thread::self());
	return 0;
}

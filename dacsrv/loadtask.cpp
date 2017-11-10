#include "loadtask.h"
#include "dac/dacdef.h"
#include <ace/OS.h>
#include "config.h"
#include <QtCore/QCoreApplication>	//wfp add at 20101129

using namespace PDT;

CLoadTask::CLoadTask()
{
	m_curDateTime.update(ACE_OS::gettimeofday());
}

CLoadTask::~CLoadTask()
{
}

bool CLoadTask::init()
{
	//wfp add at 20110602 for ��ʾ�ɼ�ϵͳδ��ʼ���ɹ�
	m_commInf.systemInfo()->initFlag = false;
	logprint(LOG_DACSRV_BASE,"<dacsrv>����� ���ݲ��� + ����洢������Ϣ !");
	m_dataInf.resetAllDataPara();
	m_commInf.resetAllCommInfo();

	//���ز���
	if ( !m_loadPara.loadAll() )
	{
		return false;
	}

	logprint(LOG_DACSRV_BASE,"����������ݵ���������Ϣ !");
	m_dataInf.resetAllDataInfo();

	m_commInf.systemInfo()->initFlag = true;
	return true;
}

int CLoadTask::start()
{
	logprint(LOG_DACSRV_BASE,"thread:%d , dacsrv��������!",ACE_Thread::self());
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

void CLoadTask::stop()
{
	m_stateTask.stop();
	m_ctrlTask.stop();
	m_linkTaskPool.stop();
	m_serialTaskPool.stop();
	m_serialWTask.stop();

	if (thr_count() > 0) thr_mgr()->cancel_task(this);
}

hInt32 CLoadTask::svc()
{
	DAC_SYSTEM_INFO* pSystemInfo = m_commInf.systemInfo();
	ACE_Thread_Manager *mgr = this->thr_mgr ();
	ACE_Time_Value tv(0,10000);

	//��������߳�
	m_serialTaskPool.init();
	m_serialTaskPool.start();

	m_serialWTask.init();
	m_serialWTask.start();

	m_linkTaskPool.start();

	if ( !m_ctrlTask.init() )
		logprint(LOG_DACSRV_BASE,"dacsrv: ctrlTask  ��ʼ��ʧ��!");
	else
		m_ctrlTask.start();

	m_stateTask.start();

	//wfp add for ��Լheart���
	m_timeId = m_timer.add(1000);	//1s
	
#ifdef PDT_DEBUG
	m_timeOutID = m_timer.add(1500);
#else	
	m_timeOutID = m_timer.add(500);
#endif //end PDT_DEBUG
	m_timer.start();
	
	while ( 1 )
	{
		if (mgr->testcancel (mgr->thr_self ()))
			break;

		ACE_OS::sleep(tv);

		//wfp add for ��Լheart���
		#ifdef PDT_DEBUG
		#else
		if ( m_timer.time_out(m_timeId) )
		{
			if ( m_linkTaskPool.checkDeadHeart() )
			{
				logprint(LOG_DACSRV_BASE,"��Լ�߳����г�ʱ�������˳�!");
				errprint(LOG_DACSRV_BASE,"��Լ�߳����г�ʱ�������˳�!");
				if ( CONFIG::instance()->deadCheck() ) 
				{
					qApp->quit();
				}
			}
		}//endif time_out
		#endif

		//add by yaoff	����ͬʱ���ͣ�ͬʱ����
		if (m_timer.time_out(m_timeOutID))
		{
			m_linkTaskPool.setTimeOutFlag(true);
			m_timer.start();
		}


		//add end.

		//װ�ز�����
		if(pSystemInfo->loadParaFlag != 0)
		{
			logprint(LOG_DACSRV_BASE,"dacsrv:�����б仯:%p��׼������װ��!",pSystemInfo);

			//station����
			if(pSystemInfo->loadParaFlag & LOADPARA_STATION)
			{
				m_loadPara.loadStation();
				pSystemInfo->loadParaFlag &= ~LOADPARA_STATION;
				pSystemInfo->loadParaFlag |= LOADPARA_GROUP;
				m_loadPara.loadGroup(false);
				pSystemInfo->loadParaFlag &= ~LOADPARA_GROUP;
			}
			//channel����
			if(pSystemInfo->loadParaFlag & LOADPARA_CHANNEL)
			{
				logprint(LOG_DACSRV_BASE,"dacsrv:����װ��channel�� !");
				m_loadPara.loadChannel();
				pSystemInfo->loadParaFlag &= ~LOADPARA_CHANNEL;
				pSystemInfo->loadParaFlag |=LOADPARA_ROUTE;
				m_loadPara.loadRoute();
				pSystemInfo->loadParaFlag &= ~LOADPARA_ROUTE;
				
				m_serialTaskPool.start();
				m_serialWTask.start();
				m_linkTaskPool.start();	
			}
			//Group����
			if(pSystemInfo->loadParaFlag & LOADPARA_GROUP)
			{
				m_loadPara.loadGroup();
				m_loadPara.loadSendDev();
				pSystemInfo->loadParaFlag &= ~LOADPARA_GROUP;
				pSystemInfo->loadParaFlag |= LOADPARA_ROUTE;
				m_loadPara.loadRoute();
				pSystemInfo->loadParaFlag &= ~LOADPARA_ROUTE;
			}
			//Route����
			if(pSystemInfo->loadParaFlag & LOADPARA_ROUTE)
			{
				m_loadPara.loadRoute();
				pSystemInfo->loadParaFlag &= ~LOADPARA_ROUTE;
			}
			//Protocol����
			if(pSystemInfo->loadParaFlag & LOADPARA_PROTOCOL)
			{
				m_loadPara.loadProtocol();
				pSystemInfo->loadParaFlag &= ~LOADPARA_PROTOCOL;
				pSystemInfo->loadParaFlag |= LOADPARA_ROUTE;
				m_loadPara.loadRoute();
				pSystemInfo->loadParaFlag &= ~LOADPARA_ROUTE;
			}
			//ң��
			if(pSystemInfo->loadParaFlag & LOADPARA_YK)
			{
				m_loadPara.loadYk();
				pSystemInfo->loadParaFlag &= ~LOADPARA_YK;
			}
			//ң��
			if(pSystemInfo->loadParaFlag & LOADPARA_YT)
			{
				m_loadPara.loadYt();
				pSystemInfo->loadParaFlag &= ~LOADPARA_YT;
			}
			/*wfp removed to ctrltask
			//YC
			if(pSystemInfo->loadParaFlag & LOADPARA_YC)
			{
				m_loadPara.loadYc();
				pSystemInfo->loadParaFlag &= ~LOADPARA_YC;
				pSystemInfo->loadParaFlag |= LOADPARA_ZFYC;
			}
			//Yx
			if(pSystemInfo->loadParaFlag & LOADPARA_YX)
			{
				m_loadPara.loadYx();
				pSystemInfo->loadParaFlag &= ~LOADPARA_YX;
				pSystemInfo->loadParaFlag |= LOADPARA_ZFYX;
			}
			//KWH
			if(pSystemInfo->loadParaFlag & LOADPARA_KWH)
			{
				m_loadPara.loadKwh();
				pSystemInfo->loadParaFlag &= ~LOADPARA_KWH;
				pSystemInfo->loadParaFlag |= LOADPARA_ZFKWH;
			}
			*/
		}//end if(loadParaFlag)

		doEveryDay();
	}//end while

	logprint(LOG_DACSRV_BASE,"threadId = %d , dacsrv�߳��˳�!",ACE_Thread::self());

	return 0;
}

//ÿ��0��0��0�����һЩ����
void CLoadTask::doEveryDay()
{
	if ( CDateTime::currentDateTime().day() == m_curDateTime.day() ) return;

	m_curDateTime = CDateTime::currentDateTime();

	hUInt32 i = 0;
	for (i=0;i<m_commInf.systemInfo()->channelNum;i++)
	{
		m_commInf.channelInfo(i,false)->dayOkTime = 0;
		m_commInf.channelInfo(i,false)->dayErrCnt = 0;
	}

	for (i=0;i<m_commInf.systemInfo()->groupNum;i++)
	{
		m_commInf.groupInfo(i,false)->dayOkTime = 0;
		m_commInf.groupInfo(i,false)->dayErrCnt = 0;
	}

	for (i=0;i<m_commInf.systemInfo()->routeNum;i++)
	{
		m_commInf.routeInfo(i,false)->dayOkTime = 0;
		m_commInf.routeInfo(i,false)->dayErrCnt = 0;
	}

}


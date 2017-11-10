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
	//wfp add at 20110602 for 表示采集系统未初始化成功
	m_commInf.systemInfo()->initFlag = false;
	logprint(LOG_DACSRV_BASE,"<dacsrv>，清空 数据参数 + 共享存储公共信息 !");
	m_dataInf.resetAllDataPara();
	m_commInf.resetAllCommInfo();

	//加载参数
	if ( !m_loadPara.loadAll() )
	{
		return false;
	}

	logprint(LOG_DACSRV_BASE,"清空所有数据的质量码信息 !");
	m_dataInf.resetAllDataInfo();

	m_commInf.systemInfo()->initFlag = true;
	return true;
}

int CLoadTask::start()
{
	logprint(LOG_DACSRV_BASE,"thread:%d , dacsrv处理启动!",ACE_Thread::self());
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

	//启动相关线程
	m_serialTaskPool.init();
	m_serialTaskPool.start();

	m_serialWTask.init();
	m_serialWTask.start();

	m_linkTaskPool.start();

	if ( !m_ctrlTask.init() )
		logprint(LOG_DACSRV_BASE,"dacsrv: ctrlTask  初始化失败!");
	else
		m_ctrlTask.start();

	m_stateTask.start();

	//wfp add for 规约heart检测
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

		//wfp add for 规约heart检测
		#ifdef PDT_DEBUG
		#else
		if ( m_timer.time_out(m_timeId) )
		{
			if ( m_linkTaskPool.checkDeadHeart() )
			{
				logprint(LOG_DACSRV_BASE,"规约线程运行超时，正在退出!");
				errprint(LOG_DACSRV_BASE,"规约线程运行超时，正在退出!");
				if ( CONFIG::instance()->deadCheck() ) 
				{
					qApp->quit();
				}
			}
		}//endif time_out
		#endif

		//add by yaoff	控制同时发送，同时接收
		if (m_timer.time_out(m_timeOutID))
		{
			m_linkTaskPool.setTimeOutFlag(true);
			m_timer.start();
		}


		//add end.

		//装载参数表
		if(pSystemInfo->loadParaFlag != 0)
		{
			logprint(LOG_DACSRV_BASE,"dacsrv:参数有变化:%p，准备重新装载!",pSystemInfo);

			//station参数
			if(pSystemInfo->loadParaFlag & LOADPARA_STATION)
			{
				m_loadPara.loadStation();
				pSystemInfo->loadParaFlag &= ~LOADPARA_STATION;
				pSystemInfo->loadParaFlag |= LOADPARA_GROUP;
				m_loadPara.loadGroup(false);
				pSystemInfo->loadParaFlag &= ~LOADPARA_GROUP;
			}
			//channel参数
			if(pSystemInfo->loadParaFlag & LOADPARA_CHANNEL)
			{
				logprint(LOG_DACSRV_BASE,"dacsrv:正在装载channel表 !");
				m_loadPara.loadChannel();
				pSystemInfo->loadParaFlag &= ~LOADPARA_CHANNEL;
				pSystemInfo->loadParaFlag |=LOADPARA_ROUTE;
				m_loadPara.loadRoute();
				pSystemInfo->loadParaFlag &= ~LOADPARA_ROUTE;
				
				m_serialTaskPool.start();
				m_serialWTask.start();
				m_linkTaskPool.start();	
			}
			//Group参数
			if(pSystemInfo->loadParaFlag & LOADPARA_GROUP)
			{
				m_loadPara.loadGroup();
				m_loadPara.loadSendDev();
				pSystemInfo->loadParaFlag &= ~LOADPARA_GROUP;
				pSystemInfo->loadParaFlag |= LOADPARA_ROUTE;
				m_loadPara.loadRoute();
				pSystemInfo->loadParaFlag &= ~LOADPARA_ROUTE;
			}
			//Route参数
			if(pSystemInfo->loadParaFlag & LOADPARA_ROUTE)
			{
				m_loadPara.loadRoute();
				pSystemInfo->loadParaFlag &= ~LOADPARA_ROUTE;
			}
			//Protocol参数
			if(pSystemInfo->loadParaFlag & LOADPARA_PROTOCOL)
			{
				m_loadPara.loadProtocol();
				pSystemInfo->loadParaFlag &= ~LOADPARA_PROTOCOL;
				pSystemInfo->loadParaFlag |= LOADPARA_ROUTE;
				m_loadPara.loadRoute();
				pSystemInfo->loadParaFlag &= ~LOADPARA_ROUTE;
			}
			//遥控
			if(pSystemInfo->loadParaFlag & LOADPARA_YK)
			{
				m_loadPara.loadYk();
				pSystemInfo->loadParaFlag &= ~LOADPARA_YK;
			}
			//遥调
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

	logprint(LOG_DACSRV_BASE,"threadId = %d , dacsrv线程退出!",ACE_Thread::self());

	return 0;
}

//每天0点0分0秒清除一些计数
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


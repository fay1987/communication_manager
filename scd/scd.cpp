#include "sys/procman.h"
#include "scd.h"
#include "scddef.h"
#include <ace/Task.h>
#include "hisdatatask.h"
#include "dbsavetask.h"
#include "config.h"
#include "filereadtask.h"
#include "eventtask.h"
#include "sys/plat_api.h"
#include "limittask.h"
#include "dbproceduretask.h"
#include "loadtask.h"
#include "calctask.h"

#ifdef WIN32
#include "tray.h"
#include <QApplication>
#include <QMessageBox>
#include <QtCore/QtCore>
#include <QtCore/QString>
#endif

using namespace PDT;

CScada::CScada()
{
}

CScada::~CScada()
{
	//结束任务
	TASK_LOAD::instance()->stop();
	if ( CONFIG::instance()->isRunTask_dbsave() )
	{
		TASK_DBSAVE::instance()->stop();
	}
	
	if ( CONFIG::instance()->isRunTask_hisdata() )
	{
		TASK_HISDATA::instance()->stop();
	}

	if ( CONFIG::instance()->isRunTask_fileread() )
	{
		TASK_FILEREAD::instance()->stop();
	}

	if ( CONFIG::instance()->isRunTask_calc() )
	{
		TASK_CALC::instance()->stop();
	}

	if ( CONFIG::instance()->isRunTask_limit() )
	{
		TASK_LIMIT::instance()->stop();
	}
	
	if ( CONFIG::instance()->isExecProcedure() )
	{
		TASK_DBPROCEDURE::instance()->stop();
	}
	//wfp masked at 20110616
	//TASK_EVENT::instance()->stop();
	

	logprint(LOG_SCD_BASE,"<scd>is stoping task");
	ACE_Time_Value tvWait(3,0);
	if ( ACE_Thread_Manager::instance()->wait(&tvWait,0,0) == -1 )
	{
		logprint(LOG_SCD_BASE,"<scd>stop error");
		m_procMan.endProc();
		exit(0);	
	}
	m_procMan.endProc();
}

bool CScada::init()
{
	logprint(LOG_SCD_BASE,"准备启动SCADA服务进程scdsrv !");
	if( m_procMan.isProcExist("scdsrv") == TRUE )
	{
		logprint(LOG_SCD_BASE,"SCADA服务进程 scdsrv 已经启动 !");
		return false;
	}

	if ( !CONFIG::instance()->read() ) return false;

#ifdef PDT_DEBUG
#else

#ifdef	WIN32
	m_procMan.startProc("scdsrv","SCADA服务",(HWND)PROCHANDLE_SCDSRV);
#else
	m_procMan.startProc("scd","存盘进程");  
#endif //end WIN32

#endif //end PDT_DEBUG

	m_timeId = m_timer.add(PERIOD_CHECK_EISD*1000);	//1s
	m_timer.start();

	m_procMan.beginInitProc();

	TASK_LOAD::instance()->init();
	TASK_LOAD::instance()->start();

	//启动任务
	if ( CONFIG::instance()->isRunTask_dbsave() )
	{
		TASK_DBSAVE::instance()->init();
		TASK_DBSAVE::instance()->start();
	}

	if ( CONFIG::instance()->isRunTask_hisdata() )
	{
		TASK_HISDATA::instance()->init();
		TASK_HISDATA::instance()->start();
	}

	if ( CONFIG::instance()->isRunTask_fileread() )
	{
		TASK_FILEREAD::instance()->init();
		TASK_FILEREAD::instance()->start();
	}

	if ( CONFIG::instance()->isRunTask_calc() )
	{
		TASK_CALC::instance()->init();
		TASK_CALC::instance()->start();
	}


	//wfp masked at 20110616
	//TASK_EVENT::instance()->init();
	//TASK_EVENT::instance()->start();
	if ( CONFIG::instance()->isRunTask_limit() )
	{	
		TASK_LIMIT::instance()->init();
		TASK_LIMIT::instance()->start();
	}

	if ( CONFIG::instance()->isExecProcedure() )
	{
		TASK_DBPROCEDURE::instance()->init();
		TASK_DBPROCEDURE::instance()->start();
	}

	m_procMan.endInitProc();
	return true;
}

bool CScada::run()
{
#ifdef PDT_DEBUG
#else
	if ( m_timer.time_out(m_timeId) )
	{
		if ( m_procMan.checkEisd() == FALSE )
		{
			logprint(LOG_SCD_BASE,"check平台失败，scdsrv正在退出");
			return false;
		}
	}
#endif //end PDT_DEBUG
	return true;
}


#if defined(WIN32)
int ACE_TMAIN(int argc,char* argv[])
{
	//QTextCodec * codec = QTextCodec::codecForName("GB2312");
	QTextCodec * codec = QTextCodec::codecForLocale();
	if (codec)
	{
		QTextCodec::setCodecForLocale(codec);
		QTextCodec::setCodecForTr(codec);
		QTextCodec::setCodecForCStrings(codec);		
	}

	QApplication app(argc, argv);

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("Scdtray"),
			QObject::tr("I couldn't detect any system tray "
			"on this system."));
		return 1;
	}
	QApplication::setQuitOnLastWindowClosed(false);

	CTray tray;
	if ( !tray.init() ) return -1;

	return app.exec();
}

#else //unix

bool g_exitFlag = false;

void	signal_function(int type)
{
	if( g_exitFlag )
	{ 
		return;
	}
	switch(type)
	{
	case	SIGKILL:
	case	SIGTERM: 
		g_exitFlag = true;
		break;
	case	SIGCHLD: 
		kill_defunct_child_proc();
	default:
		break;
	}
}

int ACE_TMAIN(int argc,char* argv[])
{
#ifdef PDT_DEBUG
#else
	//将进程放入后台运行 
	if( procToDaemon() == FALSE)
	{
		printf("procToDaemon() error");
		//bLastErrDisp("procToDaemon()");   
		exit(0);
	} 
	sigset(SIGKILL,signal_function);
	sigset(SIGTERM,signal_function);
	sigset(SIGCHLD,signal_function);
#endif //end PDT_DEBUG

	CScada scada;
	if ( !scada.init() ) 
		return -1;

	while ( !g_exitFlag )
	{
		if ( !scada.run() ) break;
		ACE_OS::sleep( ACE_Time_Value(0,10000) );	//10ms
	}
	printf("scada成功退出\n");
	return 1;
}
	
#endif //end __unix

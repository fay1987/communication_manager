#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <signal.h>
#include "channelmanager.h"
#include "dac/dacdef.h"
#include "loadtask.h"
#include "statetask.h"
#include "ctrltask.h"
#include "linktaskpool.h"
#include "config.h"
#include "sys/plat_api.h"
#include "processchecktask.h"

#ifdef WIN32
#include "tray.h"
#include <QApplication>
#include <QMessageBox>
#else
#include <QtCore/QCoreApplication>
#include "processcheck.h"
#endif

using namespace  PDT;

CChannelManager* g_pChannelMgr = NULL;
#ifdef WIN32
QApplication* pApp = NULL;
#else
QCoreApplication* pApp = NULL;
#endif

//由于参数加载等时间较长，容易被eisd误杀，故启动一个临时线程进行初始化
class CDacSrv
{
public:
	CDacSrv(){};
	virtual ~CDacSrv()
	{
		stopSystem();
	}

	bool startSystem()
	{
		if ( !CONFIG::instance()->read() ) return false;

		if ( !m_loadTask.init() ) return false;

		m_loadTask.start();

		logprint(LOG_DACSRV_BASE,"<dacsrv>程序启动成功!\n");
		return true;
	}

	void stopSystem()
	{
		m_loadTask.stop();

		logprint(LOG_DACSRV_BASE,"<dacsrv>is stoping task...");
		ACE_Time_Value tvWait(3,0);
		if ( ACE_Thread_Manager::instance()->wait(&tvWait,0,0) == -1 )
		{
			logprint(LOG_DACSRV_BASE,"<dacsrv>stop error");
			exit(0);	
		}
	}
private:
	CLoadTask		m_loadTask;
};

#if defined(WIN32)

int ACE_TMAIN(int argc, char *argv[])
{
	//QTextCodec * codec = QTextCodec::codecForName("GB2312");
	QTextCodec * codec = QTextCodec::codecForLocale();
	if (codec)
	{
		QTextCodec::setCodecForLocale(codec);
		QTextCodec::setCodecForTr(codec);
		QTextCodec::setCodecForCStrings(codec);		
	}

	//wfp add
	pApp = new QApplication(argc,argv);

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("dacsrv"),
			QObject::tr("I couldn't detect any system tray "
			"on this system."));
		return 1;
	}
	QApplication::setQuitOnLastWindowClosed(false);

	CTray tray;
	CProcessCheckTask checkTask;
	if ( !checkTask.init("dacsrv","数据采集程序",PROCHANDLE_DACSRV,LOG_DACSRV_BASE) ) return -1;
	checkTask.beginInitSystem();

	CDacSrv dacsrv;
	if ( !dacsrv.startSystem() )
	{
		logprint(LOG_DACSRV_BASE,"<loadtask>初始化失败，正在退出!");
		return -1;
	}

	//启动网络
	CChannelManager channelMgr;
	g_pChannelMgr = &channelMgr;
	g_pChannelMgr->init();
	g_pChannelMgr->startSendDataTimer( CONFIG::instance()->netSendCheckInterval() );

	checkTask.endInitSystem();
	checkTask.start();
	logprint(LOG_DACSRV_BASE,"<dacsrv>程序启动成功!\n");
	pApp->exec();

	checkTask.stop();
	return 0;
}
#else

void	signal_function(int type)
{
	switch(type)
	{
	case	SIGKILL:
	case	SIGTERM: 
		if ( pApp != NULL ) pApp->quit();
		break;
	case	SIGCHLD: 
		kill_defunct_child_proc();
	default:
		break;
	}
}

int ACE_TMAIN(int argc, char *argv[])
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

	QTextCodec * codec = QTextCodec::codecForName("GB2312");
	if (codec)
	{
		QTextCodec::setCodecForLocale(codec);
		QTextCodec::setCodecForTr(codec);
		QTextCodec::setCodecForCStrings(codec);		
	}

	pApp = new QCoreApplication(argc,argv);

	CProcessCheckTask checkTask;
	if ( !checkTask.init("dacsrv","数据采集程序",PROCHANDLE_DACSRV,LOG_DACSRV_BASE) ) return -1;
	checkTask.beginInitSystem();

	//CProcessCheck processCheck;
	//if ( !processCheck.init() ) return -1;
	//processCheck.beginInitSystem();

	CDacSrv dacsrv;
	if ( !dacsrv.startSystem() )
	{
		logprint(LOG_DACSRV_BASE,"<loadtask>初始化失败，正在退出!");
		return -1;
	}

	//启动网络
	CChannelManager channelMgr;
	g_pChannelMgr = &channelMgr;
	g_pChannelMgr->init();
	g_pChannelMgr->startSendDataTimer( CONFIG::instance()->netSendCheckInterval() );

	//processCheck.endInitSystem();
	checkTask.endInitSystem();		//wfp changed at 20110714 from beginInitSystem
	checkTask.start();
	logprint(LOG_DACSRV_BASE,"dacsrv 程序启动成功!\n");
	pApp->exec();

	checkTask.stop();
	return 0;
}
#endif



#include "processcheck.h"
#include "dac/dacdef.h"

CProcessCheck::CProcessCheck()
{
	m_pCheckTimer = new QTimer(this);
	m_pCheckTimer->start(1000);		//1000ms
	connect( m_pCheckTimer,SIGNAL( timeout() ),this, SLOT( onCheckTimeOut() ) );
}

CProcessCheck::~CProcessCheck()
{
	m_procMgr.endProc();
}

bool CProcessCheck::init()
{
#ifdef PDT_DEBUG
#else
	//检查进程是否已经启动
	if( m_procMgr.isProcExist("dacsrv") == TRUE )
	{
		logprint(PDT::LOG_DACSRV_BASE,"dacsrv 已经启动 !");
		return false;
	}

	//通知eisd进程启动
#if defined(WIN32)
	m_procMgr.startProc("dacsrv","采集服务",(HWND)10001);
#else
	m_procMgr.startProc("dacsrv","采集服务");
#endif

#endif //end PDT_DEBUG

	return true;
}

void CProcessCheck::beginInitSystem()
{
	m_procMgr.beginInitProc();
}

void CProcessCheck::endInitSystem()
{
	m_procMgr.endInitProc();
}

void CProcessCheck::onCheckTimeOut()
{
#ifdef PDT_DEBUG
#else
	if ( m_procMgr.checkEisd() == FALSE )
	{
		logprint(PDT::LOG_DACSRV_BASE,"checkEisd失败，dacsrv正在退出");
		qApp->quit();
	}
#endif
}

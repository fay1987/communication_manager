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
	//�������Ƿ��Ѿ�����
	if( m_procMgr.isProcExist("dacsrv") == TRUE )
	{
		logprint(PDT::LOG_DACSRV_BASE,"dacsrv �Ѿ����� !");
		return false;
	}

	//֪ͨeisd��������
#if defined(WIN32)
	m_procMgr.startProc("dacsrv","�ɼ�����",(HWND)10001);
#else
	m_procMgr.startProc("dacsrv","�ɼ�����");
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
		logprint(PDT::LOG_DACSRV_BASE,"checkEisdʧ�ܣ�dacsrv�����˳�");
		qApp->quit();
	}
#endif
}

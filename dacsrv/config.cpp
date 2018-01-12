#include "config.h"
#include "dac/dacdef.h"
#include <QtCore/QtCore>
#include <QtCore/QString>

using namespace  PDT;

CConfig::CConfig()
{
	m_linkThreadPoolSize = 0;
	m_netThreadPoolSize = 0;
	m_qualityInvalidInterval = 0;							//默认不检查
	m_netSendCheckInterval	= MIN_NETSEND_CHECK_INTERVAL;	//默认为最小间隔
	m_deadCheck = false;									//默认不检测
	//event 默认不过滤
	m_filterChannel = false;
	m_filterRoute = false;
	m_filterGroup = false;
	m_filterBegin = false;
	m_timeinterval = 0;
}

CConfig::~CConfig()
{

}

bool CConfig::read() 
{
	QString sysRoot = QString("%1").arg( ACE_OS::getenv(SYS_ENV_VAR) );
	QDir dir( sysRoot );
	if ( !dir.cd("ini") )
	{
		logprint(LOG_DACSRV_CONFIG,"Can't find the ini directory,please check it");
		return false;
	}	

	QString fileName;
	fileName = dir.absolutePath() + "/";
	fileName += DACSRV_CONFIG_FILE;

	if ( !QFile::exists(fileName) ) 
	{
		logprint(LOG_DACSRV_CONFIG,"配置文件<%s>不存在",DACSRV_CONFIG_FILE);
		return false;
	}

	QSettings sysIni( fileName,QSettings::IniFormat );

	//read settings
	m_qualityInvalidInterval = sysIni.value("RUN_MODE/qualityinvalidinterval").toInt();
	m_netSendCheckInterval = sysIni.value("RUN_MODE/netsendcheckinterval").toInt();
	if ( m_netSendCheckInterval < MIN_NETSEND_CHECK_INTERVAL ) m_netSendCheckInterval = MIN_NETSEND_CHECK_INTERVAL;

	m_linkThreadPoolSize = sysIni.value("THREAD_POOL/linksize").toInt();
	if ( m_linkThreadPoolSize < 1 ) m_linkThreadPoolSize = 1;

	m_netThreadPoolSize = sysIni.value("THREAD_POOL/netsize").toInt();
	if ( m_netThreadPoolSize < 1 ) m_netThreadPoolSize = 1;

	m_serialThreadPoolSize = sysIni.value("THREAD_POOL/serialsize").toInt();
	if ( m_serialThreadPoolSize < 1 ) m_serialThreadPoolSize = 1; 

	m_deadCheck = (bool)sysIni.value("RUN_MODE/deadcheck").toInt();

	//event
	m_filterChannel = (bool)sysIni.value("EVENT_MODE/filterchannel").toInt();
	m_filterRoute = (bool)sysIni.value("EVENT_MODE/filterroute").toInt();
	m_filterGroup = (bool)sysIni.value("EVENT_MODE/filtergroup").toInt();
	m_filterBegin = (bool)sysIni.value("EVENT_MODE/filterbegin").toInt();
	
	if (sysIni.contains("REQUEST_INTERVAL/timeinterval"))
	{
		m_timeinterval = sysIni.value("REQUEST_INTERVAL/timeinterval").toInt();
	}

	logprint(LOG_DACSRV_CONFIG,"读取配置文件<%s>成功",DACSRV_CONFIG_FILE);

	return true;
}

hUInt32 CConfig::linkThreadPoolSize() const 
{
	return m_linkThreadPoolSize;
}

hUInt32 CConfig::netThreadPoolSize() const
{
	return m_netThreadPoolSize;
}

hUInt32 CConfig::serialThreadPoolSize() const
{
	return m_serialThreadPoolSize;
}

hUInt32 CConfig::qualityInvalidInterval() const
{
	return m_qualityInvalidInterval;
}

hUInt32 CConfig::netSendCheckInterval() const
{
	return m_netSendCheckInterval;
}

bool CConfig::deadCheck() const
{
	return m_deadCheck;
}

//event
bool CConfig::filterChannel() const
{
	return m_filterChannel;
}

bool CConfig::filterRoute() const
{
	return m_filterRoute;
}

bool CConfig::filterGroup() const
{
	return m_filterGroup;
}

bool CConfig::filterBegin() const
{
	return m_filterBegin;
}

hUInt32 CConfig::timeinterval() const
{
	return m_timeinterval;
}


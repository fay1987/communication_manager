#include "config.h"
#include <cstdlib>
#include "dac/dacdef.h"
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include "scddef.h"
#include "sys/blog.h"

using namespace  PDT;

#define MIN_STATIC_SAVE_INTERVAL	10		//��쾲̬���ݴ��̼��(10s)

CConfig::CConfig()
{
	m_isSaveWater = false;
	m_hasBattery = false;
	m_microNet = false;
	m_isExecProcedure = false;
	m_isSaveErrFile = false;

	m_isRunTask_dbsave = true;
	m_isRunTask_hisdata = true;
	m_isRunTask_fileread = true;
	m_isRunTask_calc = true;
	m_isRunTask_limit = true;
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
		logprint(LOG_SCD_BASE,"Can't find the ini directory,please check it");
		return false;
	}	

	QString fileName;
	fileName = dir.absolutePath() + "/";
	fileName += SCD_CONFIG_FILE;

	if ( !QFile::exists(fileName) ) 
	{
		logprint(LOG_SCD_BASE,"�����ļ�<%s>������",SCD_CONFIG_FILE);
		return false;
	}

	QSettings sysIni( fileName,QSettings::IniFormat );

	//read settings
	m_isSaveWater = sysIni.value("RUN_MODE/savewater").toInt();
	m_saveMode = sysIni.value("RUN_MODE/savemode").toInt();
	m_hasBattery = sysIni.value("RUN_MODE/hasbattery").toInt();
	m_microNet = sysIni.value("RUN_MODE/micronet").toInt();
	m_isExecProcedure = sysIni.value("RUN_MODE/execprocedure").toInt();
	m_isSaveErrFile = sysIni.value("RUN_MODE/saveerrorfile").toInt();
	m_isYcInSgz = sysIni.value("RUN_MODE/ycinsgz").toInt();

	if (sysIni.contains("TASK_LIST/task_dbsave"))
	{
		m_isRunTask_dbsave = sysIni.value("TASK_LIST/task_dbsave").toInt();
	}
	if (sysIni.contains("TASK_LIST/task_hisdata"))
	{
		m_isRunTask_hisdata = sysIni.value("TASK_LIST/task_hisdata").toInt();
	}
	if (sysIni.contains("TASK_LIST/task_fileread"))
	{
		m_isRunTask_fileread = sysIni.value("TASK_LIST/task_fileread").toInt();
	}
	if (sysIni.contains("TASK_LIST/task_calc"))
	{
		m_isRunTask_calc = sysIni.value("TASK_LIST/task_calc").toInt();
	}
	if (sysIni.contains("TASK_LIST/task_limit"))
	{
		m_isRunTask_limit = sysIni.value("TASK_LIST/task_limit").toInt();
	}

	logprint(LOG_SCD_BASE,"��ȡ�����ļ�<%s>�ɹ�",SCD_CONFIG_FILE);

	return true;
}

bool CConfig::isSaveWater() const
{
	return m_isSaveWater;
}

hUInt32	CConfig::saveMode() const
{
	return m_saveMode;
}

bool CConfig::hasBattery() const
{
	return m_hasBattery;
}

bool CConfig::microNet() const
{
	return m_microNet;
}

/*
 * ����˵����Ŀǰ�ա���ͳ�ƿɸ���f_isstat��ʶ����ͳ�ƣ���������ͳ���򲻹����ñ�ʶ
 *			 �����Ƹñ�־���ж��Ƿ�ִ�д洢���̵����߳�.
 */
bool CConfig::isExecProcedure() const
{
	return m_isExecProcedure;
}

bool CConfig::isSaveErrFile() const
{
	return m_isSaveErrFile;
}

bool CConfig::isYcInSgz() const
{
	return m_isYcInSgz;
}

bool CConfig::isRunTask_dbsave() const
{
	return m_isRunTask_dbsave;
}

bool CConfig::isRunTask_hisdata() const
{
	return m_isRunTask_hisdata;
}

bool CConfig::isRunTask_fileread() const
{
	return m_isRunTask_fileread;
}

bool CConfig::isRunTask_calc() const
{
	return m_isRunTask_calc;
}

bool CConfig::isRunTask_limit() const
{
	return m_isRunTask_limit;
}

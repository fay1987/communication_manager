#include "filemanager.h"
#include "sys/blog.h"
#include "utl/directory.h"
#include "utl/string.h"
#include "scddef.h"

using namespace  PDT;

CFileManager::CFileManager()
{
	m_isOpen = false;
	m_curRecords = 0;
	m_pFile = NULL;
}

CFileManager::~CFileManager()
{
	if ( m_isOpen )
	{
		closeFile();
		rename();
	}
}

bool CFileManager::init(const char* dir,const char* prefix,int maxRecords/* =SCD_MAX_RECORDS_INFILE */)
{
	m_dirName = CString(ACE_OS::getenv(SYS_ENV_VAR)) + "/" + CString(dir);
	m_prefix = prefix;

	if ( maxRecords <= 0 )
		m_maxRecords = SCD_MAX_RECORDS_INFILE;
	else
		m_maxRecords = maxRecords;

	CDir::makeDir(m_dirName.c_str());

	return true;
}

bool CFileManager::read() 
{
	//logprint(LOG_SCD_FILEREAD,"读取配置文件<%s>成功",SCD_CONFIG_FILE);

	return true;
}

bool CFileManager::save(const char* pData,int length)
{
	if( pData == NULL || length <= 0 ) return false;

	if ( !m_isOpen )
	{
		openNewFile();
	}

	if ( m_isOpen )
	{
		int nwrittenItems = ACE_OS::fwrite(pData,length,1,m_pFile);
		if ( nwrittenItems != 1 )
		{
			logprint(LOG_SCD_FILEWRITE,"fwrite error <%d != %d> real write>");
			closeFile();
			return false;
		}

		++m_curRecords;
		if ( m_curRecords >= m_maxRecords )
		{
			m_curRecords = 0;
			logprint(LOG_SCD_FILEWRITE,"文件所含记录数已达到上限");
			closeFile();
			rename();
		}
	}
	return true;
}

bool CFileManager::openNewFile()
{
	CString timeStr;
	timeStr.from( (hUInt32)ACE_OS::time(NULL) );
	m_tmpFName = m_dirName + CString("/") + m_prefix + timeStr + CString(".tmp");

	//fwrite说明：必须以二进制方式存数据库，否则可能额外写入文本字符.
	m_pFile = ACE_OS::fopen(m_tmpFName.c_str(),"w+b");	//wfp changed at 20100104 from "w+"
	if ( m_pFile == NULL ) return false;
	
	m_isOpen = true;
	return true;
}

bool CFileManager::closeFile()
{
	if ( m_pFile != NULL )
	{
		ACE_OS::fclose(m_pFile);
		m_pFile = NULL;
		m_isOpen = false;
	}
	return true;
}

bool CFileManager::rename()
{
	if( m_tmpFName.is_empty() ) return false;

	CString fname = m_tmpFName;
	fname.replace(".tmp",".data");

	if ( ACE_OS::rename(m_tmpFName.c_str(),fname.c_str()) == -1 )
	{
		fname = m_tmpFName;
		fname.replace(".tmp","T.data");
		if ( ACE_OS::rename(m_tmpFName.c_str(),fname.c_str()) == -1 ) 
		{
			logprint(LOG_SCD_FILEWRITE,"rename error! %s-->%s",m_tmpFName.c_str(),fname.c_str());
			return false;
		}
	}
	logprint(LOG_SCD_FILEWRITE,"rename sucess! %s-->%s",m_tmpFName.c_str(),fname.c_str());
	return true;
}

#include "sys/blog.h"
#include <ace/OS.h>
#include "sys/plat_api.h"

#ifdef WIN32
#include <windows.h>
#elif defined(__unix)
#include <errno.h>
#endif 

using namespace PDT;

PLAT_EXPORT	CLogInfo	g_logInfo;

BLOG_MEM	*CLogInfo::m_ptr = NULL;
char		CLogInfo::m_procName[SYS_PROCNAME_LEN];

CLogInfo::CLogInfo()
{
	if ( m_ptr == NULL )
	{
		bool isNetCreated = false;
		createAndMapShareMemory(BLOG_SHM_KEY,sizeof(BLOG_MEM),isNetCreated);
 		m_ptr = (BLOG_MEM*)shmPointer();
		if ( m_ptr == NULL )
		{
			perror("create memory error\n");
			exit(0);
		}
		if ( isNetCreated ) ACE_OS::memset(m_ptr,0,sizeof(BLOG_MEM));
	}
	//��ȡ�����������
	getProcName(ACE_OS::getpid(),m_procName);
	char *p = strrchr(m_procName,'.');
	if ( p != NULL ) *p = 0;

	m_readPos = m_ptr->writePos;
}

CLogInfo::~CLogInfo()
{
	m_ptr->lock = FALSE;
	m_ptr->flock = FALSE;
}

/*
 * ��������:ĸ�����������İ�ȫ,�������WriteLog�Ķ��̰߳�ȫ����.
 * ��������:��д����־��λ��
 * ˵    ��:���̵߳ȴ�ĸ��,���ڽ�ʹ���˱��������Ǻ�����,������ܵ��¼����߳�ͬʱ�г� lock=false������д�ڴ�.
 */
int	CLogInfo::lockWrite()
{
	int	pos = m_ptr->writePos;
	while(m_ptr->lock) ACE_OS::sleep(0);  //sleep(0)--�ȴ���С��ʱ��

	m_ptr->lock = TRUE;						//wfp_test
	while(true)
	{
		if(m_ptr->data[pos].lock)
		{
			pos = (pos + 1) % BLOG_MAX_NUM;
			ACE_OS::sleep(0);
			continue;
		}
		else	break;
	}
	m_ptr->data[pos].lock = TRUE;
	m_ptr->lock = FALSE;

	return	pos;
}

void	CLogInfo::unLockWrite(int pos)
{
	m_ptr->data[pos].lock = FALSE;
}

void CLogInfo::setDisplay(int type)
{
	if(type<0 || type>=BLOG_TYPE_NUM) return;
	m_ptr->flag[type] = TRUE;
}

void CLogInfo::clearDisplay(int type)
{
	if(type<0 || type>=BLOG_TYPE_NUM) return;
	m_ptr->flag[type] = FALSE;
}

void CLogInfo::setDisplayAll()
{
	for(int i=0;i<BLOG_TYPE_NUM;i++) m_ptr->flag[i] = TRUE;
	m_readPos = m_ptr->writePos;
}

void CLogInfo::clearDisplayAll()
{
	for(int i=0;i<BLOG_TYPE_NUM;i++) m_ptr->flag[i] = FALSE;
	m_readPos = m_ptr->writePos;
}

char *CLogInfo::readLog(int &type,char *loghead,BLOG_DATA **data,int flag)
{
	if(m_readPos == m_ptr->writePos)	return NULL;
	int oldReadPos = m_readPos;
	m_readPos = (m_readPos+1)%BLOG_MAX_NUM;

	if(loghead)
	{
		CDateTime* pDateTime = &(m_ptr->data[oldReadPos].dateTime);
		//��֤�������������ַ�������С��LOG_HEAD_LEN
		ACE_OS::snprintf(loghead,BLOG_HEAD_LEN,"[ %s ] , Type = %d,Process = %s\n", 
			m_ptr->data[oldReadPos].dateTime.toString(DT_FMT_DEF).c_str(),
			(int)m_ptr->data[oldReadPos].type,m_ptr->data[oldReadPos].procName);
	}
	if(data)	*data = &m_ptr->data[oldReadPos];
	
	////д�ļ�,��������÷ŵ�logd���Խ�����̰߳�ȫ������,������Ҫ���ͬһ��־���д�ļ�������///////
	if( flag == (int)TRUE )
	{
		/* temp not wirte in to file--wfp
		FILE	*fp;
		char filename[FILE_PATH_LEN];
		//sprintf(filename,"log/%s_%d.log",ptr->data[rp].proc_name,getpid());//���ӽ���id��׺����־�ļ�
		sprintf(filename,"log/%s.log",ptr->data[oldReadPos].proc_name);
		strcpy(filename,logfile.get_filename());
		if(logfile.get_file_len() > LOG_FILE_LEN)	fp = fopen(filename,"w+");
		else	fp = fopen(filename,"a+");
		if(fp == NULL)	WriteLog(LOG_LOG,"���ļ� %s ʧ��",filename);
		else
		{
			if(loghead)	fprintf(fp,"\n%s%s",loghead,ptr->data[oldReadPos].info);
			else	fprintf(fp,"\n%s",ptr->data[oldReadPos].info);
			fclose(fp);
		}
		*/
	}//end if(flag)
  ////////////////////////////////////////////////////////////////
	type = (int)m_ptr->data[oldReadPos].type;  //wfp
	return m_ptr->data[oldReadPos].msg;
}

bool CLogInfo::readLog(int readPos,int& type,char *data/* =NULL */,char *procNmae/* =NULL */)
{
	if( readPos < 0 || readPos >= BLOG_MAX_NUM ) return false;
	if( readPos == m_ptr->writePos )	return false;

	char loghead[BLOG_HEAD_LEN] = "";

	if(loghead)
	{
		CDateTime* pDateTime = &(m_ptr->data[readPos].dateTime);
		//��֤�������������ַ�������С��LOG_HEAD_LEN
		ACE_OS::snprintf(loghead,BLOG_HEAD_LEN,"[ %s ] , Type = %d,Process = %s\n", 
			m_ptr->data[readPos].dateTime.toString(DT_FMT_DEF).c_str(),
			(int)m_ptr->data[readPos].type,m_ptr->data[readPos].procName);
	}

	ACE_OS::snprintf(data,BLOG_LEN,"%s%s",loghead,m_ptr->data[readPos].msg);
	type = (int)m_ptr->data[readPos].type;  //wfp

	return true;
}

int CLogInfo::writePos() const
{
	return m_ptr->writePos;
}

void CLogInfo::writeLog(int type,const char *format,...)
{
	if(type<0 || type>=BLOG_TYPE_NUM)	return;
	if(m_ptr->flag[type] == FALSE)	return;
	int pos = lockWrite();
	m_ptr->data[pos].type = type;
	m_ptr->data[pos].dateTime = CDateTime::currentDateTime();

	va_list args;
	va_start(args,format);
	int num = ACE_OS::vsnprintf(m_ptr->data[pos].msg,BLOG_LEN,format,args);
	//int num = qvsnprintf(m_ptr->data[pos].msg,BLOG_LEN,format,args);
	va_end(args);
	if(num <= 0)
	{
		unLockWrite(pos);
		return;
	}
	else	m_ptr->data[pos].msg[BLOG_LEN-1] = 0;
	ACE_OS::strncpy(m_ptr->data[pos].procName,m_procName,SYS_PROCNAME_LEN);
	unLockWrite(pos);

	//����writePos
	while(m_ptr->lock) ACE_OS::sleep(0);  //sleep(0)--�ȴ���С��ʱ��
	m_ptr->lock = TRUE;	
	m_ptr->writePos = (pos + 1) % BLOG_MAX_NUM;
	m_ptr->lock = FALSE;
}

/*
 *�������ܣ�����־�ֽ���д��"*.err.log"�ļ��С��ú�������д��־�������ڴ�
 */
void CLogInfo::writeError(int type,const char *format,...)
{
	char msg[BLOG_LEN] = "";
	CDateTime curDateTime = CDateTime::currentDateTime();

	if(type<0 || type>=BLOG_TYPE_NUM)	return;
	while(m_ptr->flock == TRUE) ACE_OS::sleep(0);  //wfp
	m_ptr->flock = TRUE;

	va_list args;
	va_start(args,format);
	int num = ACE_OS::vsnprintf(msg,BLOG_LEN,format,args);
	//int num = qvsnprintf(msg,BLOG_LEN,format,args);
	va_end(args);
	if(num <= 0)
	{
		m_ptr->flock = FALSE;
		return;
	}
	else	msg[BLOG_LEN-1] = 0;  //�ý�����־

	FILE	*fp = NULL;
	char filename[FILE_PATH_LEN];
	ACE_OS::sprintf(filename,"%s/log/%s.err.log",ACE_OS::getenv(SYS_ENV_VAR),m_procName);

	char loghead[BLOG_HEAD_LEN] = "";

	//��֤�������������ַ�������С��LOG_HEAD_LEN
	ACE_OS::sprintf(loghead,"[ %s ] , Type = %d , ProcessName = %s\n",
							curDateTime.toString(DT_FMT_DEF).c_str(),
							(int)type,m_procName); 

	if( ACE_OS::filesize(filename) > BLOG_FILE_LEN )	
		fp = ACE_OS::fopen(filename,"w+");
	else	
		fp = ACE_OS::fopen(filename,"a+");

	if(fp == NULL)	ACE_OS::printf("���ļ� %s ʧ��\n",filename);
	else
	{
		if(loghead)	ACE_OS::fprintf(fp,"\n%s%s",loghead,msg);
		else	ACE_OS::fprintf(fp,"\n%s",msg);
		ACE_OS::fclose(fp);
	}
	m_ptr->flock = FALSE;	
}

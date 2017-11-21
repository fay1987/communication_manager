/*==============================================================================
* 文件名称 : filereadtask.cpp
* 模块 : 数据库存盘任务
* 创建时间 : 2009-07-22 15:55:23
* 作者 : wfp
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "filereadtask.h"
#include <ace/OS.h>
#include "sys/blog.h"
#include "scddef.h"
#include "dac/dac.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include "scdmacro.h"
#include "dbsavetask.h"

using namespace PDT;

CFileReadTask::CFileReadTask()
{
}

CFileReadTask::~CFileReadTask()
{

}

bool CFileReadTask::init()
{
	m_dirName = CString(ACE_OS::getenv(SYS_ENV_VAR)) + CString("/") + CString(SCD_HISDATA_FILEDIR);
	return true;
}

int CFileReadTask::start()
{
	logprint( LOG_SCD_FILEREAD_TASK,"<CFileReadTask>:创建一个线程!" );
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

void CFileReadTask::stop()
{
	if (thr_count() > 0) 
	{
		thr_mgr()-> cancel_task(this);
		wait();
	}
}

int CFileReadTask::svc()
{
	m_timeId = m_timer.add(1000);		//1000ms
	m_timer.start();

	ACE_Time_Value tv(0,10000);			//10ms

	while ( true )
	{
		if ( thr_mgr()->testcancel( thr_mgr()->thr_self() ) )
			break;
		ACE_OS::sleep( tv );

		if ( m_timer.time_out(m_timeId) )
		{
			if ( !TASK_DBSAVE::instance()->isEmpty() )
				continue;

			//遍历文件夹
			//readFiles();
		}

	}//end while

	return 0;
}

void CFileReadTask::readFiles()
{
	QStringList filters;
	filters << "*.data" << "*.tmp";

	QDir dir;
	dir.setPath(m_dirName.c_str());

	//按时间，最新的在前.
	QStringList fileList = dir.entryList(filters,QDir::Files,QDir::Time);

	//仅处理一个文件 
	for (int i = 0; i < fileList.size(); i++)
	{
		if ( fileList.at(i).indexOf(SCD_HISDATA_FILE_PREFIX) != -1 )
		{
			procHisDataFile(fileList.at(i).toAscii().constData());
			break;
		}
		else
		{
			logprint(LOG_SCD_FILEREAD_TASK,"找到未知类型的文件<%s>,正在删除",fileList.at(i).toAscii().constData());
			QFile::remove( fileList.at(i) );
		}
	}
}

void CFileReadTask::procHisDataFile(const char* fname)
{
	if ( fname == NULL ) return;

	FILE* fp = NULL;
	CString filePathName = m_dirName + CString("/") + CString(fname);

	//fread说明：必须以二进制方式读取，否则会导致读取字节数<总字节数；此外不能一次读太多字节
	fp = ACE_OS::fopen(filePathName.c_str(),"rb");
	if ( fp == NULL ) return;

	int nreadItem = -1;
	bool isFileEnd = false;
	char databuf[1024] = "";
	SCD_DATA_HEAD* pHead = (SCD_DATA_HEAD*)databuf;

	while ( !isFileEnd )
	{
		nreadItem = ACE_OS::fread(databuf,sizeof(SCD_DATA_HEAD),1,fp);
		if ( nreadItem != 1 )
		{//认为文件已读取结束
			logprint(LOG_SCD_FILEREAD_TASK,"read from file error or end<%s>",fname);
			isFileEnd = true;
			break;
		}

		switch ( pHead->dataclass )
		{
		case HIS_CLASS_YC:
			if  ( ACE_OS::fread(databuf+sizeof(SCD_DATA_HEAD),sizeof(SCD_DATA_YC),1,fp) != 1 )
			{//认为文件已读取结束
				isFileEnd = true;
			}
			else 
			{
				putData(databuf,sizeof(SCD_DATA_HEAD)+sizeof(SCD_DATA_YC));
			}
			break;
		case HIS_CLASS_EVENT:
			//isFileEnd = true;
			if  ( ACE_OS::fread(databuf+sizeof(SCD_DATA_HEAD),sizeof(SCD_DATA_EVENT),1,fp) != 1 )
			{//认为文件已读取结束
				isFileEnd = true;
			}
			else 
			{
				putData(databuf,sizeof(SCD_DATA_HEAD)+sizeof(SCD_DATA_EVENT));
			}
			break;
		case HIS_CLASS_DAY_STA:
		case HIS_CLASS_MONTH_STA:		
		case HIS_CLASS_SEASON_STA:		
		case HIS_CLASS_YEAR_STA:				
		case HIS_CLASS_CHARGE:				
		case HIS_CLASS_DYNAMIC:				
		case HIS_CLASS_STATE:				
			if  ( ACE_OS::fread(databuf+sizeof(SCD_DATA_HEAD),sizeof(SCD_DATA_STA),1,fp) != 1 )
			{//认为文件已读取结束
				isFileEnd = true;
			}
			else 
			{
				putData(databuf,sizeof(SCD_DATA_HEAD)+sizeof(SCD_DATA_STA));
			}
			break;
		default:
			isFileEnd = true;
			logprint(LOG_SCD_FILEREAD_TASK,"read unknow type of data");
			break;
		}//end switch
	}//end while

	ACE_OS::fclose(fp);
	QFile::remove( filePathName.c_str() );
}

bool CFileReadTask::putData(const char* databuf,int size)
{
	if ( databuf == NULL ) return false;
	if ( size <= 0 ) return false;

	ACE_Message_Block* mblk = new ACE_Message_Block(size+1);
	if ( mblk == NULL ) return false;

	if ( mblk->copy(databuf,size) == -1 )
	{
		logprint(LOG_SCD_FILEREAD_TASK,"<fileread task> copy to message block error");
		mblk->release();
		return false;
	}

	//ACE_Time_Value nonblock (0);
	//无限阻塞
	if ( TASK_DBSAVE::instance()->put(mblk) == -1 ) 
	{
		logprint(LOG_SCD_HISDATA_TASK,"<fileread task> can not put the msg");
		return false;
	}

	return true;
}


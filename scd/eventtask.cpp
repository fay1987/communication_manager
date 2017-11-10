/*==============================================================================
* 文件名称 : eventtask.cpp
* 模块 : 事项存盘任务
* 创建时间 : 2009-07-22 15:55:23
* 作者 : wfp
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "eventtask.h"
#include <ace/OS.h>
#include "sys/blog.h"
#include "scddef.h"
#include "dac/dac.h"
#include "dac/dacdef.h"
#include "event/type.h"
#include "dbsavetask.h"

using namespace PDT;

CEventTask::CEventTask()
{
}

CEventTask::~CEventTask()
{

}

bool CEventTask::init()
{
	return true;
}

int CEventTask::start()
{
	logprint( LOG_SCD_EVENT_TASK,"<CEventTask>:创建一个线程!" );
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

void CEventTask::stop()
{
	if (thr_count() > 0) 
	{
		thr_mgr()-> cancel_task(this);
		wait();
	}
}

int CEventTask::svc()
{
	m_timeId = m_timer.add(1000);		//1000ms
	m_timer.start();

	ACE_Time_Value tv(0,10000);			//10ms

	while ( true )
	{
		if ( thr_mgr()->testcancel( thr_mgr()->thr_self() ) )
			break;
		ACE_OS::sleep( tv );

		if  ( m_timer.time_out(m_timeId) )
			checkSaveEvent();

	}
	return 0;
}

void CEventTask::checkSaveEvent()
{
	nSysEvent sysEvent;
	while ( m_event.fetchOneEvent(sysEvent) )
	{
		putEvent(sysEvent);
	}//end while
}

//遗留问题：有很多项没有赋值;事项缓冲中只有厂站名，无厂站编码，无法和数据库厂站编码对应
bool CEventTask::putEvent(const nSysEvent& data)
{
	ACE_Message_Block* mblk = new ACE_Message_Block(sizeof(SCD_DATA_HEAD) + sizeof(SCD_DATA_EVENT)+1);
	if ( mblk == NULL ) return false;

	SCD_DATA_HEAD* pHead = (SCD_DATA_HEAD*)mblk->wr_ptr();
	SCD_DATA_EVENT* pData = (SCD_DATA_EVENT*)(mblk->wr_ptr() + sizeof(SCD_DATA_HEAD));
	ACE_OS::memset(pData,0,sizeof(SCD_DATA_EVENT));
	pHead->dataclass = HIS_CLASS_EVENT;
	pData->evtClass = data.m_eventSort;
	pData->evtType = data.m_eventType;
	CDateTime dateTime;
	dateTime.year(data.m_ymd/100/100);
	dateTime.month( (data.m_ymd/100) % 100 );
	dateTime.day(data.m_ymd % 100);

	dateTime.hour(data.m_hmsms/100/100/1000);
	dateTime.minute( (data.m_hmsms /100/1000) % 100 );
	dateTime.second(data.m_hmsms%(100*1000) / 1000);
	dateTime.microsec(data.m_hmsms%1000 * 1000);		//微秒

	pData->level = data.m_security;						//wfp add at 20101014
	pData->occurTime = (hUInt32)dateTime.toTimeT();
	pData->stateValue = data.m_stateValue;
	pData->stateValue1 = data.m_stateValue1;
	pData->floatValue = data.m_floatValue;
	pData->floatValue1 = data.m_floatValue1;
	ACE_OS::strncpy(pData->staCode,data.m_stationCode,DAC_NAME_LEN);
	ACE_OS::strncpy(pData->staName,data.m_stationName,DAC_NAME_LEN);
	ACE_OS::strncpy(pData->code,data.m_code,DAC_NAME_LEN);
	ACE_OS::strncpy(pData->name,data.m_name,DAC_NAME_LEN);
	ACE_OS::strncpy(pData->user,"",16);
	ACE_OS::strncpy(pData->parentCode,data.m_parentCode,DAC_NAME_LEN);
	ACE_OS::strncpy(pData->parentName,data.m_parentName,DAC_NAME_LEN);
	ACE_OS::strncpy(pData->info,data.m_charDesc,FULLDESCRIBE_LEN);

	mblk->wr_ptr(sizeof(SCD_DATA_HEAD) + sizeof(SCD_DATA_EVENT));

	//ACE_Time_Value nonblock (0);
	//无限阻塞，确保不丢事项
	if ( TASK_DBSAVE::instance()->put(mblk) == -1 ) 
	{
		logprint(LOG_SCD_HISDATA_TASK,"<his event>can not put the msg");
		mblk->release();
		return false;
	}

	return true;
}

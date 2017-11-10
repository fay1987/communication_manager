#include "serialtaskpool.h"
#include "config.h"
#include <QTime>

CSerialTaskPool::CSerialTaskPool()
{
	m_curChanNo = 0;
}

CSerialTaskPool::~CSerialTaskPool()
{
	for (hUInt32 i=0;i<DAC_CHANNEL_NUM;i++)
	{
		if ( m_serialChannel[i].pObj != NULL )
		{
			delete m_serialChannel[i].pObj;
			m_serialChannel[i].pObj = NULL;
		}
	}
}

bool CSerialTaskPool::init()
{
	memset(&m_serialChannel,0,sizeof(m_serialChannel));

	for (hUInt32 i=0;i<DAC_CHANNEL_NUM;i++)
	{
		if ( isSerialChannel(i) )
			m_serialChannel[i].pObj = new CSerialChannel(i);
	}

	return true;
}

/*
* 说明：链路增加删除/池大小变化需要调用.
*/
hInt32 CSerialTaskPool::start()
{
	//初始化 thread_count==0;
	//可适应poolSize参数值修改的情况
	hInt32 validSerialChanNum = serialChannelNumber();
	hInt32 poolSize = CONFIG::instance()->serialThreadPoolSize();
	hInt32 activeThreadNum = (int)this->thr_count();										//已活动的线程数目
	hInt32 totalThreadNum = validSerialChanNum < poolSize ? validSerialChanNum : poolSize;	//获取池中需要开辟的总线程数
	if ( totalThreadNum > activeThreadNum )
	{
		logprint(LOG_SERIAL_BASE,"<CSerialTaskPool> pool:准备增加%d个线程到线程池!",totalThreadNum - activeThreadNum);
		//return this->activate(THR_NEW_LWP | THR_DETACHED,totalThreadNum - activeThreadNum,1);
		return this->activate(THR_NEW_LWP | THR_DETACHED);
	}
	return 0;
}

void CSerialTaskPool::stop()
{
	if (thr_count() > 0) thr_mgr()->cancel_task(this);
}

hUInt32 CSerialTaskPool::serialChannelNumber()		const
{
	const DAC_SYSTEM_INFO *pSystemInfo = m_commInf.systemInfo();
	if ( pSystemInfo == NULL ) return 0;

	const DAC_CHANNEL* pChannel = NULL;
	int num = 0;
	for (hUInt32 i=0;i<pSystemInfo->channelNum;i++)
	{	
		pChannel = m_commInf.channel(i);
		if( pChannel == NULL ) continue;
	
		if ( pChannel->type == CHANNEL_TYPE_SERIAL )
			++num;
	}
	return num;
}

bool CSerialTaskPool::isSerialChannel(hInt32 channel) const
{
	const DAC_CHANNEL* pChannel = m_commInf.channel(channel);
	if ( pChannel == NULL ) return false;

	if ( pChannel->type == CHANNEL_TYPE_SERIAL ) return true;

	return false;
}

hInt32 CSerialTaskPool::lockSerialChannel()
{
	DAC_SYSTEM_INFO *pSystemInfo = m_commInf.systemInfo();

	ACE_GUARD_RETURN(ACE_Thread_Mutex, mon, m_mutex, -1);
	
	if ( m_curChanNo >= (pSystemInfo->channelNum) )
		m_curChanNo = 0;

	hUInt32 chanNo = m_curChanNo;
	for (hUInt32 i=0;i<pSystemInfo->channelNum;i++,chanNo++)
	{
		chanNo = chanNo % pSystemInfo->channelNum;

		if ( !isSerialChannel(chanNo) ) continue;
		if ( m_serialChannel[chanNo].lock ) continue;
		if ( m_serialChannel[chanNo].pObj == NULL ) continue;

		m_serialChannel[chanNo].lock = true;
		++m_curChanNo;
		return chanNo;
	}

	++m_curChanNo;
	return -1;
}

void CSerialTaskPool::unLockSerialChannel(hInt32 channel)
{
	if (channel < 0 || channel >= DAC_CHANNEL_NUM) return;

	ACE_GUARD(ACE_Thread_Mutex, mon, m_mutex);
	
	m_serialChannel[channel].lock = false;
}

//动态检测obj对象是否需要删除
void CSerialTaskPool::checkObject()
{
	DAC_SYSTEM_INFO *pSystemInfo = m_commInf.systemInfo();
	ACE_GUARD(ACE_Thread_Mutex, mon, m_mutex);

	for (hUInt32 i=0;i<pSystemInfo->channelNum;i++)
	{
		//if ( m_serialChannel[i].lock ) continue;

		if ( isSerialChannel(i) ) 
		{
			if ( m_serialChannel[i].pObj == NULL )
			{
				m_serialChannel->pObj = new CSerialChannel(i);
			}
		}
		else
		{
			if ( m_serialChannel[i].pObj != NULL )
			{
				delete m_serialChannel[i].pObj;
				m_serialChannel[i].pObj = NULL;
			}
		}
	}//end for
	
}

hInt32	CSerialTaskPool::svc()
{
	DAC_SYSTEM_INFO *pSystemInfo = m_commInf.systemInfo();
	ACE_Thread_Manager* mgr = this->thr_mgr();
	int no = 0;
	//ACE_Time_Value tv(0,10000);	//10ms
	ACE_Time_Value tv(0,100000);	//100ms
	CSerialChannel* pSerialChannel = NULL;
	hUInt32 checkObjectCount = 0;

	checkObject();	//第一次先check
	while ( 1 )		//!isExit
	{
		//QTime t;
		

		if ( mgr->testcancel(mgr->thr_self()) ) 
			break;
		
		

		ACE_OS::sleep(tv);
		if ( pSystemInfo->loadParaFlag & LOADPARA_CHANNEL ) 
			continue;
		//每秒中check一次对象
		++checkObjectCount;
		if ( checkObjectCount % 100 == 0 )	//1s
			checkObject();

		//logprint(9998,"time:%s ",CDateTime::currentDateTime().toString(DT_FMT_DEF).c_str());

		for (hUInt32 i=0;i<pSystemInfo->channelNum;i++)
		{
			//if ( m_serialChannel[i].lock ) continue;

			if ( isSerialChannel(i) ) 
			{
				pSerialChannel = m_serialChannel[i].pObj;

				if(pSerialChannel)
				{
					
					if ( !pSerialChannel->isOpen() )
						pSerialChannel->open();//t.start(); 

					if ( pSerialChannel->isOpen() )
						pSerialChannel->run();

					//logprint(9998,"Time elapsed: %d ms", t.elapsed());
				}
			}
		}


		//no = lockSerialChannel();
		////logprint(9999,"thread<%d> lock link<%d> sucess",ACE_Thread::self(),no);
		//if (no != -1) 
		//{
		//	pSerialChannel = m_serialChannel[no].pObj;

		//	if ( !pSerialChannel->isOpen() )
		//		pSerialChannel->open();

		//	if ( pSerialChannel->isOpen() )
		//		pSerialChannel->run();

		//	unLockSerialChannel(no);	
		//}
		//else 
		//	ACE_OS::sleep(ACE_Time_Value(0,10000));	//若线程数目多于链路数目，则需要等待，防止占用cpu

	}//end while
	logprint(LOG_SERIAL_BASE,"thread:%d ,serial task pool exit!",ACE_Thread::self());

	return	0;
}

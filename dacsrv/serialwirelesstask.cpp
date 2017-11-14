#include "serialwirelesstask.h"
#include "config.h"
#include <QTime>

//#define  W_Serial_Channel_Num	5
#define  Cmd_TimeOut			50
#define  Cmd_ReceiveTime		400

CSerialWTask::CSerialWTask()
{
	m_curChanNo = 0;
	//m_nReceiveNum = 0;
	//m_bTimeOut = false;
}

CSerialWTask::~CSerialWTask()
{
	//for (hUInt32 i=0;i<DAC_CHANNEL_NUM;i++)
	//{
	//	if ( m_serialChannel[i].pObj != NULL )
	//	{
	//		delete m_serialChannel[i].pObj;
	//		m_serialChannel[i].pObj = NULL;
	//	}
	//}
	QMap<int, CSerialChannelWireless*>::iterator it;
	for (it = m_mapSerialChannel.begin(); it != m_mapSerialChannel.end(); it++)
	{
		CSerialChannelWireless* pDelete = it.value();
		delete pDelete;
		pDelete = NULL;
	}
	m_mapSerialChannel.clear();
}

bool CSerialWTask::init()
{
	//memset(&m_serialChannel,0,sizeof(m_serialChannel));

	//for (hUInt32 i=0;i<DAC_CHANNEL_NUM;i++)
	//{
	//	if (isWirelessSerialChannel(i))
	//	{
	//		m_serialChannel[i].pObj = new CSerialChannelWireless(i);
	//	}
	//} 

	for (hUInt32 i=0;i<DAC_CHANNEL_NUM;i++)
	{
		if (isWirelessSerialChannel(i))
		{
			CSerialChannelWireless* pSerialChannel = new CSerialChannelWireless(i);
			m_mapSerialChannel.insert(i, pSerialChannel);

			//DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(i);
			//for (int x =0; x < DAC_ROUTE_IN_GROUP; x++)
			//{
			//	DAC_ROUTE_INFO* pInfo = m_commInf.routeInfo(x);
			//	if ( pInfo) 
			//		pInfo->manStop = true;
			//}
		}
	}
	
	return true;
}

/*
* 说明：链路增加删除/池大小变化需要调用.
*/
hInt32 CSerialWTask::start()
{
	m_timeOutID = m_tmSet.add(Cmd_TimeOut);
	m_tmSet.start(m_timeOutID);
	//m_recOutID = m_tmSet.add(Cmd_ReceiveTime);
	//m_tmSet.start(m_recOutID);

	//if (m_bflag)
	//{
	//	QMap<int, CSerialChannelWireless*>::iterator it;
	//	for (it = m_mapSerialChannel.begin(); it != m_mapSerialChannel.end(); it++)
	//	{
	//		DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(it.key());
	//		for (int x =0; x < DAC_ROUTE_IN_GROUP; x++)
	//		{
	//			DAC_ROUTE_INFO* pInfo = m_commInf.routeInfo(x);
	//			if ( pInfo) 
	//				pInfo->manStop = false;
	//		}
	//	}
	//	m_bflag = false;
	//}

	//初始化 thread_count==0;
	//可适应poolSize参数值修改的情况
	hInt32 validSerialChanNum = serialChannelNumber();
	hInt32 poolSize = CONFIG::instance()->serialThreadPoolSize();
	hInt32 activeThreadNum = (int)this->thr_count();										//已活动的线程数目
	hInt32 totalThreadNum = validSerialChanNum < poolSize ? validSerialChanNum : poolSize;	//获取池中需要开辟的总线程数
	if ( totalThreadNum > activeThreadNum )
	{
		logprint(LOG_SERIAL_BASE,"<CSerialWTask>:准备增加%d个线程到线程池!",totalThreadNum - activeThreadNum);
		//return this->activate(THR_NEW_LWP | THR_DETACHED,totalThreadNum - activeThreadNum,1);
		return this->activate(THR_NEW_LWP | THR_DETACHED);
	}

	return 0;
}

void CSerialWTask::stop()
{
	if (thr_count() > 0) thr_mgr()->cancel_task(this);
	m_tmSet.remove(m_timeOutID);
	//m_tmSet.remove(m_recOutID);

}

hUInt32 CSerialWTask::serialChannelNumber()		const
{
	//const DAC_SYSTEM_INFO *pSystemInfo = m_commInf.systemInfo();
	//if ( pSystemInfo == NULL ) return 0;

	//const DAC_CHANNEL* pChannel = NULL;
	//int num = 0;
	//for (hUInt32 i=0;i<pSystemInfo->channelNum;i++)
	//{	
	//	pChannel = m_commInf.channel(i);
	//	if( pChannel == NULL ) continue;
	//
	//	if (pChannel->type == CHANNEL_TYPE_SERIALWIRELESS)
	//		++num;
	//}
	//return num;
	return m_mapSerialChannel.size();
}


bool CSerialWTask::isWirelessSerialChannel(hInt32 channel) const
{
	const DAC_CHANNEL* pChannel = m_commInf.channel(channel);
	if ( pChannel == NULL ) return false;

	if ( pChannel->type == CHANNEL_TYPE_SERIALWIRELESS ) return true;

	return false;
	//return m_mapSerialChannel.contains(channel);
}


//hInt32 CSerialWTask::lockSerialChannel()
//{
//	DAC_SYSTEM_INFO *pSystemInfo = m_commInf.systemInfo();
//
//	ACE_GUARD_RETURN(ACE_Thread_Mutex, mon, m_mutex, -1);
//	
//	if ( m_curChanNo >= (pSystemInfo->channelNum) )
//		m_curChanNo = 0;
//
//	hUInt32 chanNo = m_curChanNo;
//	for (hUInt32 i=0;i<pSystemInfo->channelNum;i++,chanNo++)
//	{
//		chanNo = chanNo % pSystemInfo->channelNum;
//
//		if ( !isWirelessSerialChannel(chanNo) ) continue;
//		if ( m_serialChannel[chanNo].lock ) continue;
//		if ( m_serialChannel[chanNo].pObj == NULL ) continue;
//
//		m_serialChannel[chanNo].lock = true;
//		++m_curChanNo;
//		return chanNo;
//	}
//
//	++m_curChanNo;
//	return -1;
//}
//
//void CSerialWTask::unLockSerialChannel(hInt32 channel)
//{
//	if (channel < 0 || channel >= DAC_CHANNEL_NUM) return;
//
//	ACE_GUARD(ACE_Thread_Mutex, mon, m_mutex);
//	
//	m_serialChannel[channel].lock = false;
//}

//动态检测obj对象是否需要删除
void CSerialWTask::checkObject()
{
	DAC_SYSTEM_INFO *pSystemInfo = m_commInf.systemInfo();
	ACE_GUARD(ACE_Thread_Mutex, mon, m_mutex);

	//for (hUInt32 i=0;i<pSystemInfo->channelNum;i++)
	//{
	//	//if ( m_serialChannel[i].lock ) continue;
	//	if (isWirelessSerialChannel(i))
	//	{
	//		if (m_serialChannel[i].pObj == NULL)
	//		{
	//			m_serialChannel[i].pObj = new CSerialChannelWireless(i);
	//		}
	//	}
	//	else
	//	{
	//		if ( m_serialChannel[i].pObj != NULL )
	//		{
	//			delete m_serialChannel[i].pObj;
	//			m_serialChannel[i].pObj = NULL;
	//		}
	//	}
	//}//end for


	for (hUInt32 i=0;i<pSystemInfo->channelNum;i++)
	{
		if (isWirelessSerialChannel(i))
		{
			if (!m_mapSerialChannel.contains(i))
			{
				CSerialChannelWireless* pSerialChannel = new CSerialChannelWireless(i);
				m_mapSerialChannel.insert(i, pSerialChannel);
			}
		}
		else
		{
			if (m_mapSerialChannel.contains(i))
			{
				QMap<int, CSerialChannelWireless*>::iterator it = m_mapSerialChannel.find(i);
				delete it.value();
				it.value() = NULL;
				m_mapSerialChannel.remove(i);
			}
		}
	}//end for
	
}

hInt32	CSerialWTask::svc()
{
	DAC_SYSTEM_INFO *pSystemInfo = m_commInf.systemInfo();
	ACE_Thread_Manager* mgr = this->thr_mgr();

	CSerialChannelWireless* pWSerialChannel = NULL;

	int starttime = clock()/CLOCKS_PER_SEC *1000;
	int lastTime = starttime;

	QMap<int, CSerialChannelWireless*>::iterator it;
	for (it = m_mapSerialChannel.begin(); it != m_mapSerialChannel.end(); it++)
	{
		pWSerialChannel = it.value();
		if(pWSerialChannel)
		{
			if ( !pWSerialChannel->isOpen() )
				pWSerialChannel->open();
		}
	}

	checkObject();	//第一次先check
	ACE_Time_Value tv(0,100000);							//100ms
	while ( 1 )		//!isExit
	{
		if ( mgr->testcancel(mgr->thr_self()) ) 
			break;

		if ( pSystemInfo->loadParaFlag & LOADPARA_CHANNEL ) 
			continue;

		ACE_OS::sleep( tv );

		//每秒check一次对象
		lastTime = clock()/CLOCKS_PER_SEC*1000;;
		if ( lastTime-starttime >= 1000 )	//1s
		{
			checkObject();
			starttime = lastTime;
		}


		QMap<int, CSerialChannelWireless*>::iterator it;
	//	if (m_tmSet.time_out(m_recOutID))
		{
			for (it = m_mapSerialChannel.begin(); it != m_mapSerialChannel.end(); it++)
			{
				pWSerialChannel = it.value();
				if(pWSerialChannel)
				{
					if ( !pWSerialChannel->isOpen() )
						pWSerialChannel->open();

					if (pWSerialChannel->isOpen() )
					{
						pWSerialChannel->readData();
					}
				}
			}
		}



		if (m_tmSet.time_out(m_timeOutID)/*m_bTimeOut*/)
		{
			
			for (it = m_mapSerialChannel.begin(); it != m_mapSerialChannel.end(); it++)
			{

				pWSerialChannel = it.value();
				if(pWSerialChannel)
				{
					if ( !pWSerialChannel->isOpen() )
						pWSerialChannel->open();

					if (pWSerialChannel->isOpen() )
					{
						pWSerialChannel->run();
						pWSerialChannel->writeData();
					}
				}
			}

			//m_bTimeOut = false;
			m_tmSet.start(m_timeOutID);
		}
		//for (hUInt32 i=0;i<pSystemInfo->channelNum;i++)
		//{
		//	if ( isWirelessSerialChannel(i) ) 
		//	{
		//		pWSerialChannel = m_serialChannel[i].pObj;

		//		if(pWSerialChannel)
		//		{
		//			if ( !pWSerialChannel->isOpen() )
		//				pWSerialChannel->open();

		//			if (pWSerialChannel->isOpen() )
		//			{
		//				pWSerialChannel->readData();
		//			}
		//		}
		//	}
		//}

		//logprint(LOG_SERIAL_BASE,"m_timeSet.time_out(%d): %d!",m_timeOutID, m_tmSet.time_out(m_timeOutID));

		//if (m_tmSet.time_out(m_timeOutID))
		//{
		//	logprint(LOG_SERIAL_BASE,"m_timeSet.time_out(m_timeOutID)m_timeSet.time_out(m_timeOutID): %d!", m_tmSet.time_out(m_timeOutID));

		//	for (hUInt32 i=0;i<pSystemInfo->channelNum;i++)
		//	{
		//		if ( isWirelessSerialChannel(i) ) 
		//		{
		//			pWSerialChannel = m_serialChannel[i].pObj;

		//			if(pWSerialChannel)
		//			{
		//				if ( !pWSerialChannel->isOpen() )
		//					pWSerialChannel->open();

		//				if (pWSerialChannel->isOpen())
		//				{
		//					pWSerialChannel->run();
		//					pWSerialChannel->writeData();
		//				}
		//			}
		//		}
		//	}
		//	m_tmSet.start(m_timeOutID);
		//}

	}//end while
	logprint(LOG_SERIAL_BASE,"thread:%d ,serial task pool exit!",ACE_Thread::self());

	return	0;
}

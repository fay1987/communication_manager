#include "linktaskpool.h"
#include "config.h"
#include <QtCore/QCoreApplication>	//wfp add at 20110125

CLinkTaskPool::CLinkTaskPool()
{
	memset(&m_link,0,sizeof(m_link));

	for (hUInt32 i=0;i<DAC_CHANNEL_NUM;i++)
	{
		m_link[i].pObj = new CLink(i);
	}

	m_curChanNo = 0;
	m_bTimeOut = false;

}

CLinkTaskPool::~CLinkTaskPool()
{
	for (hUInt32 i=0;i<DAC_CHANNEL_NUM;i++)
	{
		if ( m_link[i].pObj != NULL )
		{
			delete m_link[i].pObj;
			m_link[i].pObj = NULL;
		}
	}
}

/*
* ˵������·����ɾ��/�ش�С�仯��Ҫ����.
*/
hInt32 CLinkTaskPool::start()
{
	//��ʼ�� thread_count==0;
	//����ӦpoolSize����ֵ�޸ĵ����
	hInt32 validChanNum = channelNumber();
	hInt32 poolSize = CONFIG::instance()->linkThreadPoolSize();
	hInt32 activeThreadNum = (int)this->thr_count();							//�ѻ���߳���Ŀ
	hInt32 totalThreadNum = validChanNum < poolSize ? validChanNum : poolSize;	//��ȡ������Ҫ���ٵ����߳���
	if ( totalThreadNum > activeThreadNum )
	{
		logprint(LOG_DACSRV_LINKPOOL,"<CLinkTaskPool> pool:׼������%d���̵߳��̳߳�!",totalThreadNum - activeThreadNum);
		return this->activate(THR_NEW_LWP | THR_DETACHED,totalThreadNum - activeThreadNum,1);
	}
	return 0;
}

void CLinkTaskPool::stop()
{
	if (thr_count() > 0) thr_mgr()->cancel_task(this);
}

hUInt32 CLinkTaskPool::channelNumber()		const
{
	const DAC_SYSTEM_INFO *pSystemInfo = m_commInf.systemInfo();
	if ( pSystemInfo == NULL ) return 0;

	int num = 0;
	for (hUInt32 i=0;i<pSystemInfo->channelNum;i++)
	{	
		if ( m_commInf.isChannelValid(i) )
			++num;
	}
	return num;
}

/*
 * ����˵�������ÿ��lockһ��link������GUARD����̫��Ӷ�����cpu���ߵ�����
 * �������룺linknoVec-link no vector;num-���lock������link��Ŀ
 */
hInt32 CLinkTaskPool::lockMultiLinks(LINKNO_VECTOR& linknoVec,hInt32 num)
{
	hInt32 linkNo = -1;
	linknoVec.clear();
	ACE_GUARD_RETURN(ACE_Thread_Mutex, mon, m_mutex, -1);

	for (int i=0;i<num;i++)
	{
		linkNo = lockLink();
		if ( linkNo != -1 ) linknoVec.push_back(linkNo);
		else break;											//wfp add at 20101110
	}
	if ( linknoVec.size() <= 0 ) return -1;
	else return 0;
}

void CLinkTaskPool::unLockMultiLinks(const LINKNO_VECTOR& linknoVec)
{
	ACE_GUARD(ACE_Thread_Mutex, mon, m_mutex);
	for (hUInt32 i=0;i<linknoVec.size();i++)
	{
		unLockLink(linknoVec[i]);
	}
}

hInt32 CLinkTaskPool::lockLink()
{
	DAC_SYSTEM_INFO *pSystemInfo = m_commInf.systemInfo();

	//ACE_GUARD_RETURN(ACE_Thread_Mutex, mon, m_mutex, -1);
	
	if ( m_curChanNo >= (pSystemInfo->channelNum) )
		m_curChanNo = 0;

	hUInt32 chanNo = m_curChanNo;
	for (hUInt32 i=0;i<pSystemInfo->channelNum;i++,chanNo++)
	{
		chanNo = chanNo % pSystemInfo->channelNum;

		if ( m_link[chanNo].lock ) continue;
		if ( m_link[chanNo].pObj == NULL || !m_link[chanNo].pObj->isValid()) continue;

		m_link[chanNo].lock = true;
		++m_curChanNo;
		return chanNo;
	}

	++m_curChanNo;
	return -1;
}

void CLinkTaskPool::unLockLink(hInt32 linkNo)
{
	if (linkNo < 0 || linkNo >= DAC_CHANNEL_NUM) return;

	//ACE_GUARD(ACE_Thread_Mutex, mon, m_mutex);
	
	m_link[linkNo].lock = false;
}

hInt32	CLinkTaskPool::svc()
{
	DAC_SYSTEM_INFO *pSystemInfo = m_commInf.systemInfo();
	ACE_Thread_Manager* mgr = this->thr_mgr();
	int no = 0;
	ACE_Time_Value tv(0,100000);	//100ms 
	CLink* pLink = NULL;
	LINKNO_VECTOR	linkNoVec;
	int poolSize = CONFIG::instance()->linkThreadPoolSize();
	int lockNum = 1;
	int heartID = createDeadHeart();			//wfp add at 20101129

	while ( 1 )  //!isExit
	{
		if ( mgr->testcancel(mgr->thr_self()) ) 
			break;

		ACE_OS::sleep(tv);
		if ( pSystemInfo->loadParaFlag & LOADPARA_CHANNEL ) 
			continue;

		m_deadHeartSet.clearCount(heartID);		//wfp add at 20101129

		lockNum = pSystemInfo->channelNum / poolSize;
		if ( lockNum < 1 ) lockNum = 1;

		if ( lockMultiLinks(linkNoVec,lockNum) != -1 )
		{
			addHeartLink(heartID,linkNoVec);	//wfp add at 20110714

			if (m_bTimeOut)
			{
				for (hUInt32 i=0;i<linkNoVec.size();i++)
				{
					no = linkNoVec[i];
					pLink = m_link[no].pObj;
					pLink->setTimeOutFlag(m_bTimeOut);
				}
				m_bTimeOut = false;
			}
			for (hUInt32 i=0;i<linkNoVec.size();i++)
			{			
				try
				{
					no = linkNoVec[i];
					pLink = m_link[no].pObj;
					if ( !pLink->isOpen() )
						pLink->open();

					if ( pLink->isOpen() )
					{
						pLink->run();
					//	logprint(LOG_DACSRV_LINK,"<Link %d>��Լ�쳣��·����Ч���ӵ�ǰ·��<%d>�л�����һ��·��<%d>",
						CDateTime tNow = CDateTime::currentDateTime();
						logprint(LOG_DACSRV_LINKPOOL, "���գ� -- %02d:%02d:%02d  %03d, pLink<%d>->run()",
							tNow.hour(), tNow.minute(), tNow.second(), tNow.microsec()/1000, 
							no);

					}
				}
				catch (...)
				{
					errprint(LOG_DACSRV_LINKPOOL,"link<%d> protocol catch error",no);	//wfp add at 20110125
					qApp->quit();
				}
			}
			unLockMultiLinks(linkNoVec);
		}
		else
		{
			ACE_OS::sleep(ACE_Time_Value(1,0));	//���߳���Ŀ������·��Ŀ������Ҫ�ȴ�����ֹռ��cpu
		}

	}//end while
	logprint(LOG_DACSRV_LINKPOOL,"thread:%d , link task pool exit!",ACE_Thread::self());

	m_deadHeartSet.deleteHeart(heartID);
	return	0;
}

int CLinkTaskPool::createDeadHeart()
{
	ACE_GUARD_RETURN(ACE_Thread_Mutex, mon, m_mutex,-1);
	return m_deadHeartSet.createHeart();
}

bool CLinkTaskPool::checkDeadHeart()
{
	if ( m_deadHeartSet.isDead() ) return true;
	m_deadHeartSet.addAll();
	return false;
}

void CLinkTaskPool::addHeartLink(int heartid,LINKNO_VECTOR& linknoVec)
{
	m_deadHeartSet.removeLink(heartid);
	for (hUInt32 i=0;i<linknoVec.size();i++)
	{
		m_deadHeartSet.addLink(heartid,linknoVec[i]);
	}
}

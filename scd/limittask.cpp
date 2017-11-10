/*==============================================================================
* �ļ����� : dbsavetask.cpp
* ģ�� : ���ݿ��������
* ����ʱ�� : 2009-07-22 15:55:23
* ���� : wfp
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#include "limittask.h"
#include <ace/OS.h>
#include <ace/CDR_Stream.h>
#include "sys/blog.h"
#include "scddef.h"
#include "dac/dac.h"
#include "dac/dacdef.h"
#include "event/type.h"

using namespace PDT;

CLimitTask::CLimitTask()
{
	m_isLimitTypeChange = false;
}

CLimitTask::~CLimitTask()
{

}

bool CLimitTask::init()
{
	return true;
}

int CLimitTask::start()
{
	logprint( LOG_SCD_LIMIT_TASK,"<CLimitTask>:����һ���߳�!" );
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

void CLimitTask::stop()
{
	if (thr_count() > 0) 
	{
		thr_mgr()-> cancel_task(this);
		wait();
	}
}

int CLimitTask::svc()
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
			ycCheck();
			yxEvent();
		}


	}//end while
	return 0;
}

void CLimitTask::ycCheck()
{
	m_curCycleTime = (hUInt32)ACE_OS::time(0);

	DAC_ROUTE* pRoute = NULL;
	DAC_YC_CHG_DATA chgData;
	for (hUInt32 route=0;route<m_commInf.systemInfo()->routeNum;route++)
	{
		pRoute = m_commInf.route(route);
		if ( pRoute == NULL ) continue;

		while ( m_dataInf.chgYc(route,chgData) )
		{
			m_isLimitTypeChange = false;
			overLimitCheck(pRoute->group,chgData);
			overLimitEventProc(pRoute->group,chgData);
		}
	}//end for
}

/*************************************************************
void  CLimitTask::overLimitCheck(void)
�������ܣ�Խ�޼��
������	  ��
����    ����������ֵ����Խ�޼��
���أ�	  ��
˵����	  ֻ���б仯�����ݽ���Խ�޴������������ޱ仯��Խ�޼���Ҳ���䣩
**************************************************************/
void  CLimitTask::overLimitCheck(hInt32 group,const DAC_YC_CHG_DATA& chgData)
{
	DAC_YC* pPara = m_dataInf.ycPara(group,chgData.no);
	if ( pPara == NULL || !pPara->valid ) return;
	if ( !pPara->isEvent ) return;

	//��ֵ
	/*wfp masked at 20101008(ȡ���������ж�ʱ����ֵ����)
	if( PDT_ISZERO(chgData.data.val) ) 
	{
		if( pPara->limitType != YC_OVLIMIT_NORMAL ) 
		{
			m_isLimitTypeChange = true;
			pPara->limitType = YC_OVLIMIT_NORMAL;
			pPara->eventStartTime = m_curCycleTime;
		}
		return;	//��ֵ������Ϊ��·ͣ��,������Խ��
	}
	*/

	//********���˵��������ε�Խ�޼�����ͬ�����********START
	if( pPara->limitType == YC_OVLIMIT_NORMAL ) //�ϴε�Խ�޼���Ϊ����������
	{
		if( chgData.data.val >= pPara->lowLimit && chgData.data.val <= pPara->highLimit ) return;
		//(NORMAL-->LOW��HIGH��LOWLOW��HIGHHIGH)�봦��
	}
	else if( pPara->limitType == YC_OVLIMIT_HIGH )  //�ϴε�Խ�޼���Ϊ��Խ���ޡ���
	{
		//��������ʱ��HIGH-->NORMAL��HIGHHIGH��LOW��LOWLOW�����봦��
		//һ������ʱ��HIGH-->NORMAL,LOW�봦��
		if ( chgData.data.val > pPara->highLimit 
			&& chgData.data.val <= pPara->hihiLimit ) return;
	}
	else if( pPara->limitType == YC_OVLIMIT_LOW ) //�ϴε�Խ�޼���Ϊ��Խ���ޡ���
	{
		//��������ʱ��LOW-->NORMAL,HIGH,HIGHHIGH,LOWLOW�봦��
		//һ������ʱ��LOW-->NORMAL,HIGH�봦��
		if ( chgData.data.val < pPara->lowLimit
			&& chgData.data.val >= pPara->lolowLimit ) return;
	}
	else if( pPara->limitType == YC_OVLIMIT_HIHI )
	{
		//HIGHHIGH-->HIGH,NORMAL,LOW,LOWLOW�봦��
		if ( chgData.data.val > pPara->hihiLimit ) return;
	}
	else if( pPara->limitType==YC_OVLIMIT_LOLO ) 
	{
		//LOWLOW-->LOW,NORMAL,HIGH,HIGHHIGH�봦��
		if ( chgData.data.val < pPara->lolowLimit ) return;
	}
	//********���˵��������ε�Խ�޼�����ͬ�����********ENDS

	//Խ�޼������˱仯������Խ�޻ָ��������������Խ�ޱ�����־��
	m_isLimitTypeChange = true;

	//���±仯��Խ�޼���-------START
	//��ǰ��HIGH��LOW����Խ�ޱ�־��Խ�޼��𣻷�����Խ�޻ָ���
	if( chgData.data.val > pPara->highLimit )
		pPara->limitType = YC_OVLIMIT_HIGH;
	else if( chgData.data.val < pPara->lowLimit )
		pPara->limitType = YC_OVLIMIT_LOW;
	else   
	{
		pPara->limitType = YC_OVLIMIT_NORMAL;
	}
	pPara->eventStartTime = m_curCycleTime;

	//Ҫ��������Խ��ʱ
	if ( chgData.data.val > pPara->hihiLimit ) 
		pPara->limitType = YC_OVLIMIT_HIHI;
	else if ( chgData.data.val < pPara->lolowLimit )
		pPara->limitType = YC_OVLIMIT_LOLO;

	//���±仯��Խ�޼���-------END
	return ;
}

//����Խ�������Խ�ޣ�������ʵ���Խ�������֮��Ȼ��
void  CLimitTask::overLimitEventProc(hInt32 group,const DAC_YC_CHG_DATA& chgData)
{
	DAC_YC* pPara = m_dataInf.ycPara(group,chgData.no);
	if ( pPara == NULL || !pPara->valid ) return;

	if( m_isLimitTypeChange ) //Խ�޼������仯����Խ�޻ָ���
	{
		if( pPara->limitType >= YC_OVLIMIT_HIHI )	//����--->HIHI,LOLO���������������
		{
			pPara->hasPutEvent = true;				//����Խ������������Ѳ�����־������
			createYcEvent(group,pPara->limitType,chgData);
			return;
		}

		if( pPara->limitType > YC_OVLIMIT_NORMAL ) 
			//(1)NORMAL->HI,LO(2)LO->HI, HI->LO, HIHI->HI,LO, LOLO->HI,LO
		{
			//Խ�޼����ΪHI����LO����û����ʱҪ��ֱ������������ñ�־�����ء�
			//Խ�޼����ΪHI����LO�����ϴε����������ɣ������򼶱����˱仯���������±�����
			//����Խ�޺��ѱ������������Խ�޼���仯�����ָ�������Ӧ������ʱ���ٱ�����
			if( pPara->hasPutEvent || pPara->eventDelay == 0 )
			{
				pPara->hasPutEvent = true;
				createYcEvent(group,pPara->limitType,chgData);
				return;
			}
		}
		else if( pPara->limitType == YC_OVLIMIT_NORMAL ) 
		{
			//����Խ�޻ָ�ʱ�����ϴ�Խ�����������ɣ��������ɻָ����Ȼ�������������ɱ�־�����ء�
			//���ϴ�Խ������ʱ��δ����������λָ�������������������������ɱ�־��ֱ�ӷ��ء�
			if( pPara->hasPutEvent || pPara->eventDelay == 0 )	//wfp changed at 20110801
			{
				createYcEvent(group,pPara->limitType,chgData);
			}
			pPara->hasPutEvent = true;
			return;
		}
	}//end if m_isLimitTypeChange

	//ʣ������Ϊ��
	//1����������Խͬһ�����ޣ�����ʱ�ϴ�������, ����ʱ�ϴ�������,����ʱ�ϴ�������,����ʱ�ϴ������	
	//2�������ξ�����
	//3������-->Խ(����)��,����ʱҪ��
	//4  Խ��(����ʱδ��������)->Խ��ͬ����(����)��
	//���2���ų�, 
	if( pPara->limitType > YC_OVLIMIT_NORMAL && pPara->eventDelay > 0 ) 
	{
		if( !pPara->hasPutEvent ) //�ϴ�������δ����: 1���ޣ�3��4
			//��3���ϴ�������1��4����Խ�޵���ʱδ����������Խ�ޣ����жϡ�
			//���ϴ�����������1��4���ϴ�ΪԽ�ޣ���������ΪԽ�ޣ�������ʱҪ���������������ֱ��
			//Խ�޻ָ����ű�����
		{
			if ( m_curCycleTime - pPara->eventStartTime > pPara->eventDelay )	//�ѳ�����ʱʱ��
			{
				pPara->hasPutEvent = true;
				createYcEvent(group,pPara->limitType,chgData);
				pPara->eventStartTime = m_curCycleTime;
			}
		}
	}//end if
}

bool CLimitTask::createYcEvent(hInt32 group,hInt32 limitType,const DAC_YC_CHG_DATA& chgData)
{
	DAC_YC* pPara = m_dataInf.ycPara(group,chgData.no);
	if ( pPara == NULL || !pPara->valid ) return false;

	DAC_GROUP* pGroup = m_commInf.group(group);
	if ( pGroup == NULL ) return false;

	DAC_STATION* pStation = m_commInf.station(pGroup->station);

	nSysEvent	sysEvent;
	ACE_OS::memset(&sysEvent,0,sizeof(nSysEvent));
	sysEvent.m_eventSort = EVENT_SORT_YC;
	sysEvent.m_eventType = EVENT_YC;
	if ( pStation != NULL)
	{
		ACE_OS::strsncpy(sysEvent.m_stationCode,pStation->code,SHORTNAME_LEN);
		ACE_OS::strsncpy(sysEvent.m_stationName,pStation->name,SHORTNAME_LEN);
	}
	ACE_OS::strsncpy(sysEvent.m_code,pPara->code,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_name,pPara->name,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_parentCode,pGroup->code,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_parentName,pGroup->name,SHORTNAME_LEN);

	CDateTime dateTime = CDateTime::currentDateTime();
	sysEvent.m_ymd = dateTime.year()*100*100 + dateTime.month()*100 + dateTime.day();
	sysEvent.m_hmsms= dateTime.hour()*100*100*1000 + dateTime.minute()*100*1000 + dateTime.second()*1000 + dateTime.microsec()/1000;

	sysEvent.m_security = pPara->eventLevel;
	sysEvent.m_isRevert	= false;
	sysEvent.PrintOut = 1;
	sysEvent.SmsOut = 0;
	sysEvent.m_floatValue = chgData.data.val;
	sysEvent.m_floatValue1 = chgData.lastVal;

	//wfp masked at 20110616-ȥ����վ������������
	if ( limitType == YC_OVLIMIT_NORMAL )
	{//Խ�޻ָ�
		sysEvent.m_isRevert = true;
		ACE_OS::sprintf(sysEvent.m_charDesc,"%s Խ�޻ָ�: �� %.3f %s �ָ��� %.3f %s",pPara->name,
											(hFloat)chgData.lastVal,pPara->unit,(hFloat)chgData.data.val,pPara->unit);
	}
	else if ( limitType == YC_OVLIMIT_HIGH )
	{//����
		sysEvent.SmsOut = pPara->isMsgAlarm;
		ACE_OS::sprintf(sysEvent.m_charDesc,"%s Խ����: �� %.3f %s �� %.3f %s",pPara->name,
											(hFloat)chgData.lastVal,pPara->unit,(hFloat)chgData.data.val,pPara->unit);
	}
	else if ( limitType == YC_OVLIMIT_LOW )
	{
		sysEvent.SmsOut = pPara->isMsgAlarm;
		ACE_OS::sprintf(sysEvent.m_charDesc,"%s Խ����: �� %.3f %s �� %.3f %s",pPara->name,
											(hFloat)chgData.lastVal,pPara->unit,(hFloat)chgData.data.val,pPara->unit);
	}
	else if ( limitType == YC_OVLIMIT_HIHI )
	{
		sysEvent.SmsOut = pPara->isMsgAlarm;
		sysEvent.m_security += 1;
		sysEvent.m_eventType = EVENT_YC + 1;	//wfp add at 20110916
		ACE_OS::sprintf(sysEvent.m_charDesc,"%s Խ������: �� %.3f %s �� %.3f %s",pPara->name,
											(hFloat)chgData.lastVal,pPara->unit,(hFloat)chgData.data.val,pPara->unit);
	}
	else if ( limitType == YC_OVLIMIT_LOLO )
	{
		sysEvent.SmsOut = pPara->isMsgAlarm;
		sysEvent.m_security += 1;
		sysEvent.m_eventType = EVENT_YC + 1;	//wfp add at 20110916
		ACE_OS::sprintf(sysEvent.m_charDesc,"%s Խ������: �� %.3f %s �� %.3f %s",pPara->name,
											(hFloat)chgData.lastVal,pPara->unit,(hFloat)chgData.data.val,pPara->unit);
	}

	return (bool)m_event.insertEvent(sysEvent);
}

void CLimitTask::yxEvent()
{
	DAC_ROUTE* pRoute = NULL;
	DAC_YX_CHG_DATA chgData;
	DAC_YX* pPara = NULL;

	for (hUInt32 route=0;route<m_commInf.systemInfo()->routeNum;route++)
	{
		pRoute = m_commInf.route(route);
		if ( pRoute == NULL ) continue;

		while ( m_dataInf.chgYx(route,chgData) )
		{ 
			pPara = m_dataInf.yxPara(pRoute->group,chgData.no);
			if ( pPara == NULL || !pPara->valid ) continue;
			if ( !pPara->isEvent ) continue;

			/*wfp masked at 20110622
			//��һ���յ�������״̬��ͬ��ֵ����澯
			if ( PDT_BIT_ENABLED((hUInt32)chgData.data.quality,QUALITY_BIT_INIT) )
			{
				if ( chgData.data.val != pPara->normal )				//������״̬��ͬ
					createYxEvent(pRoute->group,chgData);
				continue;
			}
			*/

			if ( PDT_BIT_ENABLED((hUInt32)chgData.data.quality,QUALITY_BIT_INIT) )
			{
				if ( chgData.data.val != pPara->normal )				//������״̬��ͬ
					createYxEvent(pRoute->group,chgData);
				else
				{
					//Ŀǰϵͳ��ʼֵΪ0�����������ֵ����Ϊ��0�����һ�ε������Ƚ϶�
					if ( chgData.lastVal != pPara->normal )			//��������,�ϴ��쳣
						createYxEvent(pRoute->group,chgData);
				}
				continue;
			}
			
			//�ǵ�һ��
			if ( chgData.data.val == chgData.lastVal ) continue;	//ֵû�б仯������������
			createYxEvent(pRoute->group,chgData);
		}//end while

	}//end for
}

//ң������
bool CLimitTask::createYxEvent(hInt32 group,const DAC_YX_CHG_DATA& chgData)
{
	DAC_YX* pPara = m_dataInf.yxPara(group,chgData.no);
	if ( pPara == NULL || !pPara->valid ) return false;

	DAC_GROUP* pGroup = m_commInf.group(group);
	if ( pGroup == NULL ) return false;

	DAC_STATION* pStation = m_commInf.station(pGroup->station);

	nSysEvent	sysEvent;
	ACE_OS::memset(&sysEvent,0,sizeof(nSysEvent));
	sysEvent.m_eventSort = EVENT_SORT_YX;
	sysEvent.m_eventType = pPara->eventType;
	if ( pStation != NULL)
	{
		ACE_OS::strsncpy(sysEvent.m_stationCode,pStation->code,SHORTNAME_LEN);
		ACE_OS::strsncpy(sysEvent.m_stationName,pStation->name,SHORTNAME_LEN);
	}
		
	ACE_OS::strsncpy(sysEvent.m_code,pPara->code,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_name,pPara->name,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_parentCode,pGroup->code,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_parentName,pGroup->name,SHORTNAME_LEN);

	CDateTime dateTime = CDateTime::currentDateTime();
	sysEvent.m_ymd = dateTime.year()*100*100 + dateTime.month()*100 + dateTime.day();
	sysEvent.m_hmsms= dateTime.hour()*100*100*1000 + dateTime.minute()*100*1000 + dateTime.second()*1000 + dateTime.microsec()/1000;

	sysEvent.m_security = pPara->eventLevel;
	
	//�ж��Ƿ��ǻָ�
	if ( chgData.data.val == pPara->normal )		//������״̬��ͬ
	{
		sysEvent.m_isRevert = true;
		sysEvent.SmsOut = 0;
	}
	else 
	{
		sysEvent.m_isRevert	= false;
		sysEvent.SmsOut = pPara->isMsgAlarm;		
	}

	sysEvent.PrintOut = 1;
	sysEvent.m_stateValue = chgData.data.val;
	sysEvent.m_stateValue1 = chgData.lastVal;

	return (bool)m_event.insertEvent(sysEvent);

}


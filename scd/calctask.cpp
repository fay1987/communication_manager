#include "loadtask.h"
#include "dac/dacdef.h"
#include <ace/OS.h>
#include "rdbop/rdbop.h"
#include "scddef.h"
#include "sys/blog.h"
#include "calctask.h"
#include "scd/exp_analyze.h"
#include "scdmacro.h"
#include "config.h"


using namespace PDT;

CCalcTask::CCalcTask()
{
	m_isInited = false;
}

CCalcTask::~CCalcTask()
{
}

bool CCalcTask::init()
{
	return true;
}

int CCalcTask::start()
{
	logprint(LOG_SCD_BASE,"thread:%d , calcTask����!",ACE_Thread::self());
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

void CCalcTask::stop()
{

	if (thr_count() > 0) thr_mgr()->cancel_task(this);
}

hInt32 CCalcTask::svc()
{
	ACE_Thread_Manager *mgr = this->thr_mgr ();
	ACE_Time_Value tv(0,10000);
	m_timeId = m_timer.add(1000);	//1000ms
	m_timer.start();
	DAC_SYSTEM_INFO* pSystemInfo = m_commInf.systemInfo();
	CDateTime lastKwhCalcTime = CDateTime::currentDateTime();
	int		kwhcalctype = 0;

	//initYcCalcData();
	//initYxCalcData();

	while ( 1 )
	{
		if (mgr->testcancel (mgr->thr_self ()))
			break;

		ACE_OS::sleep(tv);

		//wfp add at 20110602 for �ɼ�ϵͳδ�ɹ���ʼ��ʱ�������������¸���������ʧ��.
		if ( !m_commInf.systemInfo()->initFlag ) continue;
		
		if ( !m_isInited )
		{
			m_isInited = true;
			initYcCalcData();
			initYxCalcData();
			initKwhCalcData();

			//add by wlq for zju alg
			//m_cplb.init();
			//add end 20141106
			continue;
		}

		//�ɶ�̬���¼�������ϵ�����÷�ʽ�������ף����ܲ��񲻵�loadParaFlag�仯
		if (  PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_GROUP)
			||PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_YC)
			||PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_YX))
		{
			initYcCalcData();
			initYxCalcData();
			initKwhCalcData();
			ACE_OS::sleep(1);
			continue;
		}

		if ( m_timer.time_out(m_timeId) ) 
		{
			//�ж�ʱ���Ƿ����Ŀ�ʼ���������Ҫ���µ�����ʼֵ
			CDateTime curDateTime = CDateTime::currentDateTime();
			kwhcalctype = 0;

			if ( curDateTime.day() != lastKwhCalcTime.day() )
			{
				if( curDateTime.month() != lastKwhCalcTime.month() )
				{
					if( curDateTime.year() != lastKwhCalcTime.year() )
						kwhcalctype = 3;
					else
						kwhcalctype = 2;
				}
				kwhcalctype = 1;
				lastKwhCalcTime = curDateTime;
			}

			//�жϽ���
			calcYc();
			calcYx();
			calcKwh(kwhcalctype);
			/*if(m_cplb.mainCompute() >= 0)
				m_cplb.out();*/
			//fcalc();
		}

	}//end while

	
	logprint(LOG_SCD_BASE,"threadId = %d , scd loadtask�߳��˳�!",ACE_Thread::self());

	return 0;
}

bool CCalcTask::initYcCalcData()
{
	m_ycCalcVector.clear();

	DAC_ROUTE* pRoute = NULL;
	DAC_GROUP* pGroup = NULL;
	CALC_DATA	calcInfo;
	ACE_OS::memset(&calcInfo,0,sizeof(calcInfo));

	for (hUInt32 routeNo=0;routeNo<m_commInf.systemInfo()->routeNum;routeNo++)
	{
		pRoute = m_commInf.route(routeNo);
		if ( pRoute == NULL ) continue;
		pGroup = m_commInf.group(pRoute->group);
		if ( pGroup == NULL ) continue;

		calcInfo.routeNo = routeNo; 

		DAC_YC* pPara = NULL;
		for (hUInt32 pointNo=0;pointNo<pGroup->ycNum;pointNo++)
		{
			pPara = m_dataInf.ycPara(pRoute->group,pointNo);
			if ( pPara == NULL || !pPara->valid ) continue;

			//wfp changed at 20110909--���ڿɿ��ǲ���vector,ֱ�����ڴ����б���
			if ( pPara->calc && !CString(pPara->formula).is_empty() )
			{
				calcInfo.pointNo = pointNo;
				m_ycCalcVector.push_back(calcInfo);
			}

		}//end for
	}//end for

	return true;
}

bool CCalcTask::initYxCalcData()
{
	m_yxCalcVector.clear();

	DAC_ROUTE* pRoute = NULL;
	DAC_GROUP* pGroup = NULL;
	CALC_DATA	calcInfo;
	ACE_OS::memset(&calcInfo,0,sizeof(calcInfo));

	for (hUInt32 routeNo=0;routeNo<m_commInf.systemInfo()->routeNum;routeNo++)
	{
		pRoute = m_commInf.route(routeNo);
		if ( pRoute == NULL ) continue;
		pGroup = m_commInf.group(pRoute->group);
		if ( pGroup == NULL ) continue;

		calcInfo.routeNo = routeNo; 

		DAC_YX* pPara = NULL;
		for (hUInt32 pointNo=0;pointNo<pGroup->yxNum;pointNo++)
		{
			pPara = m_dataInf.yxPara(pRoute->group,pointNo);
			if ( pPara == NULL || !pPara->valid ) continue;

			//wfp changed at 20110909--���ڿɿ��ǲ���vector,ֱ�����ڴ����б���
			if ( pPara->calc && !CString(pPara->formula).is_empty() )
			{
				calcInfo.pointNo = pointNo;
				m_yxCalcVector.push_back(calcInfo);
			}

		}//end for
	}//end for

	return true;
}

bool CCalcTask::initKwhCalcData()
{
	m_kwhCalcVector.clear();

	DAC_ROUTE* pRoute = NULL;
	DAC_GROUP* pGroup = NULL;
	KWH_STATICS_DATA	kwhcalcInfo;
	SCD_BEGINKWH_DATA	beginData;
	ACE_OS::memset(&kwhcalcInfo,0,sizeof(kwhcalcInfo));

	for (hUInt32 routeNo=0;routeNo<m_commInf.systemInfo()->routeNum;routeNo++)
	{
		pRoute = m_commInf.route(routeNo);
		if ( pRoute == NULL ) continue;
		pGroup = m_commInf.group(pRoute->group);
		if ( pGroup == NULL ) continue;

		kwhcalcInfo.routeNo = routeNo; 

		DAC_YC* pPara = NULL;
		for (hUInt32 pointNo=0;pointNo<pGroup->ycNum;pointNo++)
		{
			pPara = m_dataInf.ycPara(pRoute->group,pointNo);
			if ( pPara == NULL || !pPara->valid ) continue;

			//wfp changed at 20110909--���ڿɿ��ǲ���vector,ֱ�����ڴ����б���
			if ( pPara->calc && !CString(pPara->formula).is_empty() )
			{
				if( CString(pPara->formula) == CString("rdl") )
					kwhcalcInfo.type = 1;
				else if ( CString(pPara->formula) == CString("ydl") )
					kwhcalcInfo.type = 2;
				else if ( CString(pPara->formula) == CString("ndl") )
					kwhcalcInfo.type = 3;
				else 
					kwhcalcInfo.type = 0;
				if( kwhcalcInfo.type != 0 )
				{
					kwhcalcInfo.pointNo = pointNo;
					kwhcalcInfo.dayinitval = 0.0;
					kwhcalcInfo.moninitval = 0.0;
					kwhcalcInfo.yearinitval = 0.0;
					kwhcalcInfo.init = 0;
					if ( TASK_LOAD::instance()->beginkwh(CString(CString(pGroup->code) + CString("/") + CString(pPara->code)).c_str(),beginData) )
					{
						kwhcalcInfo.dayinitval = beginData.dayvalue;
						kwhcalcInfo.moninitval = beginData.monthvalue;
						kwhcalcInfo.yearinitval = beginData.yearvalue;
						kwhcalcInfo.init = 1;
					}
					//����ʷ���ж�ȡ
					m_kwhCalcVector.push_back(kwhcalcInfo);
					logprint(LOG_SCD_CALC_TASK+1,"<��ʽ����>��ȳ�ʼ���ɹ�:�������=%s,�ճ�ʼֵ=%f,�³�ʼֵ=%f ,���ʼֵ=%f,type=%d",
											CString(CString(pGroup->code) + CString("/") + CString(pPara->code)).c_str(),kwhcalcInfo.dayinitval,kwhcalcInfo.moninitval,
											kwhcalcInfo.yearinitval,kwhcalcInfo.type);
				}
			}

		}//end for
	}//end for

	return true;
}

bool CCalcTask::calcYc()
{
	DAC_ROUTE* pRoute = NULL;
	DAC_GROUP* pGroup = NULL;
	DAC_YC* pPara = NULL;
	DAC_YC_DATA data;

	for (hUInt32 i=0;i<m_ycCalcVector.size();i++)
	{
		pRoute = m_commInf.route(m_ycCalcVector[i].routeNo);
		if ( pRoute == NULL ) continue;
		pGroup = m_commInf.group(pRoute->group);
		if ( pGroup == NULL ) continue;
		
		pPara = m_dataInf.ycPara(pRoute->group,m_ycCalcVector[i].pointNo);
		if ( pPara == NULL || !pPara->valid ) continue;

		SCD_FORMULA_DATA formulaData;
		if ( !TASK_LOAD::instance()->formula(pPara->formula,formulaData) ) continue;

		if ( formulaData.type == FORMULA_SYSTEM )
		{
			
		}
		else if ( formulaData.type == FORMULA_USER )
		{//�û��Զ�����㹫ʽ����
			bool isOk = false;
			double val = 0.0;
			m_expAnalyzer.setExp(formulaData.content.c_str());
			val = m_expAnalyzer.calc(&isOk);
			if ( isOk )
			{
				m_dataInf.yc(m_ycCalcVector[i].routeNo,m_ycCalcVector[i].pointNo,data);
				if ( val != data.val || 0 != data.quality )
				{//ֵ�仯��������仯�Ų����仯����
					data.val = val;
					data.updateTime = 0;
					data.quality = 0;
					m_dataInf.setYc(m_ycCalcVector[i].routeNo,m_ycCalcVector[i].pointNo,data);
				}
			}
			else
			{
				//wfp add at 20110516 ������Чֵˢ��
				data.val = val;
				data.updateTime = 0;
				data.quality = QUALITY_BIT_IV;		//��Ч
				m_dataInf.setYc(m_ycCalcVector[i].routeNo,m_ycCalcVector[i].pointNo,data);
				logprint(LOG_SCD_CALC_TASK,"<��ʽ����>ң�����ʧ��:route=%d,point=%d,err=%s",
											m_ycCalcVector[i].routeNo,m_ycCalcVector[i].pointNo,m_expAnalyzer.m_error.toAscii().data());
			}
		}//end if

	}//end for

	return true;
}

bool CCalcTask::calcYx()
{
	DAC_ROUTE* pRoute = NULL;
	DAC_GROUP* pGroup = NULL;
	DAC_YX* pPara = NULL;
	DAC_YX_DATA data;

	for (hUInt32 i=0;i<m_yxCalcVector.size();i++)
	{
		pRoute = m_commInf.route(m_yxCalcVector[i].routeNo);
		if ( pRoute == NULL ) continue;
		pGroup = m_commInf.group(pRoute->group);
		if ( pGroup == NULL ) continue;

		pPara = m_dataInf.yxPara(pRoute->group,m_yxCalcVector[i].pointNo);
		if ( pPara == NULL || !pPara->valid ) continue;

		SCD_FORMULA_DATA formulaData;
		if ( !TASK_LOAD::instance()->formula(pPara->formula,formulaData) ) continue;

		if ( formulaData.type == FORMULA_SYSTEM )
		{//
			if ( CString(pPara->formula) == CString(SCD_FORMULA_SYS_SGZ) )
			{
				calcYxSgz(m_yxCalcVector[i].routeNo,m_yxCalcVector[i].pointNo);
			}
			else if ( CString(pPara->formula) == CString(SCD_FORMULA_SYS_ROUTESTATE) )
			{
				calcYxComState(m_yxCalcVector[i].routeNo,m_yxCalcVector[i].pointNo);
			}
			//add by wlq 20170503���ӿ���״̬���㣬���ݵ�ѹֵ���жϵ�ѹ����0.5���λ������Ϊ��λ��
			else if ( CString(pPara->formula) == CString(SCD_FORMULA_SYS_BKSTATE) )
			{
				calcYxBkState(m_yxCalcVector[i].routeNo,m_yxCalcVector[i].pointNo);
			}

		}
		else if ( formulaData.type == FORMULA_USER )
		{//�û��Զ�����㹫ʽ����
			bool isOk = false;
			double val = 0.0;
			m_expAnalyzer.setExp(formulaData.content.c_str());
			val = m_expAnalyzer.calc(&isOk);
			if ( isOk )
			{
				m_dataInf.yx(m_yxCalcVector[i].routeNo,m_yxCalcVector[i].pointNo,data);
				if ( floatToInt(val) != data.val || 0 != data.quality )
				{//ֵ�仯��������仯�Ų����仯����
					data.val = floatToInt(val);
					data.updateTime = 0;
					data.quality = 0;
					m_dataInf.setYx(m_yxCalcVector[i].routeNo,m_yxCalcVector[i].pointNo,data);
				}
			}
			else
			{
				//wfp add at 20110516 ������Чֵˢ��
				data.val = floatToInt(val);
				data.updateTime = 0;
				data.quality = QUALITY_BIT_IV;
				m_dataInf.setYx(m_yxCalcVector[i].routeNo,m_yxCalcVector[i].pointNo,data);
				logprint(LOG_SCD_CALC_TASK,"<��ʽ����>ң�ż���ʧ��:route=%d,point=%d,err=%s",
				m_yxCalcVector[i].routeNo,m_yxCalcVector[i].pointNo,m_expAnalyzer.m_error.toAscii().data());
			}
		}//end if

	}//end for

	return true;
}

bool CCalcTask::calcKwh(int type)
{
	DAC_ROUTE* pRoute = NULL;
	DAC_GROUP* pGroup = NULL;
	DAC_YC* pPara = NULL;
	DAC_YC_DATA data;
	bool	ifsavedb = false;			//���μ����Ƿ���̼����ֵ
	bool	isnotify = false;			//������Ƿ���Ҫ���µ���ʼֵ

	for (hUInt32 i=0;i<m_kwhCalcVector.size();i++)
	{
		isnotify = false;
		pRoute = m_commInf.route(m_kwhCalcVector[i].routeNo);
		if ( pRoute == NULL ) continue;
		pGroup = m_commInf.group(pRoute->group);
		if ( pGroup == NULL ) continue;
		
		pPara = m_dataInf.ycPara(pRoute->group,m_kwhCalcVector[i].pointNo);
		if ( pPara == NULL || !pPara->valid ) continue;

		if(m_dataInf.ycMainRoute(pPara->srcGroup,pPara->srcNo,data))
		{
			//���ȿ����Ƿ�����ֵ��Ҫ���£�type=1��ˢ�£�2��ˢ�£�3��ˢ��
			//�������û�ü��������ȡ��һ�����ֵΪ��ʼ�����ֵ
			if (type == 3 || m_kwhCalcVector[i].init == 0 )
			{
				m_kwhCalcVector[i].dayinitval = data.val;
				m_kwhCalcVector[i].moninitval = data.val;
				m_kwhCalcVector[i].yearinitval = data.val;
				m_kwhCalcVector[i].init = 1;
				ifsavedb = true;
				isnotify = true;
			}
			else if (type == 2)
			{
				m_kwhCalcVector[i].dayinitval = data.val;
				m_kwhCalcVector[i].moninitval = data.val;
				ifsavedb = true;
				isnotify = true;
			}
			else if( type == 1)
			{
				m_kwhCalcVector[i].dayinitval = data.val;
				ifsavedb = true;
				isnotify = true;
			}
			else ;

			if( m_kwhCalcVector[i].type == 1 )
				data.val = data.val - m_kwhCalcVector[i].dayinitval;
			else if ( m_kwhCalcVector[i].type == 2 )
				data.val = data.val - m_kwhCalcVector[i].moninitval;
			else
				data.val = data.val - m_kwhCalcVector[i].yearinitval;
			data.updateTime = 0;
			data.quality = 0;
			m_dataInf.setYc(m_kwhCalcVector[i].routeNo,m_kwhCalcVector[i].pointNo,data);

			if(isnotify)
			{
				SCD_BEGINKWH_DATA	beginData;
				beginData.dayvalue = m_kwhCalcVector[i].dayinitval ;
				beginData.monthvalue = m_kwhCalcVector[i].moninitval;
				beginData.yearvalue = m_kwhCalcVector[i].yearinitval;
				TASK_LOAD::instance()->setBeginkwh(CString(CString(pGroup->code) + CString("/") + CString(pPara->code)).c_str(),beginData);	
			}
			logprint(LOG_SCD_CALC_TASK+1,"<��ʽ����>��ȼ���ɹ�:route=%d,point=%d,type = %d",m_kwhCalcVector[i].routeNo,m_kwhCalcVector[i].pointNo,m_kwhCalcVector[i].type);	
		}
	}//end for

	if(ifsavedb)
	{
		if(TASK_LOAD::instance()->updateBeginKwh())
			logprint(LOG_SCD_CALC_TASK+1,"<��ʽ����>���¼��������ʼֵ��ɹ���");
		else
			logprint(LOG_SCD_CALC_TASK+1,"<��ʽ����>�����������¼��������ʼֵ��ʧ�ܣ�������");
	}

	return true;
}




void CCalcTask::calcYxSgz(int routeNo,int pointNo)
{
	DAC_ROUTE* pRoute = m_commInf.route(routeNo);
	if ( pRoute == NULL ) return;
	DAC_GROUP* pGroup = m_commInf.group(pRoute->group);;
	if ( pGroup == NULL ) return;

	DAC_YX* pPara = NULL;
	DAC_YX_DATA data,sgzData;
	nUInt8 sgzVal = 0;
	ACE_OS::memset(&sgzData,0,sizeof(sgzData));
	int quality = 0;
	PDT_SET_BITS(quality,QUALITY_BIT_IV);	//��ʼ��Ч

	for (hUInt32 i=0;i<pGroup->yxNum;i++)
	{
		if ( pointNo == i ) continue;

		pPara = m_dataInf.yxPara(pRoute->group,i);
		if ( pPara == NULL || !pPara->valid ) continue;
		if ( !pPara->isInSgz ) continue;					//��������Ϊsgz����

		if ( !m_dataInf.yx(routeNo,i,data) )
			continue;

		//��Чֵ
		if ( PDT_BIT_ENABLED(data.quality,QUALITY_BIT_IV) )
			continue;

		sgzVal  = sgzVal || data.val;
		quality = 0;
		if ( sgzVal  > 0 )
		{
			m_dataInf.yx(routeNo,pointNo,sgzData);
			if ( sgzVal != sgzData.val || quality != sgzData.quality )
			{//ֵ�仯��������仯�Ų����仯����
				sgzData.val = sgzVal;
				sgzData.updateTime = 0;
				sgzData.quality = quality;
				m_dataInf.setYx(routeNo,pointNo,sgzData);
			}

			return;
		}

	}//end for

	//wfp add at 20111121
	if ( CONFIG::instance()->isYcInSgz() && isYcLimit(routeNo) )
		sgzVal = 1;

	m_dataInf.yx(routeNo,pointNo,sgzData);
	if ( sgzVal != sgzData.val || quality != sgzData.quality )
	{//ֵ�仯��������仯�Ų����仯����
		sgzData.val = sgzVal;
		sgzData.updateTime = 0;
		sgzData.quality = quality;
		m_dataInf.setYx(routeNo,pointNo,sgzData);
	}

}

//ң������¹���
bool CCalcTask::isYcLimit(int routeNo)
{
	DAC_ROUTE* pRoute = m_commInf.route(routeNo);
	if ( pRoute == NULL ) return false;
	DAC_GROUP* pGroup = m_commInf.group(pRoute->group);;
	if ( pGroup == NULL ) return false;

	DAC_YC* pPara = NULL;
	DAC_YC_DATA data;

	for (hUInt32 i=0;i<pGroup->ycNum;i++)
	{
		pPara = m_dataInf.ycPara(pRoute->group,i);
		if ( pPara == NULL || !pPara->valid ) continue;
		if ( !pPara->isEvent ) continue;					//��������Ϊsgz����

		if ( !m_dataInf.yc(routeNo,i,data) )
			continue;

		//��Чֵ
		if ( PDT_BIT_ENABLED(data.quality,QUALITY_BIT_IV) )
			continue;

		if ( pPara->limitType >= 1 )
		{
			return true;
		}

	}//end for
	return false;
}

void CCalcTask::calcYxComState(int routeNo,int pointNo)
{
	DAC_ROUTE* pRoute = m_commInf.route(routeNo);
	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(routeNo);
	if ( pRoute == NULL || pRouteInfo == NULL ) return;
	DAC_GROUP* pGroup = m_commInf.group(pRoute->group);
	if ( pGroup == NULL ) return;

	DAC_YX_DATA newData,oldData;
	if ( !m_dataInf.yx(routeNo,pointNo,oldData) )
		return;

	ACE_OS::memset(&newData,0,sizeof(newData));
	if ( pRouteInfo->state == ROUTE_STATE_DOWN )
		newData.val = 0;
	else
		newData.val = 1;

	if ( newData.val != oldData.val || newData.quality != oldData.quality )
	{//ֵ�仯��������仯�Ų����仯����
		m_dataInf.setYx(routeNo,pointNo,newData);
	}

}

void CCalcTask::calcYxBkState(int routeNo,int pointNo)
{
	DAC_ROUTE* pRoute = NULL;
	DAC_GROUP* pGroup = NULL;
	DAC_YX* pPara = NULL;
	DAC_YC_DATA data;
	DAC_YX_DATA newData,oldData;
	if ( !m_dataInf.yx(routeNo,pointNo,oldData) )
		return;

	ACE_OS::memset(&newData,0,sizeof(newData));

	pRoute = m_commInf.route(routeNo);
	if ( pRoute == NULL ) return;
	pGroup = m_commInf.group(pRoute->group);
	if ( pGroup == NULL ) return;

	pPara = m_dataInf.yxPara(pRoute->group,pointNo);
	if ( pPara == NULL || !pPara->valid ) return;

	if(m_dataInf.ycMainRoute(pPara->srcGroup,pPara->srcNo,data))
	{
		if( fabs(data.val) > 0.5 ) newData.val = 1;
		else newData.val = 0;

		newData.quality = data.quality;
	}

	if ( newData.val != oldData.val || newData.quality != oldData.quality )
	{//ֵ�仯��������仯�Ų����仯����
		m_dataInf.setYx(routeNo,pointNo,newData);
	}
}

//************************************************************************************
// *���������� : floatToInt
// *  ��    �� : ����˵�����������뽫floatתΪi
// *  ��ڲ��� : hFloat:����ĸ���ֵ
// *  �� �� ֵ : ת���������ֵ
// *  ��    ע : 
//************************************************************************************
int CCalcTask::floatToInt(hFloat val)
{
	if ( val > 0 ) 
		return int(val+0.5);
	else
		return int(val-0.5);
}
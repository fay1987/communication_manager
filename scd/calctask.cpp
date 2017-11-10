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
	logprint(LOG_SCD_BASE,"thread:%d , calcTask启动!",ACE_Thread::self());
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

		//wfp add at 20110602 for 采集系统未成功初始化时启动计算任务导致该任务运行失败.
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

		//可动态更新计算量关系，但该方式并不稳妥，可能捕获不到loadParaFlag变化
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
			//判断时间是否今天的开始，如果是则要更新电量初始值
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

			//判断结束
			calcYc();
			calcYx();
			calcKwh(kwhcalctype);
			/*if(m_cplb.mainCompute() >= 0)
				m_cplb.out();*/
			//fcalc();
		}

	}//end while

	
	logprint(LOG_SCD_BASE,"threadId = %d , scd loadtask线程退出!",ACE_Thread::self());

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

			//wfp changed at 20110909--后期可考虑不用vector,直接在内存测点中遍历
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

			//wfp changed at 20110909--后期可考虑不用vector,直接在内存测点中遍历
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

			//wfp changed at 20110909--后期可考虑不用vector,直接在内存测点中遍历
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
					//从历史库中读取
					m_kwhCalcVector.push_back(kwhcalcInfo);
					logprint(LOG_SCD_CALC_TASK+1,"<公式计算>电度初始化成功:计算代码=%s,日初始值=%f,月初始值=%f ,年初始值=%f,type=%d",
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
		{//用户自定义计算公式处理
			bool isOk = false;
			double val = 0.0;
			m_expAnalyzer.setExp(formulaData.content.c_str());
			val = m_expAnalyzer.calc(&isOk);
			if ( isOk )
			{
				m_dataInf.yc(m_ycCalcVector[i].routeNo,m_ycCalcVector[i].pointNo,data);
				if ( val != data.val || 0 != data.quality )
				{//值变化或质量码变化才产生变化数据
					data.val = val;
					data.updateTime = 0;
					data.quality = 0;
					m_dataInf.setYc(m_ycCalcVector[i].routeNo,m_ycCalcVector[i].pointNo,data);
				}
			}
			else
			{
				//wfp add at 20110516 增加无效值刷入
				data.val = val;
				data.updateTime = 0;
				data.quality = QUALITY_BIT_IV;		//无效
				m_dataInf.setYc(m_ycCalcVector[i].routeNo,m_ycCalcVector[i].pointNo,data);
				logprint(LOG_SCD_CALC_TASK,"<公式计算>遥测计算失败:route=%d,point=%d,err=%s",
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
			//add by wlq 20170503增加开关状态计算，根据电压值来判断电压大于0.5则合位，否则为分位置
			else if ( CString(pPara->formula) == CString(SCD_FORMULA_SYS_BKSTATE) )
			{
				calcYxBkState(m_yxCalcVector[i].routeNo,m_yxCalcVector[i].pointNo);
			}

		}
		else if ( formulaData.type == FORMULA_USER )
		{//用户自定义计算公式处理
			bool isOk = false;
			double val = 0.0;
			m_expAnalyzer.setExp(formulaData.content.c_str());
			val = m_expAnalyzer.calc(&isOk);
			if ( isOk )
			{
				m_dataInf.yx(m_yxCalcVector[i].routeNo,m_yxCalcVector[i].pointNo,data);
				if ( floatToInt(val) != data.val || 0 != data.quality )
				{//值变化或质量码变化才产生变化数据
					data.val = floatToInt(val);
					data.updateTime = 0;
					data.quality = 0;
					m_dataInf.setYx(m_yxCalcVector[i].routeNo,m_yxCalcVector[i].pointNo,data);
				}
			}
			else
			{
				//wfp add at 20110516 增加无效值刷入
				data.val = floatToInt(val);
				data.updateTime = 0;
				data.quality = QUALITY_BIT_IV;
				m_dataInf.setYx(m_yxCalcVector[i].routeNo,m_yxCalcVector[i].pointNo,data);
				logprint(LOG_SCD_CALC_TASK,"<公式计算>遥信计算失败:route=%d,point=%d,err=%s",
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
	bool	ifsavedb = false;			//本次计算是否存盘计算初值
	bool	isnotify = false;			//本测点是否需要更新到初始值

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
			//首先看看是否计算初值需要更新，type=1日刷新；2月刷新；3年刷新
			//如果从来没用计算过，则取第一个测点值为起始计算初值
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
			logprint(LOG_SCD_CALC_TASK+1,"<公式计算>电度计算成功:route=%d,point=%d,type = %d",m_kwhCalcVector[i].routeNo,m_kwhCalcVector[i].pointNo,m_kwhCalcVector[i].type);	
		}
	}//end for

	if(ifsavedb)
	{
		if(TASK_LOAD::instance()->updateBeginKwh())
			logprint(LOG_SCD_CALC_TASK+1,"<公式计算>更新计算电量初始值表成功。");
		else
			logprint(LOG_SCD_CALC_TASK+1,"<公式计算>！！！！更新计算电量初始值表失败！！！！");
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
	PDT_SET_BITS(quality,QUALITY_BIT_IV);	//初始无效

	for (hUInt32 i=0;i<pGroup->yxNum;i++)
	{
		if ( pointNo == i ) continue;

		pPara = m_dataInf.yxPara(pRoute->group,i);
		if ( pPara == NULL || !pPara->valid ) continue;
		if ( !pPara->isInSgz ) continue;					//事项点才作为sgz输入

		if ( !m_dataInf.yx(routeNo,i,data) )
			continue;

		//无效值
		if ( PDT_BIT_ENABLED(data.quality,QUALITY_BIT_IV) )
			continue;

		sgzVal  = sgzVal || data.val;
		quality = 0;
		if ( sgzVal  > 0 )
		{
			m_dataInf.yx(routeNo,pointNo,sgzData);
			if ( sgzVal != sgzData.val || quality != sgzData.quality )
			{//值变化或质量码变化才产生变化数据
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
	{//值变化或质量码变化才产生变化数据
		sgzData.val = sgzVal;
		sgzData.updateTime = 0;
		sgzData.quality = quality;
		m_dataInf.setYx(routeNo,pointNo,sgzData);
	}

}

//遥测参与事故总
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
		if ( !pPara->isEvent ) continue;					//事项点才作为sgz输入

		if ( !m_dataInf.yc(routeNo,i,data) )
			continue;

		//无效值
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
	{//值变化或质量码变化才产生变化数据
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
	{//值变化或质量码变化才产生变化数据
		m_dataInf.setYx(routeNo,pointNo,newData);
	}
}

//************************************************************************************
// *　函数名称 : floatToInt
// *  功    能 : 函数说明：四舍五入将float转为i
// *  入口参数 : hFloat:输入的浮点值
// *  返 回 值 : 转换后的整型值
// *  备    注 : 
//************************************************************************************
int CCalcTask::floatToInt(hFloat val)
{
	if ( val > 0 ) 
		return int(val+0.5);
	else
		return int(val-0.5);
}
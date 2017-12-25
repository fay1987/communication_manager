/*==============================================================================
* �ļ����� : dbsavetask.cpp
* ģ�� : ���ݿ��������
* ����ʱ�� : 2009-07-22 15:55:23
* ���� : wfp
* �汾 : v1.0

* һ���޸ĺ�汾 : 1.1
* �޸�ժҪ :  �Ժ���saveDb�������ݣ���������saveHisMonthCal��saveHisDayCal
* �޸��� :  hll
* �޸����� : 2017-01-18 09:30:12

* �����޸ĺ�汾 : 1.2
* �޸�ժҪ :  �Ժ���saveDb�������ݣ���������saveHisRSDayYcSta��saveHisRSYc
* �޸��� :  hll
* �޸����� : 2017-02-10 15:32:16
==============================================================================*/
#include "dbsavetask.h"
#include <ace/OS.h>
#include "sys/blog.h"
#include "scddef.h"
#include "dac/dac.h"
#include "dac/dacdef.h"
#include "loadtask.h"
#include "config.h"
#include <QStringList>

using namespace PDT;

CDbSaveTask::CDbSaveTask()
{
	ACE_Message_Queue<ACE_MT_SYNCH> *msg_queue=this->msg_queue();
	msg_queue->high_water_mark(HIGH_WATER_MARK);

	m_pRdbOp = NULL;
	m_pSql = NULL;

	if ( !checkDb() ) return;

	checkProcedure();
	
}

CDbSaveTask::~CDbSaveTask()
{
	if ( m_pRdbOp != NULL )
	{
		delete m_pRdbOp;
		m_pRdbOp = NULL;
	}
	if ( m_pSql != NULL )
	{
		m_pSql->destroy();
		m_pSql = NULL;
	}
}

bool CDbSaveTask::init()
{
	m_hisDataFile.init(SCD_HISDATA_FILEDIR,SCD_HISDATA_FILE_PREFIX);
	m_errDataFile.init(SCD_ERRDATA_FILEDIR,SCD_ERR_FILE_PREFIX);
	return true;
}

int CDbSaveTask::start()
{
	logprint( LOG_SCD_DBSAVE_TASK,"<CDbSaveTask>:����һ���߳�!" );
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

void CDbSaveTask::stop()
{
	if (thr_count() > 0) 
	{
		thr_mgr()-> cancel_task(this);
		//saveLeftDataToFile();			//wfp add at 20100104
		wait();
	}
}

int CDbSaveTask::svc()
{
	m_timeId = m_timer.add(1000);		//1000ms
	m_timer.start();

	ACE_Time_Value tv(0,10000);			//10ms
	ACE_Time_Value nonblock (0);
	ACE_Message_Block *messageBlock = NULL; 
	DAC_SYSTEM_INFO* pSystemInfo = m_commInf.systemInfo();
	CDateTime lastDateTime = CDateTime::currentDateTime();

	while ( true )
	{
		if ( thr_mgr()->testcancel( thr_mgr()->thr_self() ) )
			break;

		if (  PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_GROUP)
			||PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_YC)
			||PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_YX))
		{
			ACE_OS::sleep( tv );
			continue;
		}

		if ( m_timer.time_out(m_timeId) )
		{
			if ( CDateTime::currentDateTime().month() != lastDateTime.month() )
			{
				if ( checkDb() )
				{
					checkProcedure();
					lastDateTime = CDateTime::currentDateTime();
				}
				else
					ACE_OS::sleep( tv );

			}
		}

		if ( this->getq(messageBlock,&nonblock) != -1 )
		{
			//��ֹ�����ˮλ
			if( this->msg_queue()->message_length() >= (HIGH_WATER_MARK*2/3) )
			{
				//m_hisDataFile.save(messageBlock->rd_ptr(),messageBlock->length());
				messageBlock->release();
				continue;
			}

			//wfp add at 20111102
			if ( !checkDb() ) 
			{
				ungetq(messageBlock);
				ACE_OS::sleep( tv );
				continue;
			}

			if ( !saveDb(messageBlock) )
			{
				if ( m_pRdbOp != NULL && m_pRdbOp->isOpen() )
				{//sql������
					if ( CONFIG::instance()->isSaveErrFile() )
						m_errDataFile.save(messageBlock->rd_ptr(),messageBlock->length());

					messageBlock->release();
					continue;
				}
				ungetq(messageBlock);
				ACE_OS::sleep( tv );
			}
			else 
				messageBlock->release();
		}//end if getq()
		else
		{
			ACE_OS::sleep( tv );
		}

	}//end while
	return 0;
}

int CDbSaveTask::put(ACE_Message_Block *pMsgBlock, ACE_Time_Value *pTimeout /* = 0 */)
{
	ACE_Message_Queue<ACE_MT_SYNCH> *msg_queue=this->msg_queue();
	if(msg_queue->is_full())
	{
		//clear the queue using save file
		logprint(LOG_SCD_DBSAVE_TASK,"the queue is full");
	}
	return putq (pMsgBlock,pTimeout);
}

/*
 * �������ܣ����ڴ���ʣ�����ʷ���ݱ��浽�ļ�����ֹ��ʷ���ݶ�ʧ.
 */
void CDbSaveTask::saveLeftDataToFile()
{
	ACE_Time_Value nonblock (0);
	ACE_Message_Block *messageBlock = NULL; 

	while ( this->getq(messageBlock,&nonblock) != -1 )
	{
		m_hisDataFile.save(messageBlock->rd_ptr(),messageBlock->length());
		messageBlock->release();
	}//end if getq()
}

bool CDbSaveTask::saveDb(ACE_Message_Block* pMsgBlock)
{
	if ( pMsgBlock == NULL ) return false;
	if ( pMsgBlock->length() <= 0 ) return false;

	SCD_DATA_HEAD* pHead = (SCD_DATA_HEAD*)pMsgBlock->rd_ptr();  //������������ �������ֱ�

	switch (pHead->dataclass)   
	{
	case HIS_CLASS_YC:
		{
			SCD_DATA_YC* pData = (SCD_DATA_YC*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
			if ( !saveHisYc(pData) )
			{	
				logprint(LOG_SCD_DBSAVE_TASK_ERR,"<save ��ʷң�� error>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->no,(hFloat)pData->data.val);
				return false;
			}
			break;
		}

	//case HIS_CLASS_EVENT:
	//	{
	//		SCD_DATA_EVENT* pData = (SCD_DATA_EVENT*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
	//		if ( !saveHisEvent(pData) )
	//		{	
	//			logprint(LOG_SCD_DBSAVE_TASK_ERR,"<save event error>");
	//			return false;
	//		}
	//		break;
	//	}
	//case HIS_CLASS_DAY_STA:
	//	{
	//		SCD_DATA_STA* pData = (SCD_DATA_STA*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
	//		if ( !saveHisDaySta(pData) )
	//		{
	//			logprint(LOG_SCD_DBSAVE_TASK_ERR,"<save ��ͳ�� error>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);
	//			return false;
	//		}
	//		break;
	//	}
	//case HIS_CLASS_MONTH_STA:
	//	{
	//		SCD_DATA_STA* pData = (SCD_DATA_STA*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
	//		if ( !saveHisMonthSta(pData) )
	//		{
	//			logprint(LOG_SCD_DBSAVE_TASK_ERR,"<save ��ͳ�� error>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);
	//			return false;
	//		}
	//		break;
	//	}
	//case HIS_CLASS_SEASON_STA:
	//	{
	//		SCD_DATA_STA* pData = (SCD_DATA_STA*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
	//		if ( !saveHisSeasonSta(pData) )
	//		{
	//			logprint(LOG_SCD_DBSAVE_TASK_ERR,"<save ��ͳ�� error>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);
	//			return false;
	//		}
	//		break;
	//	}
	//case HIS_CLASS_YEAR_STA:
	//	{
	//		SCD_DATA_STA* pData = (SCD_DATA_STA*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
	//		if ( !saveHisYearSta(pData) )
	//		{
	//			logprint(LOG_SCD_DBSAVE_TASK_ERR,"<save ��ͳ�� error>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);
	//			return false;
	//		}
	//		break;
	//	}
	//case HIS_CLASS_RUN:
	//	{
	//		SCD_DATA_STA* pData = (SCD_DATA_STA*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
	//		if ( !saveHisBatteryRun(pData) )
	//		{
	//			logprint(LOG_SCD_DBSAVE_TASK_ERR,"<save ���м��� error>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);	
	//			return false;
	//		}
	//		break;
	//	}
	//case HIS_CLASS_CHARGE:
	//	{
	//		SCD_DATA_STA* pData = (SCD_DATA_STA*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
	//		if ( !saveHisBatteryCharge(pData) )
	//		{
	//			logprint(LOG_SCD_DBSAVE_TASK_ERR,"<save ��ŵ�� error>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);	
	//			return false;
	//		}
	//		break;
	//	}
	//case HIS_CLASS_DYNAMIC:
	//	{
	//		SCD_DATA_STA* pData = (SCD_DATA_STA*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
	//		if ( !saveHisBatteryDynamic(pData) )
	//		{
	//			logprint(LOG_SCD_DBSAVE_TASK_ERR,"<save ��̬�ŵ�� error>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);
	//			return false;
	//		}
	//		break;
	//	}
	//case HIS_CLASS_STATE:
	//	{
	//		SCD_DATA_STA* pData = (SCD_DATA_STA*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
	//		if ( !saveHisBatteryState(pData) )
	//		{
	//			logprint(LOG_SCD_DBSAVE_TASK_ERR,"<save ��ʷ״̬�� error>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);
	//			return false;
	//		}
	//		break;
	//	}
	//case HIS_CLASS_RSYC:    //2017/1/18 by hll ��վ����ͳ��
	//	{

	//		HIS_STA_RS_DATA* pData = (HIS_STA_RS_DATA*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
	//		if ( !saveHisRSYc(pData) )
	//		{	
	//			logprint(LOG_SCD_DBSAVE_TASK_ERR,"<save ��վ������ʷң�� error>rsname=%s,code=%s,val=%f",pData->rsname,pData->code,(hFloat)pData->val);
	//			return false;
	//		}
	//		break;
	//	}
	//	
	//case HIS_CLASS_RSDAY_STA:   //2017/1/18 by hll ��վ����������ͳ��
	//	{

	//		HIS_STA_RS_DATA* pData = (HIS_STA_RS_DATA*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
	//		if ( !saveHisRSDayYcSta(pData) )
	//		{	
	//			logprint(LOG_SCD_DBSAVE_TASK_ERR,"<save ��վ������ͳ�� error>rsname=%s,code=%s,val=%f",pData->rsname,pData->code,(hFloat)pData->val);
	//			return false;
	//		}
	//		break;
	//	}

	//case HIS_CLASS_DAY_CAL:   //2017/02/10  by hll ��վ��������ֵ��������ֵmyc
	//	{
	//		HIS_STA_RS_DATA* pData = (HIS_STA_RS_DATA*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
	//		if ( !saveHisDayCal(pData) )
	//		{	
	//			logprint(2018,"<save ��վ��������ֵ��ͳ�� error>rsname=%s,code=%s,val=%f",pData->rsname,pData->code,(hFloat)pData->val);
	//			return false;
	//		}
	//		break;
	//	}
	//case HIS_CLASS_MONTH_CAL: //2017/02/10  by hll ��վ��������ֵyyc
	//	{
	//		HIS_STA_RS_DATA* pData = (HIS_STA_RS_DATA*)(pMsgBlock->rd_ptr()+sizeof(SCD_DATA_HEAD));
	//		if ( !saveHisMonthCal(pData) )
	//		{	
	//			logprint(LOG_SCD_DBSAVE_TASK_ERR,"<save ��վ��������ֵ��ͳ�� error>rsname=%s,code=%s,val=%f",pData->rsname,pData->code,(hFloat)pData->val);
	//			return false;
	//		}
	//		break;
	//	}
		
	default:
		logprint(LOG_SCD_DBSAVE_TASK_ERR,"<show>unknow data");
		return false;

	}//end switch

	return true;
}

bool CDbSaveTask::saveHisYc(const SCD_DATA_YC* pData)
{
	if (pData == NULL ) return false;

	DAC_GROUP* pGroup = NULL;
	pGroup = m_commInf.group(pData->group);
	if ( pGroup == NULL ) return false;

	DAC_YC* pPara = m_dataInf.ycPara(pData->group,pData->no);
	if ( pPara == NULL || !pPara->valid ) return false;

	//ִ�д洢����
	//if ( !checkYcProcedure() ) return false;

	CDateTime curTime = CDateTime::currentDateTime();
	CDateTime updateTime = CDateTime( ACE_Time_Value((hUInt32)pData->data.updateTime,0) );

	//�յ����ڵ�ǰʱ�����ʷ���ݣ�ֱ�Ӷ���
	if ( (updateTime.year()*1000 + updateTime.month()) > (curTime.year()*1000 + curTime.month()) )
	{
		logprint(LOG_SCD_DBSAVE_TASK_ERR,"<show yc error>����ʱ����ڵ�ǰʱ��");
		return false;
	}
	char monthStr[8] = "";
	ACE_OS::sprintf(monthStr,"%02d",updateTime.month());


	CString tableName;

	hUInt32 ycdatatype = pData->ycdataType;
	if (ycdatatype >= 1 && ycdatatype <= 30)
	{
		tableName = "tdata_power";
	}
	else if (ycdatatype > 30 && ycdatatype <= 34)
	{
		tableName = "tdata_energy";
	}
	else if ((ycdatatype > 40 && ycdatatype <= 51) || ycdatatype > 52)
	{
		tableName = "tdata_other";
	}

	//wfp temp
	DAC_SENDDEV* pSendDev = m_commInf.getSenddevbygroup(pData->group);

	m_pSql->setOperate(CSql::OP_Insert);
	m_pSql->setTableName(tableName.c_str());
	m_pSql->insertField("f_sdevcode",pSendDev->code);
	CDateTime dateTime( ACE_Time_Value((hUInt32)pData->data.updateTime,0) );

	m_pSql->insertField("f_date",dateTime.toString("yyyyMMdd"));
	m_pSql->insertField("f_time",dateTime.toString("hhmmss"));

	m_pSql->insertField("f_type",(hUInt32)pData->ycdataType);
	m_pSql->insertField("f_value",(hFloat)pData->data.val);
	
	m_pSql->insertField("f_updatetime",dateTime.toString("yyyyMMdd hhmmss"));

	if ( m_pRdbOp->exec(*m_pSql) == -1 )
	{
		return false;
	}
	
	//wfp add
	//char sqlStr[512] = "";
	//ACE_OS::sprintf(sqlStr, "execute %s \'%s\',\'%s\',\'%s\',\'%s\',%f,%d,%d",
	//	SCD_YC_INSERT_PROCEDURE,tableName.c_str(),pGroup->code,pPara->code,
	//	CDateTime( ACE_Time_Value((hUInt32)pData->data.updateTime,0) ).toString(DT_FMT_DEF).c_str(),
	//	(hFloat)pData->data.val,(hUInt32)pData->dataType,CONFIG::instance()->saveMode());

	//if ( m_pRdbOp->exec(sqlStr) == -1 )
	//{
	//	return false;
	//}

	logprint(LOG_SCD_DBSAVE_TASK,"<show yc>grpno=%d,no=%d,type=%d,val=%f",
								 (hInt32)pData->group,(hUInt32)pData->no,(hUInt32)pData->dataType,(hFloat)pData->data.val);
	return true;
}

bool CDbSaveTask::saveHisEvent(const SCD_DATA_EVENT* pData)
{
	if (pData == NULL ) return false;

	m_pSql->setOperate(CSql::OP_Insert);
	m_pSql->setTableName(SCD_EVENT);

	m_pSql->insertField("f_evtclass",(hInt32)pData->evtClass);
	m_pSql->insertField("f_evttype",(hInt32)pData->evtType);
	CDateTime dateTime( ACE_Time_Value((hUInt32)pData->occurTime,0) );
	m_pSql->insertField("f_time",dateTime);

	m_pSql->insertField("f_stacode",pData->staCode);
	m_pSql->insertField("f_parentcode",pData->parentCode);
	m_pSql->insertField("f_code",pData->code);
	m_pSql->insertField("f_usercode",pData->user);
	m_pSql->insertField("f_level",(hUInt8)pData->level);
	m_pSql->insertField("f_state",(hUInt8)pData->stateValue);
	m_pSql->insertField("f_state1",(hUInt8)pData->stateValue1);
	m_pSql->insertField("f_float",(hFloat)pData->floatValue);
	m_pSql->insertField("f_float1",(hFloat)pData->floatValue1);
	m_pSql->insertField("f_alarmflag",(hUInt8)pData->alarmFlag);
	m_pSql->insertField("f_info",pData->info);

	if ( m_pRdbOp->exec(*m_pSql) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBSAVE_TASK,"<show event>type=%d",(hUInt32)pData->evtType);
	return true;
}

bool CDbSaveTask::saveHisDaySta(const SCD_DATA_STA* pData)
{
	if (pData == NULL ) return false;
	CString tableName;

	DAC_GROUP* pGroup = m_commInf.group(pData->group);
	if ( pGroup == NULL ) return false;
	DAC_YC* pPara = m_dataInf.ycPara(pData->group,pData->data.no);
	if ( pPara == NULL || !pPara->valid ) return false;

	//ִ�д洢����
	CDateTime curTime = CDateTime::currentDateTime();
	CDateTime updateTime = CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) );

	//�յ����ڵ�ǰʱ�����ʷ���ݣ�ֱ�Ӷ���
	if ( updateTime.year() > curTime.year() )
	{
		logprint(LOG_SCD_DBSAVE_TASK_ERR,"<show day yc error>����ʱ����ڵ�ǰʱ��");
		return false;
	}
	tableName = CString(SCD_DAY_STA_TABLE_PREFIX) + CString().from((hUInt32)updateTime.year());
	//if ( !checkDayStaProcedure() ) return false;
	/*
	m_pSql->setOperate(CSql::OP_Insert);
	m_pSql->setTableName(tableName.c_str());
	m_pSql->insertField("f_grpcode",pGroup->code);
	m_pSql->insertField("f_code",pPara->code);

	m_pSql->insertField("f_time",CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ));
	m_pSql->insertField("f_value",(hFloat)pData->data.val);

	if ( pData->data.type == HIS_TYPE_DAY_MAXVAL || pData->data.type == HIS_TYPE_DAY_MINVAL )
		m_pSql->insertField( "f_occurtime",CDateTime( ACE_Time_Value((hUInt32)pData->data.occurTime,0) ));

	m_pSql->insertField("f_type",(hUInt32)pData->data.type);
	if ( m_pRdbOp->exec(*m_pSql) == -1 )
	{
		return false;
	}
	*/
	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%f,%d,%d",
							SCD_STA_INSERT_PROCEDURE,tableName.c_str(),pGroup->code,pPara->code,
							CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ).toString(DT_FMT_DEF).c_str(),
							CDateTime( ACE_Time_Value((hUInt32)pData->data.occurTime,0) ).toString(DT_FMT_DEF).c_str(),
							(hFloat)pData->data.val,(hUInt32)pData->data.type,CONFIG::instance()->saveMode());
	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBSAVE_TASK,"<show ��ͳ��>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);
	return true;
}

bool CDbSaveTask::saveHisMonthSta(const SCD_DATA_STA* pData)
{
	if (pData == NULL ) return false;

	DAC_GROUP* pGroup = m_commInf.group(pData->group);
	if ( pGroup == NULL ) return false;
	DAC_YC* pPara = m_dataInf.ycPara(pData->group,pData->data.no);
	if ( pPara == NULL || !pPara->valid ) return false;
	/*
	m_pSql->setOperate(CSql::OP_Insert);
	m_pSql->setTableName(SCD_MONTH_STA_TABLE);
	m_pSql->insertField("f_grpcode",pGroup->code);
	m_pSql->insertField("f_code",pPara->code);

	m_pSql->insertField("f_time",CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ));
	m_pSql->insertField("f_value",(hFloat)pData->data.val);

	if ( pData->data.type == HIS_TYPE_MONTH_MAXVAL || pData->data.type == HIS_TYPE_MONTH_MINVAL )
		m_pSql->insertField("f_occurtime",CDateTime( ACE_Time_Value((hUInt32)pData->data.occurTime,0) ));

	m_pSql->insertField("f_type",(hUInt32)pData->data.type);
	if ( m_pRdbOp->exec(*m_pSql) == -1 )
	{
		//CString str = m_pSql->sql().c_str();//wfp_test
		return false;
	}
	*/
	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%f,%d,%d",
							SCD_STA_INSERT_PROCEDURE,SCD_MONTH_STA_TABLE,pGroup->code,pPara->code,
							CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ).toString(DT_FMT_DEF).c_str(),
							CDateTime( ACE_Time_Value((hUInt32)pData->data.occurTime,0) ).toString(DT_FMT_DEF).c_str(),
							(hFloat)pData->data.val,(hUInt32)pData->data.type,CONFIG::instance()->saveMode());
	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBSAVE_TASK,"<show ��ͳ��>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);
	return true;
}

bool CDbSaveTask::saveHisSeasonSta(const SCD_DATA_STA* pData)
{
	if (pData == NULL ) return false;

	DAC_GROUP* pGroup = m_commInf.group(pData->group);
	if ( pGroup == NULL ) return false;
	DAC_YC* pPara = m_dataInf.ycPara(pData->group,pData->data.no);
	if ( pPara == NULL || !pPara->valid ) return false;

	/*
	m_pSql->setOperate(CSql::OP_Insert);
	m_pSql->setTableName(SCD_SEASON_STA_TABLE);
	m_pSql->insertField("f_grpcode",pGroup->code);
	m_pSql->insertField("f_code",pPara->code);

	m_pSql->insertField("f_time",CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ));
	m_pSql->insertField("f_value",(hFloat)pData->data.val);

	if ( pData->data.type == HIS_TYPE_SEASON_MAXVAL || pData->data.type == HIS_TYPE_SEASON_MINVAL )
		m_pSql->insertField("f_occurtime",CDateTime( ACE_Time_Value((hUInt32)pData->data.occurTime,0) ));

	m_pSql->insertField("f_type",(hUInt32)pData->data.type);
	if ( m_pRdbOp->exec(*m_pSql) == -1 )
	{
		//CString str = m_pSql->sql().c_str();//wfp_test
		return false;
	}
	*/
	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%f,%d,%d",
							SCD_STA_INSERT_PROCEDURE,SCD_SEASON_STA_TABLE,pGroup->code,pPara->code,
							CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ).toString(DT_FMT_DEF).c_str(),
							CDateTime( ACE_Time_Value((hUInt32)pData->data.occurTime,0) ).toString(DT_FMT_DEF).c_str(),
							(hFloat)pData->data.val,(hUInt32)pData->data.type,CONFIG::instance()->saveMode());
	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBSAVE_TASK,"<show ��ͳ��>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);
	return true;
}

bool CDbSaveTask::saveHisYearSta(const SCD_DATA_STA* pData)
{
	if (pData == NULL ) return false;

	DAC_GROUP* pGroup = m_commInf.group(pData->group);
	if ( pGroup == NULL ) return false;
	DAC_YC* pPara = m_dataInf.ycPara(pData->group,pData->data.no);
	if ( pPara == NULL || !pPara->valid ) return false;
	/*
	m_pSql->setOperate(CSql::OP_Insert);
	m_pSql->setTableName(SCD_YEAR_STA_TABLE);
	m_pSql->insertField("f_grpcode",pGroup->code);
	m_pSql->insertField("f_code",pPara->code);

	m_pSql->insertField("f_time",CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ));
	m_pSql->insertField("f_value",(hFloat)pData->data.val);

	if ( pData->data.type == HIS_TYPE_YEAR_MAXVAL || pData->data.type == HIS_TYPE_YEAR_MINVAL )
		m_pSql->insertField("f_occurtime",CDateTime( ACE_Time_Value((hUInt32)pData->data.occurTime,0) ));

	m_pSql->insertField("f_type",(hUInt32)pData->data.type);
	if ( m_pRdbOp->exec(*m_pSql) == -1 )
	{
		return false;
	}
	*/
	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%f,%d,%d",
							SCD_STA_INSERT_PROCEDURE,SCD_YEAR_STA_TABLE,pGroup->code,pPara->code,
							CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ).toString(DT_FMT_DEF).c_str(),
							CDateTime( ACE_Time_Value((hUInt32)pData->data.occurTime,0) ).toString(DT_FMT_DEF).c_str(),
							(hFloat)pData->data.val,(hUInt32)pData->data.type,CONFIG::instance()->saveMode());
	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBSAVE_TASK,"<show ��ͳ��>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);
	return true;
}

//���м���
bool CDbSaveTask::saveHisBatteryRun(const SCD_DATA_STA* pData)
{
	if (pData == NULL ) return false;

	DAC_GROUP* pGroup = m_commInf.group(pData->group);
	if ( pGroup == NULL ) return false;
	DAC_YC* pPara = m_dataInf.ycPara(pData->group,pData->data.no);
	if ( pPara == NULL || !pPara->valid ) return false;

	CDateTime curTime = CDateTime::currentDateTime();
	CDateTime updateTime = CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) );

	//�յ����ڵ�ǰʱ�����ʷ���ݣ�ֱ�Ӷ���
	if ( (updateTime.year()*1000 + updateTime.month()) > (curTime.year()*1000 + curTime.month()) )
	{
		logprint(LOG_SCD_DBSAVE_TASK_ERR,"<show battery run error>����ʱ����ڵ�ǰʱ��");
		return false;
	}
	char monthStr[8] = "";
	ACE_OS::sprintf(monthStr,"%02d",updateTime.month());

	CString tableName = CString(SCD_BAT_RUN_TB_PREFIX) + CString().from((hUInt32)updateTime.year())+ CString(monthStr);

	m_pSql->setOperate(CSql::OP_Insert);
	m_pSql->setTableName(tableName.c_str());
	m_pSql->insertField("f_devcode",pGroup->code);
	m_pSql->insertField("f_code",pPara->code);

	m_pSql->insertField("f_time",CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ));
	m_pSql->insertField("f_value",(hFloat)pData->data.val);
	m_pSql->insertField("f_type",(hUInt32)pData->data.type);
	m_pSql->insertField("f_state",(hUInt32)pData->data.valstate);
	if ( m_pRdbOp->exec(*m_pSql) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBSAVE_TASK,"<show �������м��>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);
	return true;
}

//��ŵ��
bool CDbSaveTask::saveHisBatteryCharge(const SCD_DATA_STA* pData)
{
	if (pData == NULL ) return false;

	DAC_GROUP* pGroup = m_commInf.group(pData->group);
	if ( pGroup == NULL ) return false;
	DAC_YC* pPara = m_dataInf.ycPara(pData->group,pData->data.no);
	if ( pPara == NULL || !pPara->valid ) return false;

	m_pSql->setOperate(CSql::OP_Insert);
	m_pSql->setTableName(SCD_BAT_CHARGE_TABLE);
	m_pSql->insertField("f_devcode",pGroup->code);
	m_pSql->insertField("f_code",pPara->code);

	m_pSql->insertField("f_time",CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ));
	m_pSql->insertField("f_value",(hFloat)pData->data.val);
	m_pSql->insertField("f_type",(hUInt32)pData->data.type);
	m_pSql->insertField("f_state",(hUInt32)pData->data.valstate);
	if ( m_pRdbOp->exec(*m_pSql) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBSAVE_TASK,"<show ���س�ŵ�>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);
	return true;
}

//��̬�ŵ��
bool CDbSaveTask::saveHisBatteryDynamic(const SCD_DATA_STA* pData)
{
	if (pData == NULL ) return false;

	DAC_GROUP* pGroup = m_commInf.group(pData->group);
	if ( pGroup == NULL ) return false;
	DAC_YC* pPara = m_dataInf.ycPara(pData->group,pData->data.no);
	if ( pPara == NULL || !pPara->valid ) return false;

	m_pSql->setOperate(CSql::OP_Insert);
	m_pSql->setTableName(SCD_BAT_DYNAMIC_TABLE);
	m_pSql->insertField("f_devcode",pGroup->code);
	m_pSql->insertField("f_code",pPara->code);

	m_pSql->insertField("f_time",CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ));
	m_pSql->insertField("f_value",(hFloat)pData->data.val);
	m_pSql->insertField("f_type",(hUInt32)pData->data.type);
	m_pSql->insertField("f_state",(hUInt32)pData->data.valstate);
	if ( m_pRdbOp->exec(*m_pSql) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBSAVE_TASK,"<show ��̬�ŵ�>grpno=%d,no=%d,val=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.val);
	return true;
}

//����״̬��
bool CDbSaveTask::saveHisBatteryState(const SCD_DATA_STA* pData)
{
	if (pData == NULL ) return false;

	DAC_GROUP* pGroup = m_commInf.group(pData->group);
	if ( pGroup == NULL ) return false;
	DAC_YX* pPara = m_dataInf.yxPara(pData->group,pData->data.no);
	if ( pPara == NULL || !pPara->valid ) return false;

	m_pSql->setOperate(CSql::OP_Insert);
	m_pSql->setTableName(SCD_BAT_STATE_TABLE);
	m_pSql->insertField("f_devcode",pGroup->code);
	m_pSql->insertField("f_code",pPara->code);

	m_pSql->insertField("f_state",(hUInt32)pData->data.valstate);
	m_pSql->insertField("f_time",CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ));
	
	if ( m_pRdbOp->exec(*m_pSql) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBSAVE_TASK,"<show ����״̬>grpno=%d,no=%d,state=%f",(hInt32)pData->group,(hUInt32)pData->data.no,(hFloat)pData->data.valstate);
	return true;
}

//2017/1/18 hll ��վ��������ͳ�ƺ���
bool  CDbSaveTask::saveHisRSYc(const HIS_STA_RS_DATA* pData)
{
	if (pData == NULL ) return false;

	/*DAC_GROUP* pGroup = NULL;
	pGroup = m_commInf.group(pData->group);
	if ( pGroup == NULL ) return false;

	DAC_YC* pPara = m_dataInf.ycPara(pData->group,pData->no);
	if ( pPara == NULL || !pPara->valid ) return false;*/

	//ִ�д洢����
	//if ( !checkYcProcedure() ) return false;

	CDateTime curTime = CDateTime::currentDateTime();
	CDateTime updateTime = CDateTime( ACE_Time_Value((hUInt32)pData->saveTime,0) );

	//�յ����ڵ�ǰʱ�����ʷ���ݣ�ֱ�Ӷ���
	if ( (updateTime.year()*100 + updateTime.month()) > (curTime.year()*100 + curTime.month()) )
	{
		logprint(LOG_SCD_DBSAVE_TASK_ERR,"<show rsyc error>����ʱ����ڵ�ǰʱ��");
		return false;
	}
	char monthStr[8] = "";
	ACE_OS::sprintf(monthStr,"%02d",updateTime.month());

	CString tableName = CString(SCD_YC_TABLE_PREFIX) + CString().from((hUInt32)curTime.year())+ CString(monthStr);

	//wfp temp
	//m_pSql->setOperate(CSql::OP_Insert);
	//m_pSql->setTableName(tableName.c_str());
	//m_pSql->insertField("f_grpcode",pGroup->code);
	//m_pSql->insertField("f_code",pPara->code);

	//CDateTime dateTime( ACE_Time_Value((hUInt32)pData->data.updateTime,0) );
	//m_pSql->insertField("f_time",dateTime);
	//m_pSql->insertField("f_value",(hFloat)pData->data.val);
	//m_pSql->insertField("f_type",(hUInt32)pData->dataType);

	//if ( m_pRdbOp->exec(*m_pSql) == -1 )
	//{
	//	return false;
	//}

	//wfp add
	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\',\'%s\',\'%s\',\'%s\',%f,%d,%d",
		SCD_YC_INSERT_PROCEDURE,tableName.c_str(),pData->rsname,pData->code,
		CDateTime( ACE_Time_Value((hUInt32)pData->saveTime,0) ).toString(DT_FMT_DEF).c_str(),
		(hFloat)pData->val,1,CONFIG::instance()->saveMode());

	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBSAVE_TASK,"<show ��վ����Сʱ�ǵ�������ͳ��>rsname=%s,code=%s,type=%d,val=%f",
		(hInt32)pData->rsname,(hUInt32)pData->code,(hUInt32)pData->type,(hFloat)pData->val);
	return true;

}

//2017/1/18 hll ��վ����������ͳ�ƺ���
bool  CDbSaveTask::saveHisRSDayYcSta(const HIS_STA_RS_DATA* pData)
{
	if (pData == NULL ) return false;
	CString tableName;

	/*DAC_GROUP* pGroup = m_commInf.group(pData->group);
	if ( pGroup == NULL ) return false;
	DAC_YC* pPara = m_dataInf.ycPara(pData->group,pData->data.no);
	if ( pPara == NULL || !pPara->valid ) return false;*/

	//ִ�д洢����
	CDateTime curTime = CDateTime::currentDateTime();
	CDateTime updateTime = CDateTime( ACE_Time_Value((hUInt32)pData->saveTime,0) );

	//�յ����ڵ�ǰʱ�����ʷ���ݣ�ֱ�Ӷ���
	if ( updateTime.year() > curTime.year() )
	{
		logprint(LOG_SCD_DBSAVE_TASK_ERR,"<show day rsyc error>����ʱ����ڵ�ǰʱ��");
		return false;
	}
	tableName = CString(SCD_DAY_STA_TABLE_PREFIX) + CString().from((hUInt32)updateTime.year());
	//if ( !checkDayStaProcedure() ) return false;
	/*
	m_pSql->setOperate(CSql::OP_Insert);
	m_pSql->setTableName(tableName.c_str());
	m_pSql->insertField("f_grpcode",pGroup->code);
	m_pSql->insertField("f_code",pPara->code);

	m_pSql->insertField("f_time",CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ));
	m_pSql->insertField("f_value",(hFloat)pData->data.val);

	if ( pData->data.type == HIS_TYPE_DAY_MAXVAL || pData->data.type == HIS_TYPE_DAY_MINVAL )
		m_pSql->insertField( "f_occurtime",CDateTime( ACE_Time_Value((hUInt32)pData->data.occurTime,0) ));

	m_pSql->insertField("f_type",(hUInt32)pData->data.type);
	if ( m_pRdbOp->exec(*m_pSql) == -1 )
	{
		return false;
	}
	*/
	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%f,%d,%d",
							SCD_STA_INSERT_PROCEDURE,tableName.c_str(),pData->rsname,pData->code,
							CDateTime( ACE_Time_Value((hUInt32)pData->saveTime,0) ).toString(DT_FMT_DEF).c_str(),
							CDateTime( ACE_Time_Value((hUInt32)pData->occurTime,0) ).toString(DT_FMT_DEF).c_str(),
							(hFloat)pData->val,(hUInt32)pData->type,CONFIG::instance()->saveMode());
	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBSAVE_TASK,"<show ��վ�����յ���ͳ������>rsname=%s,code=%s,val=%f,type=%d",pData->rsname,pData->code,(hFloat)pData->val,(hInt32)pData->type);
	return true;

}
//2017/02/10 by hll ��վ��������ֵ ������ֵ����myc
bool CDbSaveTask::saveHisDayCal(const HIS_STA_RS_DATA* pData)
{
	if (pData == NULL ) return false;

	//ִ�д洢����
	CDateTime curTime = CDateTime::currentDateTime();
	CDateTime updateTime = CDateTime( ACE_Time_Value((hUInt32)pData->saveTime,0) );

	//�յ����ڵ�ǰʱ�����ʷ���ݣ�ֱ�Ӷ���
	if ( updateTime.year() > curTime.year() )
	{
		logprint(2018,"<show day rsyc error>����ʱ����ڵ�ǰʱ��");
		return false;
	}
	
	/*
	m_pSql->setOperate(CSql::OP_Insert);
	m_pSql->setTableName(SCD_MONTH_STA_TABLE);
	m_pSql->insertField("f_grpcode",pGroup->code);
	m_pSql->insertField("f_code",pPara->code);

	m_pSql->insertField("f_time",CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ));
	m_pSql->insertField("f_value",(hFloat)pData->data.val);

	if ( pData->data.type == HIS_TYPE_MONTH_MAXVAL || pData->data.type == HIS_TYPE_MONTH_MINVAL )
		m_pSql->insertField("f_occurtime",CDateTime( ACE_Time_Value((hUInt32)pData->data.occurTime,0) ));

	m_pSql->insertField("f_type",(hUInt32)pData->data.type);
	if ( m_pRdbOp->exec(*m_pSql) == -1 )
	{
		//CString str = m_pSql->sql().c_str();//wfp_test
		return false;
	}
	*/

	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%f,%d,%d",
							SCD_STA_INSERT_PROCEDURE,SCD_MONTH_STA_TABLE,pData->rsname,pData->code,
							CDateTime( ACE_Time_Value((hUInt32)pData->saveTime,0) ).toString(DT_FMT_DEF).c_str(),
							CDateTime( ACE_Time_Value((hUInt32)pData->occurTime,0) ).toString(DT_FMT_DEF).c_str(),
							(hFloat)pData->val,(hUInt32)pData->type,CONFIG::instance()->saveMode());
	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		return false;
	}

	logprint(2018,"<show ����վ�չ�����ֵ���������ֵͳ��>rsname=%d,code=%d,val=%f, sqlstr=%s",(hInt32)pData->rsname,(hUInt32)pData->code,(hFloat)pData->val,sqlStr);
	return true;
}

//2017/02/10 by hll ��վ��������ֵyyc
bool CDbSaveTask::saveHisMonthCal(const HIS_STA_RS_DATA* pData)
{
	if (pData == NULL ) return false;

	//ִ�д洢����
	CDateTime curTime = CDateTime::currentDateTime();
	CDateTime updateTime = CDateTime( ACE_Time_Value((hUInt32)pData->saveTime,0) );

	//�յ����ڵ�ǰʱ�����ʷ���ݣ�ֱ�Ӷ���
	if ( updateTime.year() > curTime.year() )
	{
		logprint(LOG_SCD_DBSAVE_TASK_ERR,"<show day rsyc error>����ʱ����ڵ�ǰʱ��");
		return false;
	}
	
	/*
	m_pSql->setOperate(CSql::OP_Insert);
	m_pSql->setTableName(SCD_MONTH_STA_TABLE);
	m_pSql->insertField("f_grpcode",pGroup->code);
	m_pSql->insertField("f_code",pPara->code);

	m_pSql->insertField("f_time",CDateTime( ACE_Time_Value((hUInt32)pData->data.saveTime,0) ));
	m_pSql->insertField("f_value",(hFloat)pData->data.val);

	if ( pData->data.type == HIS_TYPE_MONTH_MAXVAL || pData->data.type == HIS_TYPE_MONTH_MINVAL )
		m_pSql->insertField("f_occurtime",CDateTime( ACE_Time_Value((hUInt32)pData->data.occurTime,0) ));

	m_pSql->insertField("f_type",(hUInt32)pData->data.type);
	if ( m_pRdbOp->exec(*m_pSql) == -1 )
	{
		//CString str = m_pSql->sql().c_str();//wfp_test
		return false;
	}
	*/
	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "execute %s \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%f,%d,%d",
							SCD_STA_INSERT_PROCEDURE,SCD_YEAR_STA_TABLE,pData->rsname,pData->code,
							CDateTime( ACE_Time_Value((hUInt32)pData->saveTime,0) ).toString(DT_FMT_DEF).c_str(),
							CDateTime( ACE_Time_Value((hUInt32)pData->occurTime,0) ).toString(DT_FMT_DEF).c_str(),
							(hFloat)pData->val,(hUInt32)pData->type,CONFIG::instance()->saveMode());
	if ( m_pRdbOp->exec(sqlStr) == -1 )
	{
		return false;
	}

	logprint(LOG_SCD_DBSAVE_TASK,"<show ����վ��������ֵͳ��>rsname=%d,code=%d,val=%f",(hInt32)pData->rsname,(hUInt32)pData->code,(hFloat)pData->val);
	return true;
}


bool CDbSaveTask::checkProcedure()
{
	CDateTime dateTime = CDateTime::currentDateTime();
	checkYcProcedure(dateTime.year(),dateTime.month()-1);
	//checkYcProcedure(dateTime.year(),dateTime.month()+1);

	//checkDayStaProcedure(dateTime.year());
	//checkDayStaProcedure(dateTime.year()+1);

	//����
	//if ( CONFIG::instance()->hasBattery() )
	//{
	//	checkBatteryProcedure(dateTime.year(),dateTime.month());
	//	checkBatteryProcedure(dateTime.year(),dateTime.month()+1);
	//}

	return true;
}

bool CDbSaveTask::checkYcProcedure(hUInt32 year,hUInt32 month)
{
	if ( month < 1 || month > 12) return false;

	//char monthStr[8] = "";
	//ACE_OS::sprintf(monthStr,"%02d",month);

	////ִ�д洢����
	//CString tableName = CString(SCD_YC_TABLE_PREFIX) + CString().from(year)+ CString(monthStr);

	//char sql[256] = "";
	////wfp changed at 20111028
	//ACE_OS::sprintf(sql,"execute %s \'%s\',%d",SCD_YC_PROCEDURE,tableName.c_str(),CONFIG::instance()->saveMode());
	//if ( m_pRdbOp->exec( sql )== -1 ) 
	//{
	//	logprint(LOG_SCD_DBSAVE_TASK_ERR,"ִ�д洢����<%s> error",sql);
	//	return false;
	//}

#ifdef _ARM_LINUX
	CDateTime dt;
	dt.year(year);
	dt.month(month);
	dt.day(1);

	QStringList tableName;
	tableName << "tdata_power" << "tdata_energy" << "tdata_other"<<"tdata_harmonic" << "tdata_minmax" << "tdata_hisfgp"<< "tdata_report";
	
	for (int i = 0; i < tableName.length(); i++)
	{
		m_pSql->setOperate(CSql::OP_Delete);
		m_pSql->setTableName(tableName[i].toLatin1().data());
		m_pSql->whereField("f_date",dt.toString("yyyyMMdd"),CSql::CP_Less);
		if ( m_pRdbOp->exec(*m_pSql) == -1 )
		{
			return false;
		}
	}

#endif
	return true;
}

//��ͳ�ƽ���洢����
bool CDbSaveTask::checkDayStaProcedure(hUInt32 year)
{
	//ִ�д洢����
	CString tableName = CString(SCD_DAY_STA_TABLE_PREFIX) + CString().from(year);
	char sql[256] = "";
	ACE_OS::sprintf(sql,"execute %s \'%s\',%d",SCD_DAY_STA_PROCEDURE,tableName.c_str(),CONFIG::instance()->saveMode());

	if ( m_pRdbOp->exec(  sql )== -1 ) 
	{
		logprint(LOG_SCD_DBSAVE_TASK_ERR,"ִ�д洢����<%s> error",sql);
		return false;
	}
	return true;
}

//�������м����
bool CDbSaveTask::checkBatteryProcedure(hUInt32 year,hUInt32 month)
{
	if ( month < 1 || month > 12) return false;

	char monthStr[8] = "";
	ACE_OS::sprintf(monthStr,"%02d",month);

	//ִ�д洢����
	CString tableName = CString(SCD_BAT_RUN_TB_PREFIX) + CString().from(year)+ CString(monthStr);
	CString sql = CString( "execute ") + CString(SCD_BAT_RUN_PROCEDURE) + CString(" ") + CString("'") + tableName + CString("'");
	if ( m_pRdbOp->exec(  sql.c_str() )== -1 ) 
	{
		logprint(LOG_SCD_DBSAVE_TASK_ERR,"ִ�����ش洢����<%s> error",sql.c_str());
		return false;
	}
	return true;
}

//������ݿ�����״̬.return true--���ݿ���������
bool CDbSaveTask::checkDb()
{
	if ( m_pRdbOp != NULL && m_pSql != NULL ) return true;

	if ( m_pRdbOp == NULL )
	{
		m_pRdbOp = new CRdbOp(CRdbOp::PingDirect);
	}

	if ( m_pRdbOp != NULL )
	{
		if ( m_pSql == NULL ) m_pSql = m_pRdbOp->createSQL();
		if ( m_pSql == NULL ) logprint(LOG_SCD_DBSAVE_TASK,"<CDbSaveTask> pRdbOp->createSQL() failed");
	}

	if ( m_pRdbOp != NULL && m_pSql != NULL ) return true;

	logprint(LOG_SCD_DBSAVE_TASK,"CDbSaveTask::checkDb() : error!");
	return false;
}


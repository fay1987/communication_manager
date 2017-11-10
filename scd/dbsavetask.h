#ifndef _PDT_SCD_DBSAVE_TASK_H_
#define _PDT_SCD_DBSAVE_TASK_H_

#include "utl/intervaltimerset.h"
#include "ace/Task.h"
#include "ace/Thread_Manager.h"
#include "rdbop/rdbop.h"
#include "filemanager.h"
#include "dac/comminf.h"
#include "dac/datainf.h"
#include "scddef.h"

#define HIGH_WATER_MARK		(100*1024*1024)		//100M
#define QUEUE_BLOCK_TIME			2			//队列阻塞时间(秒)

class CDbSaveTask : public ACE_Task<ACE_MT_SYNCH>
{
public:
	CDbSaveTask();
	virtual ~CDbSaveTask();
public:
	bool		init();
	int			start();
	void		stop();
	int			svc();
	virtual int put (ACE_Message_Block *pMsgBlock, ACE_Time_Value *pTimeout = 0);
	bool		isEmpty(){return this->msg_queue()->is_empty();}
	bool		isFull(){return this->msg_queue()->is_full();}
protected:
	bool		saveDb(ACE_Message_Block* pMsgBlock);
	bool		saveHisYc(const PDT::SCD_DATA_YC* pData);
	bool		saveHisDaySta(const PDT::SCD_DATA_STA* pData);
	bool		saveHisMonthSta(const PDT::SCD_DATA_STA* pData);
	bool		saveHisSeasonSta(const PDT::SCD_DATA_STA* pData);
	bool		saveHisYearSta(const PDT::SCD_DATA_STA* pData);
	bool		saveHisEvent(const PDT::SCD_DATA_EVENT* pData);
	bool		saveHisBatteryRun(const PDT::SCD_DATA_STA* pData);
	bool		saveHisBatteryCharge(const PDT::SCD_DATA_STA* pData);
	bool		saveHisBatteryDynamic(const PDT::SCD_DATA_STA* pData);
	bool		saveHisBatteryState(const PDT::SCD_DATA_STA* pData);
	bool        saveHisRSYc(const HIS_STA_RS_DATA* pData);          //2017/1/18   by hll 电站区域数据统计函数
	bool        saveHisRSDayYcSta(const HIS_STA_RS_DATA* pData);    //2017/1/18   by hll 电站区域日数据统计函数
	bool        saveHisDayCal(const HIS_STA_RS_DATA* pData);        //2017/02/10  by hll 电站区域功率最值 区域峰谷值函数myc
	bool        saveHisMonthCal(const HIS_STA_RS_DATA* pData);      //2017/02/10  by hll 电站区域功率最值yyc
	bool		checkProcedure();
	bool		checkYcProcedure(hUInt32 year,hUInt32 month);		//检查并创建存储过程
	bool		checkDayStaProcedure(hUInt32 year);					//检查并创建存储过程
	bool		checkBatteryProcedure(hUInt32 year,hUInt32 month);	//检查并创建存储过程

	bool		checkDb();											//wfp add at 20111102
private:
	void		saveLeftDataToFile();								//将剩余历史数据存文件
private:
	PDT::CIntervalTimerSet	m_timer;
	int						m_timeId;
	PDT::CRdbOp*			m_pRdbOp;
	PDT::CSql*				m_pSql;
	hUInt32					m_execErrCount;		//执行错误计数	
	CFileManager			m_hisDataFile;
	CFileManager			m_errDataFile;
	PDT::CCommInf			m_commInf;
	PDT::CDataInf			m_dataInf;
};

typedef ACE_Singleton<CDbSaveTask, ACE_Null_Mutex> TASK_DBSAVE;

#endif	//_PDT_SCD_DBSAVE_TASK_H_

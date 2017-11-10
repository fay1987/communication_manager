#ifndef _PDT_SCD_HISDATA_TASK_H_
#define _PDT_SCD_HISDATA_TASK_H_

#include "utl/intervaltimerset.h"
#include "ace/Task.h"
#include "ace/Thread_Manager.h"
#include "utl/datetime.h"
#include "dac/comminf.h"
#include "dac/datainf.h"
#include "scd/hisdatainf.h"
#include "scd/realdatainf.h"

class CHisDataTask : public ACE_Task<ACE_MT_SYNCH>
{
public:
	CHisDataTask();
	virtual ~CHisDataTask();
public:
	bool	init();
	int		start();
	void	stop();
	int		svc();
protected:
	void	checkTimeInterval();			//检查时间隔是否到期
	void	procIntervalData();				//间隔存盘数据处理
	void	procWaterData();				//阀值存盘数据处理
	void	procStaData();					//历史统计存盘数据处理
	void	procBatteryChargeData();		//静态放电+充电数据
	void	procBatteryChargeData(int routeNo,int batState,hUInt32 saveTime);	//静态放电+充电数据
	void	procBatteryYx();				//蓄电池历史状态

	//将数据put到dbsavetask::messagequeue
	bool	putYcData(hInt32 groupNo,hInt32 no,hUInt32 dataType,DAC_YC_DATA& val);
	bool	putYcData(hInt32 groupNo,hInt32 no,hUInt32 dataType,hUInt32 ycdataType,DAC_YC_DATA& val);
	bool	putBatteryYcData(hInt32 groupNo,hInt32 no,hUInt32 dataType,DAC_YC_DATA& val);
	bool	putBatteryRunData(hInt32 groupNo,hInt32 no,hUInt32 dataType,DAC_YC_DATA& val,int batState);
	bool	putBatteryChargeData(hInt32 groupNo,hInt32 no,hUInt32 dataType,DAC_YC_DATA& val,int batState);
	bool	putBatteryStateData(hInt32 groupNo,DAC_YX_CHG_DATA& chgVal);
	bool	putStaData(hInt32 groupNo,HIS_STA_DATA& val);
	bool	putStaRsData(HIS_STA_RS_DATA& rsval);

	//根据值类型获取存盘类型
	int		saveType(hUInt32 valType) const;
	//根据质量吗获取存盘数据类型
	hUInt8	dataTypeFromQuality(const DAC_YC_DATA& data,DAC_YC* pPara) const;
	int		batteryState(int groupNo);
	bool	isMaintainPoint(int batteryNum,int pointNo) const;
private:
	PDT::CIntervalTimerSet	m_timer;		//间隔定时器
	int						m_timeId;		//间隔定时器ID
	//int						m_staticTimeId;	//静态放电timeid
	PDT::CDateTime			m_lastTime;		//上一次的时间
	hUInt32					m_timeMask;		//间隔时间码值

	PDT::CCommInf			m_commInf;		
	PDT::CDataInf			m_dataInf;
	PDT::CHisStaInf			m_hisStaInf;
	CRealDataInf			m_readDataInf;	//实时库接口
};

typedef ACE_Singleton<CHisDataTask, ACE_Null_Mutex> TASK_HISDATA;

#endif	//_PDT_SCD_HISDATA_TASK_H_

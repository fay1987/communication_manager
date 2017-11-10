#ifndef	_PDT_SCD_CALCTASK_H_
#define	_PDT_SCD_CALCTASK_H_

#include "dac/comminf.h"
#include "dac/datainf.h"
#include <ace/Time_Value.h>
#include <ace/Task.h>
#include <ace/Thread_Mutex.h>
#include <utl/variant.h>
#include "utl/string.h"
#include <ace/Vector_T.h>
#include "utl/intervaltimerset.h"
#include "scd/exp_analyze.h"
#include "scd/zjualg.h"

typedef struct _calc_data_
{
	int		routeNo;
	int		pointNo;
}CALC_DATA;

typedef struct _kwh_statics_data_
{
	float   dayinitval;		//日初值
	float	moninitval;		//月初值
	float	yearinitval;	//年初值
	int		type;			//1.日电量，2.月电量，3.年电量
	int		routeNo;		//总累计电量路径
	int		pointNo;		//总累计电量点号
	int		init;
}KWH_STATICS_DATA;

typedef ACE_Vector<CALC_DATA>		CALC_VECTOR;
typedef ACE_Vector<KWH_STATICS_DATA>	KWH_STATICS_VECTOR;

class CCalcTask : public ACE_Task<ACE_NULL_SYNCH>
{
public:
	CCalcTask();
	virtual ~CCalcTask();
	bool	init();
	int		start();
	void	stop();

	//主处理过程
	hInt32	svc();
protected:
	bool	initYcCalcData();
	bool	initYxCalcData();
	bool	initKwhCalcData();
	bool	calcYc();
	bool	calcYx();
	bool	calcKwh(int type);//type=1日刷新；2月刷新；3年刷新
	void	calcYxSgz(int routeNo,int pointNo);		//事故总计算量处理
	void	calcYxComState(int routeNo,int pointNo);//遥信的通讯状态计算量处理
	void	calcYxBkState(int routeNo,int pointNo);//通过电压计算开关状态
	bool	isYcLimit(int routeNo);					//是否有遥测越限
	int		floatToInt(hFloat val);
private:
	PDT::CCommInf			m_commInf;
	PDT::CDataInf			m_dataInf;
	CALC_VECTOR				m_ycCalcVector,m_yxCalcVector;
	KWH_STATICS_VECTOR		m_kwhCalcVector;

	PDT::CIntervalTimerSet	m_timer;
	int						m_timeId;

	PDT::CExpAnalyze		m_expAnalyzer;
	bool					m_isInited;

	/*PDT::CComputePLB		m_cplb;*/
};

typedef	ACE_Singleton<CCalcTask,ACE_Null_Mutex>	TASK_CALC;

#endif //_PDT_SCD_CALCTASK_H_

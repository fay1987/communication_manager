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
	float   dayinitval;		//�ճ�ֵ
	float	moninitval;		//�³�ֵ
	float	yearinitval;	//���ֵ
	int		type;			//1.�յ�����2.�µ�����3.�����
	int		routeNo;		//���ۼƵ���·��
	int		pointNo;		//���ۼƵ������
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

	//���������
	hInt32	svc();
protected:
	bool	initYcCalcData();
	bool	initYxCalcData();
	bool	initKwhCalcData();
	bool	calcYc();
	bool	calcYx();
	bool	calcKwh(int type);//type=1��ˢ�£�2��ˢ�£�3��ˢ��
	void	calcYxSgz(int routeNo,int pointNo);		//�¹��ܼ���������
	void	calcYxComState(int routeNo,int pointNo);//ң�ŵ�ͨѶ״̬����������
	void	calcYxBkState(int routeNo,int pointNo);//ͨ����ѹ���㿪��״̬
	bool	isYcLimit(int routeNo);					//�Ƿ���ң��Խ��
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

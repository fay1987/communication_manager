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
	void	checkTimeInterval();			//���ʱ����Ƿ���
	void	procIntervalData();				//����������ݴ���
	void	procWaterData();				//��ֵ�������ݴ���
	void	procStaData();					//��ʷͳ�ƴ������ݴ���
	void	procBatteryChargeData();		//��̬�ŵ�+�������
	void	procBatteryChargeData(int routeNo,int batState,hUInt32 saveTime);	//��̬�ŵ�+�������
	void	procBatteryYx();				//������ʷ״̬

	//������put��dbsavetask::messagequeue
	bool	putYcData(hInt32 groupNo,hInt32 no,hUInt32 dataType,DAC_YC_DATA& val);
	bool	putYcData(hInt32 groupNo,hInt32 no,hUInt32 dataType,hUInt32 ycdataType,DAC_YC_DATA& val);
	bool	putBatteryYcData(hInt32 groupNo,hInt32 no,hUInt32 dataType,DAC_YC_DATA& val);
	bool	putBatteryRunData(hInt32 groupNo,hInt32 no,hUInt32 dataType,DAC_YC_DATA& val,int batState);
	bool	putBatteryChargeData(hInt32 groupNo,hInt32 no,hUInt32 dataType,DAC_YC_DATA& val,int batState);
	bool	putBatteryStateData(hInt32 groupNo,DAC_YX_CHG_DATA& chgVal);
	bool	putStaData(hInt32 groupNo,HIS_STA_DATA& val);
	bool	putStaRsData(HIS_STA_RS_DATA& rsval);

	//����ֵ���ͻ�ȡ��������
	int		saveType(hUInt32 valType) const;
	//�����������ȡ������������
	hUInt8	dataTypeFromQuality(const DAC_YC_DATA& data,DAC_YC* pPara) const;
	int		batteryState(int groupNo);
	bool	isMaintainPoint(int batteryNum,int pointNo) const;
private:
	PDT::CIntervalTimerSet	m_timer;		//�����ʱ��
	int						m_timeId;		//�����ʱ��ID
	//int						m_staticTimeId;	//��̬�ŵ�timeid
	PDT::CDateTime			m_lastTime;		//��һ�ε�ʱ��
	hUInt32					m_timeMask;		//���ʱ����ֵ

	PDT::CCommInf			m_commInf;		
	PDT::CDataInf			m_dataInf;
	PDT::CHisStaInf			m_hisStaInf;
	CRealDataInf			m_readDataInf;	//ʵʱ��ӿ�
};

typedef ACE_Singleton<CHisDataTask, ACE_Null_Mutex> TASK_HISDATA;

#endif	//_PDT_SCD_HISDATA_TASK_H_

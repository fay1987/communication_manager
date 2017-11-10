#ifndef	_PDT_SCD_LOADTASK_H_
#define	_PDT_SCD_LOADTASK_H_

#include "dac/comminf.h"
#include <ace/Time_Value.h>
#include <ace/Task.h>
#include <ace/Thread_Mutex.h>
#include <utl/variant.h>
#include "utl/string.h"
#include <map>

//���㹫ʽ����
enum SCD_FORMULA_TYPE
{
	FORMULA_SYSTEM					=	0,	//ϵͳ����
	FORMULA_USER					=	1	//�û��Զ���
};

typedef struct _scd_formula_data_
{
	PDT::CString	name;
	hUInt32			type;
	PDT::CString	content;
}SCD_FORMULA_DATA;

typedef struct _scd_beginkwh_data_
{
	hFloat			dayvalue;
	hFloat			monthvalue;
	hFloat			yearvalue;
}SCD_BEGINKWH_DATA;

typedef std::map<PDT::CString,SCD_FORMULA_DATA>	FORMULA_MAP;
typedef std::map<PDT::CString,SCD_BEGINKWH_DATA>	BEGINKWH_MAP;

class CLoadTask : public ACE_Task<ACE_NULL_SYNCH>
{
public:
	CLoadTask();
	virtual ~CLoadTask();
	bool	init();
	int		start();
	void	stop();

	//���������
	hInt32	svc();
public:
	bool	isBatteryValid(hInt32 group);
	int		batteryGroupNum();

	bool	formula(const char* code,SCD_FORMULA_DATA& data);	//��ȡ��ʽ����
	bool	beginkwh(const char* code,SCD_BEGINKWH_DATA& data);	//��ȡ��ʼ����ֵ
	bool	setBeginkwh(const char* code,SCD_BEGINKWH_DATA data);	//��д��ʼ����ֵ
	bool	updateBeginKwh();
protected:
	bool	loadDevice();
	bool	loadFormula();
	bool	loadBeginKwh();			//add by wlq 20161101 ��ȡͳ�Ƴ�ʼ����
private:
	PDT::CCommInf	m_commInf;
	bool			m_isBatteryDevice[DAC_GROUP_NUM];			//�Ƿ�Ϊ�����豸
	FORMULA_MAP		m_formulaMap;
	BEGINKWH_MAP	m_beginkwhMap;
};

typedef	ACE_Singleton<CLoadTask,ACE_Null_Mutex>	TASK_LOAD;

#endif //_PDT_SCD_LOADTASK_H_

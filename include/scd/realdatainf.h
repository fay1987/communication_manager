#ifndef _PDT_SCD_REALDATA_H_
#define _PDT_SCD_REALDATA_H_

#include <ace/config-all.h>
#include "utl/sharememorybase.h"
#include "dac/dacmacro.h"

//=============================================================================
#if defined (_REALDATAINF_)
#  define	REALDATA_EXPORT	ACE_Proper_Export_Flag
#else
#  define	REALDATA_EXPORT	ACE_Proper_Import_Flag
#endif	// _REALDATAINF_
//=============================================================================


#define SCD_REAL_SHM_KEY		2010072201	//ʵʱ�⹲���ڴ��ʶ
#define SCD_REAL_YC_NUM			1024		//ʵʱ�����ң�����


typedef struct _scd_real_yc_
{
	nFloat		lastSaveVal;				//�ϴδ���ֵ
	nBool		valid;						//�ϴδδ���ֵ��Ч��־(����ʱ��Ч)
}SCD_REAL_YC;

/*
//��ͳ��--wfp add at 20110916
typedef struct _scd_real_day_yc_
{
	nFloat		maxVal;						//�������ֵ
	nUInt32		maxTime;					//�������ֵ����ʱ��
	nFloat		minVal;						//������Сֵ
	nUInt32		minTime;					//������Сֵ����ʱ��
	nFloat		sumVal;						//����5���Ӽ���ۼ�ֵ
	nFloat		avgVal;						//����ƽ��ֵ--SCADAһ�㲻��
	nUInt32		avgTime;					//����ƽ��ֵ����ʱ��
	nBool		valid;						//��Ч��־(����ʱ��Ч)
}SCD_REAL_DAY_YC;
*/

typedef struct _scd_real_mem_
{
	SCD_REAL_YC		realData[DAC_ROUTE_NUM][SCD_REAL_YC_NUM];
	//SCD_REAL_DAY_YC	dayData[DAC_ROUTE_NUM][SCD_REAL_YC_NUM];
}SCD_REAL_MEM;

class REALDATA_EXPORT CRealDataInf : public CShareMemoryBase
{
public:
	CRealDataInf();
	virtual ~CRealDataInf();
public:
	//��ֵ����
	SCD_REAL_YC*			ycData(hInt32 route,hInt32 no=0);
	const SCD_REAL_YC*		ycData(hInt32 route,hInt32 no=0) const;

	bool					yc(hInt32 route,hInt32 no,SCD_REAL_YC &data)	const;
	bool					setYc(hInt32 route,hInt32 no,const SCD_REAL_YC& data);

	/*
	//��ͳ��
	SCD_REAL_DAY_YC*		dayData(hInt32 route,hInt32 no=0);
	const SCD_REAL_DAY_YC*  dayData(hInt32 route,hInt32 no=0) const;

	bool					dayData(hInt32 route,hInt32 no,SCD_REAL_DAY_YC& data) const;
	bool					setDayData(hInt32 route,hInt32 no,const SCD_REAL_DAY_YC& data);
	*/
private:
	static	SCD_REAL_MEM	*m_ptr;
};

#endif	//_PDT_SCD_REALDATA_H_

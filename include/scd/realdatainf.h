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


#define SCD_REAL_SHM_KEY		2010072201	//实时库共享内存标识
#define SCD_REAL_YC_NUM			1024		//实时库最大遥测个数


typedef struct _scd_real_yc_
{
	nFloat		lastSaveVal;				//上次存盘值
	nBool		valid;						//上次次存盘值有效标志(启动时无效)
}SCD_REAL_YC;

/*
//日统计--wfp add at 20110916
typedef struct _scd_real_day_yc_
{
	nFloat		maxVal;						//当日最大值
	nUInt32		maxTime;					//当日最大值发生时刻
	nFloat		minVal;						//当日最小值
	nUInt32		minTime;					//当日最小值发生时刻
	nFloat		sumVal;						//当日5分钟间隔累加值
	nFloat		avgVal;						//当日平均值--SCADA一般不算
	nUInt32		avgTime;					//当日平均值发生时刻
	nBool		valid;						//有效标志(启动时无效)
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
	//阀值存盘
	SCD_REAL_YC*			ycData(hInt32 route,hInt32 no=0);
	const SCD_REAL_YC*		ycData(hInt32 route,hInt32 no=0) const;

	bool					yc(hInt32 route,hInt32 no,SCD_REAL_YC &data)	const;
	bool					setYc(hInt32 route,hInt32 no,const SCD_REAL_YC& data);

	/*
	//日统计
	SCD_REAL_DAY_YC*		dayData(hInt32 route,hInt32 no=0);
	const SCD_REAL_DAY_YC*  dayData(hInt32 route,hInt32 no=0) const;

	bool					dayData(hInt32 route,hInt32 no,SCD_REAL_DAY_YC& data) const;
	bool					setDayData(hInt32 route,hInt32 no,const SCD_REAL_DAY_YC& data);
	*/
private:
	static	SCD_REAL_MEM	*m_ptr;
};

#endif	//_PDT_SCD_REALDATA_H_

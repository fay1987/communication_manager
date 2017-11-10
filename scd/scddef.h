/*==============================================================================
* 文件名称 : scddef.h
* 文件功能 ：scada各种类型定义
* 模块 : scada
* 创建时间 : 2009-08-08
* 作者 : wfp
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_SCD_SCDDEF_H_
#define	_PDT_SCD_SCDDEF_H_

#include "dac/dac.h"
#include "scd/hisdatadef.h"
#include "event/csea.h"

namespace	PDT
{
	typedef	struct _scd_data_head_
	{
		nUInt32			dataclass;			//存盘数据类型(用于区分表，参考SCD_DATA_TYPE枚举)
	}SCD_DATA_HEAD;

	typedef struct _scd_data_yc_
	{
		nInt32			group;				//组号
		nInt32			no;					//序号
		nUInt32			dataType;			//历史数据类型1为有效2为无效
		nUInt32			ycdataType;			//遥测数据类型 1-30为电力指标 31-34电量指标 35-40谐波指标 41-51其他指标
		DAC_YC_DATA		data;
	}SCD_DATA_YC;

	//统计数据,包括日月季统计
	typedef struct _scd_data_sta_
	{
		nInt32			group;				//组号
		HIS_STA_DATA	data;
	}SCD_DATA_STA;

	typedef struct _scd_data_event_
	{
		nInt32			evtClass;				//分类号
		nInt32			evtType;				//事项类型
		nUInt32			occurTime;				//时间
		char			staCode[DAC_NAME_LEN];	//厂站编码
		char			staName[DAC_NAME_LEN];	//厂站名称
		char			parentCode[DAC_NAME_LEN];//父编码
		char			parentName[DAC_NAME_LEN];//父名称
		char			code[DAC_NAME_LEN];		//编码
		char			name[DAC_NAME_LEN];		//名称
		char			user[16];				//用户名
		nUInt8			level;					//等级
		nUInt8			stateValue;				//当前状态值
		nUInt8			stateValue1;			//状态值1
		nFloat			floatValue;				//当前浮点值
		nFloat			floatValue1;			//浮点值1
		nUInt8			alarmFlag;				//告警标志
		char			info[FULLDESCRIBE_LEN];	//文字描述
	}SCD_DATA_EVENT;

	//存盘间隔类型
	enum SCD_SAVE_INTERVAL
	{
		INTERVAL_1_MIN					=	0x00000001,	//1分钟存盘点
		INTERVAL_5_MIN					=	0x00000002,	//5分钟存盘点
		INTERVAL_10_MIN					=	0x00000004,	//10分钟存盘点
		INTERVAL_15_MIN					=	0x00000008,	//15分钟存盘点
		INTERVAL_30_MIN					=	0x00000010,	//30分钟存盘点
		INTERVAL_HOUR					=	0x00000020,	//时采集点
		INTERVAL_DAY					=	0x00000040,	//日采集点
		INTERVAL_MONTH					=	0x00000080,	//月采集点
		INTERVAL_ALL					=	0xFFFFFFFF	//适应所有间隔
	};

	//YC越限事项类型
	enum SCD_YC_OVERLIMIT_TYPE
	{
		YC_OVLIMIT_NORMAL				=	0,			//正常
		YC_OVLIMIT_HIGH					=	1,			//越上限
		YC_OVLIMIT_LOW					=	2,			//越下限
		YC_OVLIMIT_HIHI					=	3,			//越上上限
		YC_OVLIMIT_LOLO					=	4			//越下下限
	};

	//蓄电池状态
	enum BATTERY_STATE_TYPE
	{
		BAT_STATE_EQUAL					=		0,		//均充
		BAT_STATE_DRIFT					=		1,		//浮充(正常)
		BAT_STATE_DRIFT_Q				=		2,		//浮充(欠充) 
		BAT_STATE_DRIFT_G				=		3,		//浮充(过充)
		BAT_STATE_DYNAMIC				=		4,		//动态放电开始
		//BAT_STATE_DYNAMIC_END			=		5,		//动态放电结束
		BAT_STATE_DISCHARGE				=		6,		//放电(核对性放电/静态放电)
		//BAT_STATE_DISCHARGE_END		=		7,		//放电结束(核对性放电/静态放电)
		BAT_STATE_BREAK					=		8,		//开路 
		BAT_STATE_OTHER 				=		250		//其他 
	};
	//单体电池状态
	enum BATTERY_SINGLE_STATE
	{
		BAT_SINGLE_NORMAL	=	0,	//单体电池正常状态
		BAT_SINGLE_WH		=	1	//单体电池维护状态
	};

	//日志类型
	enum SCD_LOG_TYPE
	{
		LOG_SCD_BASE					=	2100,		//scd base
		LOG_SCD_HISDATA_TASK			=	2101,		//hisdata task
		LOG_SCD_CALC_TASK				=	2105,		//calc task
		LOG_SCD_LIMIT_TASK				=	2110,		//limit task
		LOG_SCD_EVENT_TASK				=	2120,		//event task
		LOG_SCD_FILEREAD_TASK			=	2130,		//file read task
		LOG_SCD_DBSAVE_TASK				=	2140,		//dbsave task
		LOG_SCD_DBSAVE_TASK_ERR			=	2141,		//dbsave task save error
		LOG_SCD_DBPROCEDURE_TASK		=	2145,		//dbprocedure task(统计存储过程)
		LOG_SCD_FILEREAD				=	2150,		//file read
		LOG_SCD_FILEWRITE				=	2151		//file write
	};

}

#endif //_PDT_SCD_SCDDEF_H_

/*==============================================================================
* 文件名称 : hisdatainf.h
* 文件功能 ：定义历史数据/统计操作接口
* 模块 : 历史统计接口
* 创建时间 : 2009-06-08
* 作者 : wfp
* 版本 : v1.0

* 一次修改后版本 : 1.1
* 修改摘要 : 对函数HIS_DATA_CLASS、HIS_DATA_TYPE新增内容
* 修改者 :  hll
* 修改日期 : 2017-01-18 09:30:12

* 二次修改后版本 : 1.2
* 修改摘要 : 对函数HIS_DATA_CLASS新增内容
* 修改者 :  hll
* 修改日期 : 2017-02-10 15:32:16
==============================================================================*/
#ifndef	_PDT_SCD_HISDATA_DEF_H_
#define	_PDT_SCD_HISDATA_DEF_H_

#include "utl/datatype.h"
#include "dac/dacmacro.h"

#define HIS_STA_NUM			512								//每个组最大统计个数

//历史统计+蓄电池
typedef struct _his_sta_data_
{
	nInt32		no;			//序号
	nUInt32		dataclass;	//历史数据等级
	nUInt32		type;		//历史统计类型
	nFloat		val;		//统计值
	nUInt32		valstate;	//值状态(蓄电池等使用)/yx值
	nUInt32		occurTime;	//当前值发生时刻，使用于最值发生时刻
	nUInt32		saveTime;	//存盘时间
}HIS_STA_DATA;

//历史统计（区域和厂站统计）
typedef struct _his_sta_rs_data_
{
	char		rsname[32];			//区域和厂站名称
	char		code[32];			//统计值代码
	nUInt32		dataclass;	//历史数据等级
	nUInt32		type;		//历史统计类型
	nFloat		val;		//统计值
	nUInt32		valstate;	//值状态(蓄电池等使用)/yx值
	nUInt32		occurTime;	//当前值发生时刻，使用于最值发生时刻
	nUInt32		saveTime;	//存盘时间
}HIS_STA_RS_DATA;

//历史统计
typedef struct _his_sta_info_
{
	HIS_STA_DATA	data[HIS_STA_NUM];						//统计值
	hUInt32			write;									//写位置
}HIS_STA_INFO;

//历史统计共享内存
typedef	struct _his_sta_shm_
{
	HIS_STA_INFO	info[DAC_GROUP_NUM];	
	HIS_STA_RS_DATA	rsdata[HIS_STA_NUM*10];
	hUInt32			rswrite;
}HIS_STA_SHM;

//数据类型级别
enum HIS_DATA_CLASS
{
	HIS_CLASS_YC					=	1,		//遥测历史数据类型
	HIS_CLASS_EVENT					=	2,		//历史事项
	HIS_CLASS_DAY_STA				=	3,		//日统计
	HIS_CLASS_MONTH_STA				=	4,		//月统计
	HIS_CLASS_SEASON_STA			=	5,		//季统计
	HIS_CLASS_YEAR_STA				=	6,		//年统计
	HIS_CLASS_RUN					=	7,		//蓄电池运行监测
	HIS_CLASS_CHARGE				=	8,		//充放电表内数据
	HIS_CLASS_DYNAMIC				=	9,		//动态放电表内数据
	HIS_CLASS_STATE					=	10,		//历史状态表
	HIS_CLASS_PEFORM_RESULT			=	11,		//性能结果表
	HIS_CLASS_PEFORM_TMP_RESULT		=	12,		//性能分析中间结果表
	HIS_CLASS_RSYC                  =   13,     //电站区域统计数据类型    2017-01-18 09:30:12  by hll
	HIS_CLASS_RSDAY_STA             =   14,     //电站区域日统计数据类型  2017-01-18 09:30:12  by hll
	HIS_CLASS_DAY_CAL               =   15,     //区域峰、谷、尖、平及功率最大、最小、平均值存盘 2017/02/10  by hll 
	HIS_CLASS_MONTH_CAL             =   16      //电站区域功率最大、最小、平均值存盘 2017/02/10  by hll 
	
};

//历史统计类型
enum HIS_DATA_TYPE
{
	//遥测历史数据表定时存盘
	HIS_TYPE_REAL					=	1,		//有效实时值
	HIS_TYPE_INVALID_REAL			=	2,		//无效实时值
	HIS_TYPE_LOCAL_MANSET			=	3,		//本地人工置入
	HIS_TYPE_REMOTE_MANSET			=	4,		//远程人工置入
	HIS_TYPE_DEAD					=	5,		//死数(无效)
	HIS_TYPE_NET_ERROR				=	6,		//网络故障
	HIS_TYPE_HIGH					=	7,		//遥测越上限
	HIS_TYPE_LOW					=	8,		//遥测越下限
	HIS_TYPE_VAL_NOQUAHIGH			=	9,		//电压越不合格上限
	HIS_TYPE_VAL_NOQUALOW			=	10,		//电压越不合格下限
	HIS_TYPE_MAN_MODIFY				=	11,		//人工修改
	HIS_TYPE_MAN_INSERT				=	12,		//人工插入
	//遥测历史数据表整点值
	HIS_TYPE_HOUR_VAL				=	21,		//整点值
	HIS_TYPE_XB_HOUR_VAL			=	22,		//谐波整点值
	//遥测历史数据表计划值
	HIS_TYPE_PLAN					=	31,		//计划值
	//遥测历史数据表统计值
	HIS_TYPE_STA_AVGVAL				=	41,		//平均值
	HIS_TYPE_STA_MAXVAL				=	42,		//最大值
	HIS_TYPE_STA_MAXTIME			=	43,		//最大时刻
	HIS_TYPE_STA_MINVAL				=	44,		//最小值
	HIS_TYPE_STA_MINTIME			=	45,		//最小时刻
	HIS_TYPE_STA_TOTALTIME			=	46,		//日统计时间
	HIS_TYPE_STA_QUARATE			=	47,		//合格率（平电量）
	HIS_TYPE_STA_QUATIME			=	48,		//合格时间
	HIS_TYPE_STA_HIGHRATE			=	49,		//超上限率(峰电量)
	HIS_TYPE_STA_HIGHNUM			=	50,		//超上限次数
	HIS_TYPE_STA_HIGHTIME			=	51,		//超上限时间
	HIS_TYPE_STA_HIHIRATE			=	52,		//超上上限率
	HIS_TYPE_STA_HIHINUM			=	53,		//超上上限次数
	HIS_TYPE_STA_HIHITIME			=	54,		//超上上限时间
	HIS_TYPE_STA_LOWRATE			=	55,		//超下限率（谷电量）
	HIS_TYPE_STA_LOWNUM				=	56,		//超下限次数
	HIS_TYPE_STA_LOWTIME			=	57,		//超下限时间
	HIS_TYPE_STA_LOLOWRATE			=	58,		//超下下限率
	HIS_TYPE_STA_LOLOWNUM			=	59,		//超上下限次数
	HIS_TYPE_STA_LOLOWTIME			=	60,		//超下下限时间
	HIS_TYPE_STA_POWERCUTTIME		=	61,		//日停运时间(分钟)
	HIS_TYPE_STA_LOADRATE			=	62,		//负荷率
	HIS_TYPE_STA_DEADTIME			=	63,		//日死数时间

	//存盘 (2017/1/18 11:14  by hll  统计存盘值)
	HIS_TYPE_VALLEYVAL              =   70, //谷    105、106、107
	HIS_TYPE_PEAKVAL                =   71, //峰
	HIS_TYPE_SHARPVAL               =   72, //尖
	HIS_TYPE_FLATVAL                =   73, //平
	HIS_TYPE_DAY_POWER              =   74,  //日表电量类型  177
	HIS_TYPE_MONTH_POWER            =   75,  //月表电量类型  178
	HIS_TYPE_YEAR_POWER             =   76,  //年表电量类型  179
	HIS_TYPE_RSHOUR_VAL             =   77,  //统计日整点值  




	//日统计表
	HIS_TYPE_DAY_START				=	100,	//日统计起始类型号
	HIS_TYPE_DAY_AVGVAL				=	101,	//日平均值
	HIS_TYPE_DAY_MAXVAL				=	102,	//日最大值，最大值发生时刻
	HIS_TYPE_DAY_MINVAL				=	103,	//日最小值，最小值发生时刻
	HIS_TYPE_DAY_TOTALTIME			=	104,	//日统计时间
	HIS_TYPE_DAY_QUARATE			=	105,	//日合格率（日平电量）
	HIS_TYPE_DAY_QUATIME			=	106,	//日合格时间
	HIS_TYPE_DAY_HIGHRATE			=	107,	//日超上限率（日峰电量）
	HIS_TYPE_DAY_HIGHNUM			=	108,	//日超上限次数
	HIS_TYPE_DAY_HIGHTIME			=	109,	//日超上限时间
	HIS_TYPE_DAY_HIHIRATE			=	110,	//日超上上限率
	HIS_TYPE_DAY_HIHINUM			=	111,	//日超上上限次数
	HIS_TYPE_DAY_HIHITIME			=	112,	//日超上上限时间
	HIS_TYPE_DAY_LOWRATE			=	113,	//日超下限率（日谷电量）
	HIS_TYPE_DAY_LOWNUM				=	114,	//日超下限次数
	HIS_TYPE_DAY_LOWTIME			=	115,	//日超下限时间
	HIS_TYPE_DAY_LOLOWRATE			=	116,	//日超下下限率
	HIS_TYPE_DAY_LOLOWNUM			=	117,	//日超上下限次数
	HIS_TYPE_DAY_LOLOWTIME			=	118,	//日超下下限时间
	HIS_TYPE_DAY_POWERCUTTIME		=	119,	//日停电时间(分钟)
	HIS_TYPE_DAY_LOADRATE			=	120,	//日负荷率
	HIS_TYPE_DAY_XB_AVGVAL			=	121,	//日谐波平均值
	HIS_TYPE_DAY_XB_MAXVAL			=	122,	//日谐波最大值
	HIS_TYPE_DAY_XB_QUARATE			=	123,	//日谐波合格率
	HIS_TYPE_DAY_XB_HIGHTIME		=	124,	//日谐波超上限时间
	HIS_TYPE_DAY_END				=	129,	//日统计的结束类型号
	//月统计值
	HIS_TYPE_MONTH_START			=	130,	//月统计起始类型号
	HIS_TYPE_MONTH_AVGVAL			=	131,	//月平均值
	HIS_TYPE_MONTH_MAXVAL			=	132,	//月最大值，最大值发生时刻
	HIS_TYPE_MONTH_MINVAL			=	133,	//月最小值，最小值发生时刻
	HIS_TYPE_MONTH_TOTALTIME		=	134,	//月统计时间
	HIS_TYPE_MONTH_QUARATE			=	135,	//月合格率（月平电量）
	HIS_TYPE_MONTH_QUATIME			=	136,	//月合格时间
	HIS_TYPE_MONTH_HIGHRATE			=	137,	//月超上限率（月峰电量）
	HIS_TYPE_MONTH_HIGHNUM			=	138,	//月超上限次数
	HIS_TYPE_MONTH_HIGHTIME			=	139,	//月超上限时间
	HIS_TYPE_MONTH_HIHIRATE			=	140,	//月超上上限率
	HIS_TYPE_MONTH_HIHINUM			=	141,	//月超上上限次数
	HIS_TYPE_MONTH_HIHITIME			=	142,	//月超上上限时间
	HIS_TYPE_MONTH_LOWRATE			=	143,	//月超下限率（月谷电量）
	HIS_TYPE_MONTH_LOWNUM			=	144,	//月超下限次数
	HIS_TYPE_MONTH_LOWTIME			=	145,	//月超下限时间
	HIS_TYPE_MONTH_LOLOWRATE		=	146,	//月超下下限率
	HIS_TYPE_MONTH_LOLOWNUM			=	147,	//月超上下限次数
	HIS_TYPE_MONTH_LOLOWTIME		=	148,	//月超下下限时间
	HIS_TYPE_MONTH_POWERCUTTIME		=	149,	//月停电时间(分钟)
	HIS_TYPE_MONTH_LOADRATE			=	150,	//月负荷率
	HIS_TYPE_MONTH_XB_AVGVAL		=	151,	//月谐波平均值
	HIS_TYPE_MONTH_XB_MAXVAL		=	152,	//月谐波最大值
	HIS_TYPE_MONTH_XB_QUARATE		=	153,	//月谐波合格率
	HIS_TYPE_MONTH_XB_HIGHTIME		=	154,	//月谐波超上限时间
	HIS_TYPE_MONTH_END				=	159,	//月统计的结束类型号
	//季统计值
	HIS_TYPE_SEASON_START			=	160,	//季统计起始类型号
	HIS_TYPE_SEASON_AVGVAL			=	161,	//季平均值
	HIS_TYPE_SEASON_MAXVAL			=	162,	//季最大值，最大值发生时刻
	HIS_TYPE_SEASON_MINVAL			=	163,	//季最小值，最小值发生时刻
	HIS_TYPE_SEASON_TOTALTIME		=	164,	//季统计时间
	HIS_TYPE_SEASON_QUARATE			=	165,	//季合格率（季平电量）
	HIS_TYPE_SEASON_QUATIME			=	166,	//季合格时间
	HIS_TYPE_SEASON_HIGHRATE		=	167,	//季超上限率（季峰电量）
	HIS_TYPE_SEASON_HIGHNUM			=	168,	//季超上限次数
	HIS_TYPE_SEASON_HIGHTIME		=	169,	//季超上限时间
	HIS_TYPE_SEASON_HIHIRATE		=	170,	//季超上上限率
	HIS_TYPE_SEASON_HIHINUM			=	171,	//季超上上限次数
	HIS_TYPE_SEASON_HIHITIME		=	172,	//季超上上限时间
	HIS_TYPE_SEASON_LOWRATE			=	173,	//季超下限率（季谷电量）
	HIS_TYPE_SEASON_LOWNUM			=	174,	//季超下限次数
	HIS_TYPE_SEASON_LOWTIME			=	175,	//季超下限时间
	HIS_TYPE_SEASON_LOLOWRATE		=	176,	//季超下下限率
	HIS_TYPE_SEASON_LOLOWNUM		=	177,	//季超上下限次数
	HIS_TYPE_SEASON_LOLOWTIME		=	178,	//季超下下限时间
	HIS_TYPE_SEASON_POWERCUTTIME	=	179,	//季停电时间(分钟)
	HIS_TYPE_SEASON_LOADRATE		=	180,	//季负荷率
	HIS_TYPE_SEASON_END				=	189,	//季统计的结束类型号
	//年统计值
	HIS_TYPE_YEAR_START				=	190,	//年统计起始类型号
	HIS_TYPE_YEAR_AVGVAL			=	191,	//年平均值
	HIS_TYPE_YEAR_MAXVAL			=	192,	//年最大值，最大值发生时刻
	HIS_TYPE_YEAR_MINVAL			=	193,	//年最小值，最小值发生时刻
	HIS_TYPE_YEAR_TOTALTIME			=	194,	//年统计时间
	HIS_TYPE_YEAR_QUARATE			=	195,	//年合格率（年平电量）
	HIS_TYPE_YEAR_QUATIME			=	196,	//年合格时间
	HIS_TYPE_YEAR_HIGHRATE			=	197,	//年超上限率（年峰电量）
	HIS_TYPE_YEAR_HIGHNUM			=	198,	//年超上限次数
	HIS_TYPE_YEAR_HIGHTIME			=	199,	//年超上限时间
	HIS_TYPE_YEAR_HIHIRATE			=	200,	//年超上上限率
	HIS_TYPE_YEAR_HIHINUM			=	201,	//年超上上限次数
	HIS_TYPE_YEAR_HIHITIME			=	202,	//年超上上限时间
	HIS_TYPE_YEAR_LOWRATE			=	203,	//年超下限率（年谷电量）
	HIS_TYPE_YEAR_LOWNUM			=	204,	//年超下限次数
	HIS_TYPE_YEAR_LOWTIME			=	205,	//年超下限时间
	HIS_TYPE_YEAR_LOLOWRATE			=	206,	//年超下下限率
	HIS_TYPE_YEAR_LOLOWNUM			=	207,	//年超上下限次数
	HIS_TYPE_YEAR_LOLOWTIME			=	208,	//年超下下限时间
	HIS_TYPE_YEAR_POWERCUTTIME		=	209,	//年停电时间(分钟)
	HIS_TYPE_YEAR_LOADRATE			=	210,	//年负荷率
	HIS_TYPE_YEAR_END				=	219		//年统计的结束类型号
};

#endif	//_PDT_SCD_HISDATA_DEF_H_

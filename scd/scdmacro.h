/*==============================================================================
* 文件名称 : scdmacro.h
* 文件功能 ：scada各种宏定义
* 模块 : scada
* 创建时间 : 2009-06-08
* 作者 : wfp
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	PDT_SCD_MACRO_H_
#define	PDT_SCD_MACRO_H_
	
#define SCD_HISDATA_FILEDIR			"hisdata"				//历史数据文件夹
#define SCD_ERRDATA_FILEDIR			"errdata"				//错误数据文件夹
#define SCD_ERR_FILE_PREFIX			"err"					//错误数据文件前缀
#define SCD_HISDATA_FILE_PREFIX		"hd"					//历史数据文件前缀
#define SCD_MAX_RECORDS_INFILE		4000					//文件可存储的最大记录数

#define SCD_YC_TABLE_PREFIX			"this_yc"				//遥测历史数据表前缀
#define SCD_YC_PROCEDURE			"phis_createyctable"	//遥测历史数据的建表存储过程
#define	SCD_YC_INSERT_PROCEDURE		"phis_inserthisdata"	//遥测历史数据的存储过程
#define SCD_EVENT					"this_event"			//事项历史表
#define SCD_DAY_STA_TABLE_PREFIX	"this_dyc"				//日统计表前缀
#define SCD_DAY_STA_PROCEDURE		"phis_createdaystatable"//日统计的建表存储过程
#define SCD_MONTH_STA_TABLE			"this_myc"				//月统计表
#define SCD_SEASON_STA_TABLE		"this_syc"				//季统计表
#define SCD_YEAR_STA_TABLE			"this_yyc"				//年统计表
#define SCD_STA_INSERT_PROCEDURE	"phis_insertsta"		//统计数据入库的存储过程


//蓄电池
#define SCD_BAT_RUN_TB_PREFIX	"this_battery"				//运行监测表前缀
#define SCD_BAT_RUN_PROCEDURE	"phis_createbatteryruntable" //蓄电池运行监测表的建表存储过程
#define SCD_BAT_CHARGE_TABLE	"this_battery_charge"		//充放电表
#define SCD_BAT_DYNAMIC_TABLE	"this_battery_dynamic"		//动态放电表
#define SCD_BAT_STATE_TABLE		"this_battery_state"		//蓄电池历史状态表

//系统自定义计算公式编码
#define SCD_FORMULA_SYS_SGZ			"sgz"					//本路径参与事故汇总的遥信-->本路径事故总
#define SCD_FORMULA_SYS_ROUTESTATE	"routestate"			//本路径状态-->本路径遥信
#define SCD_FORMULA_SYS_RDL	"rdl"			//日实时电量计算
#define SCD_FORMULA_SYS_BKSTATE	"bkstate"			//开关状态计算


#endif //PDT_DAC_MACRO_H_

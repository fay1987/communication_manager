/*==============================================================================
* 文件名称 : dacmacro.h
* 文件功能 ：数据采集各种宏定义
* 模块 : 数据采集
* 创建时间 : 2009-06-08
* 作者 : BBB
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	PDT_DAC_MACRO_H_
#define	PDT_DAC_MACRO_H_

#define DAC_CODE_LEN				32			//代码长度
#define DAC_LCODE_LEN				64			//代码长度(加长型)
#define	DAC_SNAME_LEN				32			//名字长度(short型)
#define	DAC_NAME_LEN				64			//名字长度
#define	DAC_LNAME_LEN				128			//名字长度(加长型)
#define	DAC_DESC_LEN				64			//描述长度
#define DAC_LDESC_LEN				128			//描述长度(加长型)
#define DAC_USER_LEN				64			//用户名长度
#define DAC_PWD_LEN					64			//密码长度	
#define	DAC_ADDR_LEN				32			//地址长度
#define	DAC_HOSTNAME_LEN			32			//主机名长度

#ifdef _ARM_LINUX
#define	DAC_ROUTE_IN_CHANNEL		16			//每CHANNEL支持挂接的路径数
#define	DAC_ROUTE_IN_GROUP			16			//每GROUP支持挂接的路径数
#else
#define	DAC_ROUTE_IN_CHANNEL		16			//每CHANNEL支持挂接的路径数
#define	DAC_ROUTE_IN_GROUP			16			//每GROUP支持挂接的路径数
#endif

#define	DAC_ADDR_NUM				2			//每CHANNEL可配置的远方地址个数
#define DAC_CTRL_LEN				256			//控制指令结构有效内容最大长度
#define DAC_CTRL_NUM				128			//控制指令缓冲区最大缓冲命令个数
#define DAC_ALARM_SYNTAX_LEN		32			//告警语法长度
#define DAC_UNIT_LEN				8			//单位长度
#define DAC_RECV_BUFSIZE			8192		//接收缓冲区大小(8k)
#define DAC_SEND_BUFSIZE			8192		//发送缓冲区大小(8k)
#define DAC_MSG_DATA_LEN			512			//解析后报文的大小(512bytes)
#define DAC_MSG_NUM					32			//每route msg个数

#define DAC_EIISERIAL_LEN			16			//eii设备序列号长度
#define DAC_EIIPARA_LEN				32			//eii设备参数长度

//notify by wlq 20170307 支持1024通道
#ifdef _ARM_LINUX
#define DAC_STATION_NUM				1		//厂站个数
#define	DAC_CHANNEL_NUM				8		//通道个数
#define	DAC_GROUP_NUM				128		//数据组个数
#define	DAC_ROUTE_NUM				128		//路径个数
#define DAC_SENDDEV_NUM				128		//监测点个数

#else

//#define DAC_STATION_NUM				256			//厂站个数
//#define	DAC_CHANNEL_NUM				128		//通道个数
//#define	DAC_GROUP_NUM				128		//数据组个数
//#define	DAC_ROUTE_NUM				160		//路径个数
#define DAC_STATION_NUM				256			//厂站个数
#define	DAC_CHANNEL_NUM				128		//通道个数
#define	DAC_GROUP_NUM				128		//数据组个数
#define	DAC_ROUTE_NUM				160		//路径个数
#define DAC_SENDDEV_NUM				128		//监测点个数

#endif


#define	DAC_PROTOCOL_NUM			128			//规约个数

//wfp add at 20110826
#define DAC_YK_NUM					2048		//遥控个数
#define DAC_YT_NUM					2048		//遥调个数

#define DAC_PROTO_RUN_DELAY			10000		//10ms

//存盘间隔类型--修改数据库
#define DAC_INTERVAL_1_MIN_VAL		1			//1分钟存盘点
#define DAC_INTERVAL_5_MIN_VAL		5			//5分钟存盘点
#define DAC_INTERVAL_10_MIN_VAL		10			//10分钟存盘点
#define DAC_INTERVAL_15_MIN_VAL		15			//15分钟存盘点
#define DAC_INTERVAL_30_MIN_VAL		30			//30分钟存盘点
#define DAC_INTERVAL_HOUR_VAL		60			//时采集点
#define DAC_INTERVAL_DAY_VAL		1440		//日采集点
#define DAC_INTERVAL_MONTH_VAL		17280		//月采集点

#define DAC_OVER_INC_TIME			5			//突变点持续时间


#endif //PDT_DAC_MACRO_H_

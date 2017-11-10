/*==============================================================================
* 文件名称 : dacmacro.h
* 文件功能 ：数据采集各种宏定义
* 模块 : 数据采集
* 创建时间 : 2009-06-08
* 作者 : wlq
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	PDT_PAS_MACRO_H_
#define	PDT_PAS_MACRO_H_

#define PAS_CODE_LEN				32			//代码长度
#define PAS_LCODE_LEN				64			//代码长度(加长型)
#define	PAS_SNAME_LEN				32			//名字长度(short型)
#define	PAS_NAME_LEN				64			//名字长度
#define	PAS_LNAME_LEN				128			//名字长度(加长型)
#define	PAS_DESC_LEN				64			//描述长度
#define PAS_LDESC_LEN				128			//描述长度(加长型)
#define PAS_USER_LEN				64			//用户名长度
#define PAS_PWD_LEN					64			//密码长度	
#define	PAS_ADDR_LEN				32			//地址长度
#define	PAS_HOSTNAME_LEN			32			//主机名长度
#define PAS_CTRL_LEN				256			//控制指令结构有效内容最大长度
#define PAS_CTRL_NUM				128			//控制指令缓冲区最大缓冲命令个数
#define PAS_ALARM_SYNTAX_LEN		32			//告警语法长度
#define PAS_UNIT_LEN				8			//单位长度

#define	PAS_GROUP_NUM				1024		//算法参数个数
#define	PAS_ROUTE_NUM				32			//算法任务个数
#define	PAS_PROTOCOL_NUM			32			//算法个数


#define PAS_PROTO_RUN_DELAY			10000		//10ms

//算法任务类型
enum ALGORI_TASKTYPE
{
	TASKTYPE_PERIOD		=	0,			//周期
	DATATYPE_TIMING		=	1,			//定时
	DATATYPE_TRIGGER	=	2			//触发
};

enum ALGORI_PRARTYPE
{
	PRARTYPE_INPUT		=	0,			//输入
	PRARTYPE_OUTPUT						//输出
	PRARTYPE_PRESETVALUE			//固定值
};

enum ALGORI_MEASTYPE
{
	MEASTYPE_YC		=	0,			//遥测
	MEASTYPE_YX,					//遥信
	MEASTYPE_YK,					//遥控
	MEASTYPE_YT						//遥调
};


#endif //PDT_PAS_MACRO_H_

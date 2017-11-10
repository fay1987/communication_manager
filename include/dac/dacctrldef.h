/*===================================================================
*	模块名称：	数据采集
*	创建日期:	2013年11月27日 9:34
*	文 件 名: 	E:\pdt3000\include\dac\dacctrldef.h
*	文件路径:	E:\pdt3000\include\dac
*	模块名称:	dacctrldef
*	文件类型:	h
	
*	作    者:	wlq
*	版    本:	V2.0
	
*	模块说明:	数据控制类型定义

*	修改版本：	V2.1
*	修改作者：	wlq
*	修改日期：	27/11/2013
*	修改摘要：

====================================================================*/

#ifndef	_PDT_DAC_DACCTRLDEF_H_
#define	_PDT_DAC_DACCTRLDEF_H_

#include "utl/datatype.h"

#define CTRL_COMMON_LEN		200		//通用控制命令长度,防止超ctrl buf len=256

namespace PDT
{
	//控制信息头
	typedef struct _ctrl_head_ 
	{
		nUInt32	type;				//命令类型,详见 fdcdef.h 中的 CTRL_TYPE
	}ctrl_head;

	//加载遥测、遥信、电度参数
	typedef struct _ctrl_loadpara_data_ 
	{
		char	group[DAC_CODE_LEN];		//数据组编码
	}ctrl_loadpara_data;

	//人工操作链路、路径、数据组
	typedef struct _ctrl_man_operate_
	{
		char	code[DAC_CODE_LEN];			//链路名、路径名、数据组编码
	}ctrl_man_operate;

	//切换数据组主路径
	typedef struct _ctrl_switch_mainroute_ 
	{	
		char	groupCode[DAC_CODE_LEN];			//数据组编码
		char	newMainRouteCode[DAC_CODE_LEN];		//新的主路径名
	}ctrl_switch_mainroute ;

	//设置数据组切换模式
	typedef struct _ctrl_set_switch_mode_ 
	{
		char	groupCode[DAC_CODE_LEN];	//数据组编码
		nUInt8	mode;						//新的切换模式
	}ctrl_set_switch_mode;

	//规约对组操作的命令，如复位、对时、总召唤等
	typedef struct _ctrl_pro_group_cmd_ 
	{
		char	groupCode[DAC_CODE_LEN];	//数据组编码
		nInt32	subgroupNo;					//分组召唤中使用到的分组号
	}ctrl_pro_group_cmd;

	//规约遥控
	typedef struct _ctrl_pro_yk_ 
	{
		char	groupCode[DAC_CODE_LEN];	//数据组编码
		nUInt8	ctrlType;					//控制类型，如遥控预置、执行、撤销
		nInt32	ctrlNo;						//控制号
		nInt32	ctrlOnNo;					//控合号
		nInt32	ctrlOffNo;					//控分号
		nUInt32	ctrlDuration;				//持续时间
		nUInt8	ctrlState;					//希望控制的状态
		nUInt8	ackState;					//返回状态
		char	ackReciever;				//返回信息接收者(如：SCADA_SERVER,FDC_SERVER)
		nUInt8	elapseTime;					//命令未被执行而流逝的时间
	}ctrl_pro_yk;

	//规约遥控返校
	typedef struct _ctrl_pro_yk_ack_
	{
		nUInt8	funcCode;					//功能码
		nInt32	groupNo;					//数据组号
		nInt32	ctrlNo;						//点名
		nUInt8	ackState;					//返回状态
	}ctrl_pro_yk_ack;

	//规约设点
	typedef struct _ctrl_pro_setpoint_
	{
		char	groupCode[DAC_CODE_LEN];	//数据组编码
		char	pointCode[DAC_CODE_LEN];	//点名
		nFloat	floatValue;					//命令附带的浮点值
		nInt32	int32Value;					//命令附带的整型值
		nUInt8	localFlag;					//是否只设置本地内存
	}ctrl_pro_setpoint;

	//规约设点扩展
	typedef struct _ctrl_pro_setpointex_
	{
		char	groupCode[DAC_CODE_LEN];	//数据组编码
		char	pointCode[DAC_CODE_LEN];	//点名		
		nFloat	floatValue;					//命令附带的浮点值
		nInt32	ctrlNo;						//设置号
		nUInt8	ctrlType;					//控制类型，如遥控预置、执行、撤销
		nUInt8	localFlag;					//是否只设置本地内存
	}ctrl_pro_setpointex;

	//规约直接操作
	typedef struct _ctrl_pro_direct_operate_
	{
		nUInt8	type;						//命令类型
		char	groupCode[DAC_CODE_LEN];	//数据组编码
		char	pointCode[DAC_CODE_LEN];	//点名
		nUInt32	pulseCount;					//脉冲计数
		nUInt32	onTime;						//
		nUInt32	offTime;					//
	}ctrl_pro_direct_operate;

	//add by yaoff on 20150502
	//定值处理
	typedef struct _ctrl_pro_constantvalue_
	{
		char	groupCode[DAC_CODE_LEN];	//数据组号
		nUInt8	ctrlType;
		nUInt32	length;						//外部自定义命令缓冲长度
		char	cmdBuf[CTRL_COMMON_LEN];	//外部自定义命令缓冲
	}ctrl_pro_constantvalue;

	typedef struct _ctrl_pro_constantvalue_ack_
	{
		nUInt8	ackState;					//返回状态
		nInt32	groupNo;					//数据组号
		//nUInt8	ctrlType;
		nUInt32	length;						//外部自定义命令缓冲长度
		char	ackBuf[DAC_LNAME_LEN];		//外部自定义命令缓冲	
	}ctrl_pro_constantvalue_ack;

	typedef struct _ctrl_pro_calldata_
	{
		char	groupCode[DAC_CODE_LEN];					//数据组号
		nUInt8	ctrlNo;						//命令号
		//char	dataType[DAC_CODE_LEN];		//数据类型
	}ctrl_pro_calldata;

	typedef struct _ctrl_pro_calldata_ack_
	{
		nInt32	groupNo;					//数据组号
		//nUInt8	ctrlNo;
		nUInt8	ackState;						//外部自定义命令缓冲长度
		//char	cmdBuf[CTRL_COMMON_LEN];	//外部自定义命令缓冲
	}ctrl_pro_calldata_ack;

	typedef struct _ctrl_pro_data_soe_
	{
		nInt32	nDataNo;				//数据条数编号
		nUInt8	nErrorType;				//故障类型
		nUInt32	nAction1;				//动作值1
		nUInt32	nAction2;				//动作值2
		nUInt32 nAction3;				//动作值3
		nUInt32 datetime;				//时间
		
	}ctrl_pro_data_soe;


	//add end.

	//规约通用命令
	typedef struct _ctrl_pro_common_
	{
		char	groupCode[DAC_CODE_LEN];	//数据组编码
		nUInt32	length;						//外部自定义命令缓冲长度
		char	cmdBuf[CTRL_COMMON_LEN];	//外部自定义命令缓冲
	}ctrl_pro_common;

	//通用命令反校
	typedef struct _ctrl_pro_common_ack_ 
	{
		nInt32	groupNo;					//数据组号
		nUInt32	length;						//外部自定义反校缓冲长度
		char	ackBuf[CTRL_COMMON_LEN];	//外部自定义反校缓冲
	}ctrl_pro_common_ack;

}
#endif //_PDT_DAC_DACCTRLDEF_H_

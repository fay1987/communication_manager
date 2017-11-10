/*==============================================================================
* 文件名称 : dacdef.h
* 文件功能 ：数据采集各种类型定义
* 模块 : 数据采集
* 创建时间 : 2009-06-08
* 作者 : 
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_DACDEF_H_
#define	_PDT_DAC_DACDEF_H_

namespace	PDT
{
	//CHANNEL用户
	enum	CHANNEL_USER
	{
		CHANNEL_TASK_USER		=	0x01,		//channel执行用
		CHANNEL_TASK_CLIENT		=	0x02,		//channel client
		CHANNEL_TASK_SERVER		=	0x04,		//channel server
		CHANNEL_TASK_UDP		=	0x06,		//channel dgram=udp
		CHANNEL_TASK_SERIAL		=	0x08,		//channel serial
		CHANNEL_TASK_ALL		=	0XFF		//所有任务
	};
	//装载参数表标志
	enum	LOADPARA_FLAG
	{
		LOADPARA_CHANNEL		=	0x00000001,	//通道表
		LOADPARA_GROUP			=	0x00000002,	//数据组表
		LOADPARA_ROUTE			=	0x00000004,	//路径表
		LOADPARA_YC				=	0x00000008,	//YC表
		LOADPARA_YX				=	0x00000010,	//YX表
		LOADPARA_KWH			=	0x00000020,	//KWH表
		LOADPARA_ZFYC			=	0x00000040,	//转发YC表
		LOADPARA_ZFYX			=	0x00000080,	//转发YX表
		LOADPARA_ZFKWH			=	0x00000100,	//转发KWH表
		LOADPARA_PROTOCOL		=	0x00000200,	//Protocol
		LOADPARA_STATION		=	0x00000400,	//厂站表
		LOADPARA_DEVICE			=	0x00000800,	//设备表--scd使用
		LOADPARA_DEVICETYPE		=	0x00001000,	//设备类型表--scd使用
		LOADPARA_DEVICEPARA		=	0x00002000,	//设备参数表--scd使用
		LOADPARA_YK				=	0x00004000,	//遥控表
		LOADPARA_YT				=	0x00008000,	//遥调表
		LOADPARA_ALL			=	0xFFFFFFFF	//全部加载
	};
	//通道状态
	enum	CHANNEL_STATE
	{
		CHANNEL_STATE_DOWN		=	0,			//断开
		CHANNEL_STATE_LISTEN	=	1,			//监听
		CHANNEL_STATE_UP		=	2,			//链接
		CHANNEL_STATE_DISABLED	=	3,			//禁用
	};
	enum	CHANNEL_TYPE
	{
		CHANNEL_TYPE_SERIAL		=	0,			//串口
		CHANNEL_TYPE_TCPCLIENT	=	1,			//TCP客户端
		CHANNEL_TYPE_TCPSERVER	=	2,			//TCP服务端
		CHANNEL_TYPE_UDP		=	3,			//UDP
		CHANNEL_TYPE_GATECLIENT	=	4,			//网关客户端
		CHANNEL_TYPE_GATESERVER =   5,			//网关服务端
		CHANNEL_TYPE_VIRTUAL	=	10			//虚拟类型(不处理收发，直接调度规约)
	};
	//数据组类型
	enum	GROUP_TYPE
	{
		GROUP_TYPE_STATIC_RX	= 0,			//接收组
		GROUP_TYPE_STATIC_TX	= 1,			//发送组
	};
	//数据组状态
	enum	GROUP_STATE
	{
		GROUP_STATE_DOWN		=	0,			//停止
		GROUP_STATE_UP			=	1,			//断开
		GROUP_STATE_DISABLED	=	2,			//禁用
	};
	//路径类型
	enum	ROUTE_TYPE
	{
		ROUTE_TYPE_RX			= 0,			//接收
		ROUTE_TYPE_TX			= 1				//发送
	};
	//路径状态
	enum	ROUTE_STATE
	{
		ROUTE_STATE_DOWN		=	0,			//停止
		ROUTE_STATE_UP			=	1,			//运行
		ROUTE_STATE_STANDBY		=	2,			//热备
		ROUTE_STATE_FREE		=	3,			//空闲
		ROUTE_STATE_DISABLED	=	4,			//禁用
	};
	//遥测类型
	enum	YC_TYPE
	{
		YC_TYPE_U				=	0,			//电压类型
		YC_TYPE_I				=	1,			//电流
		YC_TYPE_P				=	2,			//有功
		YC_TYPE_Q				=	3,			//无功
		YC_TYPE_F				=	4,			//频率 
		YC_TYPE_COS				=	5,			//功率因素
		YC_TYPE_BAT_U			=	6,			//单体电压
		YC_TYPE_BAT_R			=	7,			//单体内阻
		YC_TYPE_BAT_TOL_U		=	8,			//组端电压
		YC_TYPE_BAT_I			=	9,			//蓄电池电流
		YC_TYPE_TEMP			=	10,			//温度
		YC_TYPE_HUMI			=	11			//湿度
	};
	//遥信类型
	enum	YX_TYPE
	{
		YX_TYPE_ALARM			=	0,			//告警类型
		YX_TYPE_SWITCH			=	1,			//开关类型
		YX_TYPE_WORK			=	2			//工作类型
	};
	//报文缓冲区类型
	enum	BUFFER_TYPE
	{
		BUFFER_RX				=	0,			//接收
		BUFFER_TX				=	1			//发送
	};
	//报文帧类型
	enum	FRAME_TYPE
	{
		FRAME_RX				=	0,			//接收
		FRAME_RX_ERROR			=	1,			//接收的错误帧
		FRAME_TX				=	2			//发送
	};
	//质量码位定义
	enum	QUALITY_BIT
	{
		QUALITY_BIT_OV			=	0x00000001,	//溢出位		:1--溢出
		QUALITY_BIT_ES			=	0x00000002,	//估计位		:1--估计
		QUALITY_BIT_MS			=	0x00000004,	//人工置数		:1--人工置数
		QUALITY_BIT_WAR			=	0x00000008,	//告警位		:1--告警
		QUALITY_BIT_BL			=	0x00000010,	//闭锁位		:1--闭锁
		QUALITY_BIT_SB			=	0x00000020,	//取代位		:1--取代
		QUALITY_BIT_NT			=	0x00000040,	//更新位		:1--有更新
		QUALITY_BIT_IV			=	0x00000080,	//无效位		:0--有效,1--无效
		//其他
		QUALITY_BIT_INIT		=	0x10000000	//初始化位		:1--初始化
	};
	enum	TIME_MODE
	{//格林威治时间早8小时
		TIME_BEIJING			=	0,			//北京时间
		TIME_GREENWICH			=	1			//格林威治时间
	};
	//YK状态
	enum	YK_STATE
	{
		YK_ERR					=	0,			//错误
		YK_ON					=	1,			//合
		YK_OFF					=	2			//分
	};

	//控制命令类型
	enum CTRL_TYPE
	{
		//加载参数命令 ( 1-100 )
		CTRL_LOADPARA_ALL				=	1,			//加载参数ALL
		CTRL_LOADPARA_CHANNEL			=	2,			//加载参数CHANNEL
		CTRL_LOADPARA_GROUP				=	3,			//加载参数GROUP
		CTRL_LOADPARA_ROUTE				=	4,			//加载参数ROUTE
		CTRL_LOADPARA_PROTOCOL			=	5,			//加载参数PROTOCOL
		CTRL_LOADPARA_COM				=	6,			//加载参数通讯类型
		CTRL_LOADPARA_YC				=	7,			//加载参数YC
		CTRL_LOADPARA_YX				=	8,			//加载参数YX
		CTRL_LOADPARA_KWH				=	9,			//加载参数KWH
		CTRL_LOADPARA_STATION			=	10,			//加载参数station
		CTRL_LOADPARA_DEVICE			=	11,			//加载参数device
		CTRL_LOADPARA_DEVICETYPE		=	12,			//加载参数devicetype
		CTRL_LOADPARA_DEVICEPARA		=	13,			//加载参数devicepara
		CTRL_LOADPARA_YK				=	14,			//加载参数yk
		CTRL_LOADPARA_YT				=	15,			//加载参数yt
		//通道、路径、数据组控制 ( 101-200 )
		CTRL_MAN_START_CHANNEL			=	101,		//人工启动通道
		CTRL_MAN_STOP_CHANNEL			=	102,		//人工停止通道
		CTRL_MAN_ENABLE_CHANNEL			=	103,		//人工启用通道
		CTRL_MAN_DISABLE_CHANNEL		=	104,		//人工禁用通道
		CTRL_MAN_START_ROUTE			=	111,		//人工启动路径
		CTRL_MAN_STOP_ROUTE				=	112,		//人工停止路径
		CTRL_MAN_ENABLE_ROUTE			=	113,		//人工启用路径
		CTRL_MAN_DISABLE_ROUTE			=	114,		//人工禁用路径
		CTRL_MAN_RESET_PROTOCOL			=	115,		//人工重启制定路径的规约
		CTRL_MAN_START_GROUP			=	121,		//人工启动数据组
		CTRL_MAN_STOP_GROUP				=	122,		//人工停止数据组
		CTRL_MAN_ENABLE_GROUP			=	123,		//人工启用数据组
		CTRL_MAN_DISABLE_GROUP			=	124,		//人工禁用数据组
		//规约控制 ( 201-300 )
		CTRL_PRO_RESET_RTU				=	201,		//复位RTU
		CTRL_PRO_SYNC_TIME				=	202,		//对时
		CTRL_PRO_CALL_ALL_DATA			=	203,		//总召唤
		CTRL_PRO_CALL_ALL_DATA_YC		=	204,		//总召遥测
		CTRL_PRO_CALL_ALL_DATA_YX		=	205,		//总召遥信
		CTRL_PRO_CALL_ALL_DATA_KWH		=	206,		//总召电度
		CTRL_PRO_CALL_SUBGROUP_DATA		=	207,		//分组召唤(101规约)
		CTRL_PRO_YK						=	208,		//遥控
		CTRL_PRO_YT						=	209,		//遥调
		CTRL_PRO_SETPOINT_YC			=	210,		//设点遥测
		CTRL_PRO_SETPOINT_YX			=	211,		//设点遥信
		CTRL_PRO_SETPOINT_KWH			=	212,		//设点电度
		CTRL_PRO_DIRECT_OPERATE			=	213,		//直接操作
		//通用命令
		CTRL_PRO_COMMON					=	299,		//自定义通用命令
		CTRL_PRO_UNKNOWN				=	300,		//仅标识类型结束
		//规约控制返校信息( 301-400 )
		CTRL_PRO_ACK_YK					=	301,		//遥控返校	
		CTRL_PRO_COMMON_ACK				=	303			//自定义通用命令反校
	};

	//日志类型
	enum DAC_LOG_TYPE
	{
		LOG_NET_CLIENT_BASE				=	1300,		//tcp client base
		LOG_NET_CLIENT_RECV				=	1301,		//recv
		LOG_NET_CLIENT_SEND				=	1302,		//send
		LOG_NET_SERVER_BASE				=	1305,		//tcp server base
		LOG_NET_SERVER_RECV				=	1306,		//recv
		LOG_NET_SERVER_SEND				=	1307,		//send
		LOG_NET_UDP_BASE				=	1310,		//udp base
		LOG_NET_UDP_RECV				=	1311,		//udp recv
		LOG_NET_UDP_SEND				=	1312,		//udp send
		LOG_SERIAL_BASE					=	1320,		//serial base
		LOG_SERIAL_RECV					=	1321,		//serial recv
		LOG_SERIAL_SEND					=	1322,		//serial send

		LOG_DACSHM_BASE					=	1400,		//dacshm日志基础值
		LOG_COMMINF_BASE				=	1410,		//comminf基础值
		LOG_DATAINF_BASE				=	1420,		//datainf基础值
		LOG_CTRLINF_BASE				=	1430,		//ctrlinf基础值
		LOG_LOADPARA_BASE				=	1440,		//loadpara日志基础值

		/////////////dacsrv////////
		LOG_DACSRV_BASE					=	1500,		//dacsrv进程日志基础值
		LOG_DACSRV_CONFIG				=	1501,		//配置文件操作
		LOG_DACSRV_LINKPOOL				=	1502,		//linktaskpool
		
		LOG_DACSRV_CTRLTASK				=	1503,		//控制任务
		LOG_DACSRV_LINK					=	1510,		//链路
		LOG_DACSRV_STATETASK			=	1520,		//状态检测
		LOG_DACSRV_ZFTASK				=	1521		//转发
	};
}

#endif //_PDT_DAC_DACDEF_H_

/*==============================================================================
* 文件名称 : dac.h
* 文件功能 ：数据采集共享内存结构定义
* 模块 : 数据采集
* 创建时间 : 2009-06-08
* 作者 : wlq
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_H_
#define	_PDT_DAC_H_

#include "utl/datatype.h"
#include "dacmacro.h"
//---------------------------------------------------------------------------------------
//数据扫描周期参数
typedef	struct _dac_scan_cycle_
{
	nUInt32	timesync;			//时钟同步（单位:秒）
	nUInt32	all_data;			//全数据（单位:秒）
	nUInt32	all_yc;				//全YC（单位：秒）
	nUInt32	all_yx;				//全YX（单位：秒）
	nUInt32	all_kwh;			//全KWH（单位：秒）
	nUInt32	change_data;		//变化数据
	nUInt32	change_yc;			//变化YC（单位：秒）
	nUInt32	change_yx;			//变化YX（单位：秒）
	nUInt32	change_kwh;			//变化KWH（单位：秒）
	nUInt32	soe;				//SOE（单位：秒）
}DAC_SCAN_CYCLE;
//前置命令
typedef struct _dac_command_
{
	nBool		timesync;			//时钟同步
	nBool		all_data;			//全数据
	nBool		all_yc;				//全YC
	nBool		all_yx;				//全YX
	nBool		all_kwh;			//全KWH
	nBool		change_data;		//变化数据
	nBool		change_yc;			//变化YC
	nBool		change_yx;			//变化YX
	nBool		change_kwh;			//变化KWH
	nBool		soe;				//SOE
}DAC_COMMAND;
//前置控制指令(遥控、遥调、设点、直接输出等等)
typedef	struct _dac_ctrl_cmd
{
	nBool		valid;						//缓冲空间是否被占用,true-已经被占用,false-未被占用
	nUInt32		type;						//类型
	nUInt32		groupNo;					//组序号
	nUInt32		inputTime;					//命令被添加进来时的时间(time(NULL)的秒数)，用于超时处理
	nUInt32		len;						//缓冲区命令内容有效字节长度
	nUInt8		buf[DAC_CTRL_LEN];			//缓冲的命令内容(详见 dacctrl.h,ctrl_head + other cmd struct )
}DAC_CTRL_CMD;
//=======================================================================================
//系统参数
typedef	struct _dac_system_
{
	char		name[DAC_NAME_LEN];				//名称
	char		desc[DAC_DESC_LEN];				//描述
}DAC_SYSTEM;
//厂站表
typedef struct _dac_station_
{
	hBool		valid;									//有效标志
	char		code[DAC_CODE_LEN];						//编码
	char		name[DAC_NAME_LEN];						//名称
	hInt32		level;									//等级
	char		address[DAC_NAME_LEN];					//位置
	char		sgzCode[DAC_NAME_LEN];					//事故总编码(grpcode.code)
	char		mrid[DAC_NAME_LEN];						//统一编码(wfp add at 20110826)
	long		cmpynum;								//企业户号
	int			cmpyid;									//企业标识
}DAC_STATION;

typedef	struct _dac_recvdev_
{
	hBool		valid;									//有效标志
	char		code[DAC_CODE_LEN];						//编码
	char		name[DAC_NAME_LEN];						//名称
	hInt32		recvno;									//接收装置编号
	char		addr[DAC_ADDR_LEN];						//上级监控点编号
	int			frequency;								//频率
	hBool		isInit;									//是否根据配置修改过下位机频率地址
	char		chancode[DAC_CODE_LEN];					//所属通道
}DAC_RECVDEV;

typedef	struct _dac_senddev_
{
	hBool		valid;									//有效标志
	char		code[DAC_CODE_LEN];						//编码
	char		name[DAC_NAME_LEN];						//名称
	hInt32		seeno;									//监控点编号
	char		upseecode[DAC_CODE_LEN];				//上级监控点编号
	char		rdevcode[DAC_CODE_LEN];					//对应接收装置
	char 		cmac[DAC_MACADDRESS_LEN];
	int			frequency;
	int			rtu;
	hBool		isInit;									//是否根据配置修改过下位机频率地址
	char		grpcode[DAC_CODE_LEN];					//所属数据组
}DAC_SENDDEV;

//通道表
typedef struct _dac_channel_
{
	hBool		valid;									//有效标志
	char		code[DAC_CODE_LEN];						//编码
	char		name[DAC_NAME_LEN];						//名称
	hUInt8		type;									//类型（0：串口，1：client，2：server，3：udp）
	hUInt8		datatype;								//数据类型（0：普通，1：eii，2：宏电）
	char		localAddr[DAC_ADDR_LEN];				//本地地址
	char		remoteAddr[DAC_ADDR_NUM][DAC_ADDR_LEN];	//远端地址1
	hUInt32		timeout;								//超时时间(s)
	hUInt32		switchTimeOut;							//多路径切换超时时间
	char		eiiSerialNo[DAC_EIISERIAL_LEN];			//eii设备序列号
	char		eiiPara[DAC_EIIPARA_LEN];				//eii设备参数
	hBool		isEvent;								//是否产生事项
	hUInt8		eventLevel;								//报警等级
	hBool		isMsgAlarm;								//是否短信告警
	hUInt8		msgAlarmLevel;							//短信报警等级
	hBool		reserve1;								//预留1
	hUInt8		reserve2;								//预留2
	hUInt8		initState;								//初始状态（0:断开，1:链接）
}DAC_CHANNEL;
//数据组表
typedef struct _dac_group_
{
	hBool		valid;						//有效标志
	char		code[DAC_CODE_LEN];			//编码
	char		name[DAC_NAME_LEN];			//名称
	hInt32		station;					//厂站
	hUInt8		type;						//类型（0：接收，1：转发）
	hUInt32		ycNum;						//YC最大个数
	hUInt32		yxNum;						//YX最大个数
	hUInt32		kwhNum;						//KWH最大个数
	hUInt32		deadTime;					//死区时间(单位：秒，用于判死数)
	hBool		isEvent;					//是否产生事项
	hUInt8		eventLevel;					//报警等级
	hBool		isMsgAlarm;					//是否短信告警
	hUInt8		msgAlarmLevel;				//短信报警等级
	char		mrid[DAC_NAME_LEN];			//统一编码(wfp add at 20110826)
	hBool		reserve1;					//预留1
	hUInt8		reserve2;					//预留2
	hUInt8		initState;					//初始状态（0——停止，1——运行）
}DAC_GROUP;
//路径参数
typedef struct _dac_route_
{
	hBool			valid;						//有效标志
	char			code[DAC_CODE_LEN];			//编码
	char			name[DAC_NAME_LEN];			//名称
	hUInt8			type;						//类型（0：接收，1：转发）
	hInt32			group;						//组号
	hInt32			channel;					//通道号
	hUInt32			localAddr;					//本端站址
	hUInt32			remoteAddr;					//远端站址
	hInt32			protocol;					//规约类型
	char			featureCode[DAC_CODE_LEN];	//规约特征编码
	DAC_SCAN_CYCLE	scanInterval;				//数据扫描间隔
	hBool			timeSyncFlag;				//时间同步标志
	hBool			isReadOnly;					//只读标志(0:否；1：是)
	hUInt32			stopLimit;					//停止时限
	hUInt32			errRateLimit;				//误码率限值
	hUInt32			errTimeLimit;				//高误码率时限
	hUInt32			cmdTimeout;					//命令反校超时时间
	hUInt32			cmdRetryInterval;			//命令重发时间间隔
	hUInt32			cmdRetryCnt;				//命令重复次数
	hUInt8			timeMode;					//时区模式(0:北京;1:格林威治)
	hBool			isEvent;					//是否产生事项
	hUInt8			eventLevel;					//报警等级
	hBool			isMsgAlarm;					//是否短信告警
	hUInt8			msgAlarmLevel;				//短信报警等级
	hBool			reserve1;					//预留1
	hUInt8			reserve2;					//预留2
	hUInt8			initState;					//初始状态
}DAC_ROUTE;
//规约表
typedef	struct _dac_protocol_
{
	hBool			valid;						//有效标志
	char			code[DAC_CODE_LEN];			//编码
	char			name[DAC_NAME_LEN];			//名称
	char			libName[DAC_SNAME_LEN];		//规约库名
	char			featureTable[DAC_SNAME_LEN];//特征表表名
}DAC_PROTOCOL;
//前置遥测参数表
typedef	struct _dac_yc_
{
	hBool			valid;						//有效标志
	char			code[DAC_CODE_LEN];			//编码
	char			name[DAC_NAME_LEN];			//名称
	hBool			calc;						//是否计算点标志(wfp add at 20111019)			
	char			formula[DAC_CODE_LEN];		//计算公式编码
	hUInt8			type;						//类型
	hInt32			srcGroup;					//源组序号
	hInt32			srcNo;						//源序号
	hBool			isEvent;					//是否产生事项
	hUInt8			eventLevel;					//报警等级(wfp add 20110826)
	hUInt32			eventType;					//事项类型
	char			unit[DAC_UNIT_LEN];			//单位
	hBool			isStat;						//是否统计
	hFloat			coefK;						//系数
	hFloat			coefB;						//截距
	hFloat			zeroVal;					//零值范围
	hFloat			maxChange;					//最大增量
	hUInt32			saveInterval;				//存盘间隔(min)
	hFloat			minChgVal;					//阀值
	hFloat			highLimit;					//上限值
	hFloat			lowLimit;					//下限值
	hFloat			hihiLimit;					//上上限
	hFloat			lolowLimit;					//下下限
	hUInt8			eventDelay;					//事项延时(s)
	hFloat			maxIllegal;					//最大非法值
	hFloat			minIllegal;					//最小非法值
	hBool			isMsgAlarm;					//是否短信报警
	hUInt8			msgAlarmLevel;				//短信报警等级
	hBool			isControl;					//是否遥调
	hUInt8			ctrlType;					//遥调类型
	hInt32			ctrlNo;						//遥调号
	char			mrid[DAC_NAME_LEN];			//统一编码(wfp add at 20110826)
	//info
	hBool			hasPutEvent;				//延时事项是否已经生成
	hUInt32			eventStartTime;				//事项延时的起始时间
	hUInt8			limitType;					//限值类型
	hUInt32			overIncCnt;					//突变计数
	//hBool			change;						//值变化标志,由dac置位,scd复位
	hUInt32			reserve1;					//预留1
	hUInt8			reserve2;					//预留2
}DAC_YC;
//前置遥信参数表
typedef	struct _dac_yx_
{
	hBool			valid;						//有效标志
	char			code[DAC_CODE_LEN];			//编码
	char			name[DAC_NAME_LEN];			//名称
	hBool			calc;						//是否计算点标志(wfp add at 20111019)	
	char			formula[DAC_CODE_LEN];		//计算公式编码
	hUInt8			type;						//类型
	hInt8			normal;						//正常状态值(wfp add 20110826)
	hInt32			srcGroup;					//源组序号
	hInt32			srcNo;						//源序号
	hBool			isEvent;					//是否产生事项
	hUInt8			eventLevel;					//报警等级(wfp add 20110826)
	hUInt32			eventType;					//事项类型
	hBool			isStat;						//是否统计
	hBool			reverse;					//取反标志
	hBool			isMsgAlarm;					//是否短信报警
	hUInt8			msgAlarmLevel;				//短信报警等级
	hBool			isInSgz;					//是否是事故总的组合项
	hBool			isControl;					//是否遥控
	hUInt8			ctrlType;					//遥控类型
	hInt32			ctrlNo;						//遥控号
	char			mrid[DAC_NAME_LEN];			//统一编码(wfp add at 20110826)
	hBool			reserve1;					//预留1
	hUInt8			reserve2;					//预留2
}DAC_YX;
//前置电度参数表
typedef	struct _dac_kwh_
{
	hBool			valid;						//有效标志
	char			code[DAC_CODE_LEN];			//编码
	char			name[DAC_NAME_LEN];			//名称
	hInt32			srcGroup;					//源组序号
	hInt32			srcNo;						//源序号
	hFloat			coefK;						//系数
	hFloat			coefB;						//截距
}DAC_KWH;
//前置事件表
typedef	struct _dac_soe_
{
	hBool			valid;						//有效标志
	char			code[DAC_CODE_LEN];			//编码
	char			alias[DAC_CODE_LEN];		//别名
	hInt8			normal;						//正常状态值(wfp add 20110826)
	hInt32			srcGroup;					//源组序号
	hInt32			srcNo;						//源序号
	hBool			isEvent;					//是否产生事项
	hUInt8			eventLevel;					//报警等级(wfp add 20110826)
	hUInt32			eventType;					//事项类型
	hBool			isStat;						//是否统计
	hBool			isMsgAlarm;					//是否短信报警
	hUInt8			msgAlarmLevel;				//短信报警等级
}DAC_SOE;
//前置遥控表
typedef	struct _dac_yk_
{
	hBool			valid;						//有效标志
	char			routeCode[DAC_CODE_LEN];	//路径编码
	char			code[DAC_CODE_LEN];			//关联测点编码
	char			no;							//关联测点序号
	char			name[DAC_NAME_LEN];			//名称
	hInt32			ykno;						//遥控序号
	hInt32			yktype;						//遥控类型
	char			mrid[DAC_NAME_LEN];			//统一编码
}DAC_YK;
//前置遥调表
typedef	struct _dac_yt_
{
	hBool			valid;						//有效标志
	char			routeCode[DAC_CODE_LEN];	//路径编码
	char			code[DAC_CODE_LEN];			//关联测点编码
	char			no;							//关联测点序号
	char			name[DAC_NAME_LEN];			//名称
	hInt32			ytno;						//遥调序号
	char			mrid[DAC_NAME_LEN];			//统一编码
}DAC_YT;

//不需要实时召唤的测点
typedef	struct _dac_yccon_
{
	hBool			valid;
	short			ntype;						//测点类型 0 电能  1 谐波  2 装置参数   3 SOE  4 录波
	char			code[DAC_CODE_LEN];			//测点编码
	char			name[DAC_NAME_LEN];			//测点名称
	short			ndatatype;					//数据类型 0 遥测  1 遥信
}DAC_YCCON;
//=======================================================================================
//系统信息
typedef	struct _dac_system_info_
{
	hBool			serverFlag;					//是否是DAC服务器
	hUInt32			stationNum;					//系统实际厂站数码
	hUInt32			recvdevNum;					//系统接收装置数量
	hUInt32			senddevNum;					//系统实际监测点数目
	hUInt32			channelNum;					//系统实际通道数目
	hUInt32			groupNum;					//系统实际数据组数目
	hUInt32			routeNum;					//系统实际路径数目
	hBool			initFlag;					//初始化标志
	hUInt32			loadParaFlag;				//装载参数表标志，根据位不同区分参数表
}DAC_SYSTEM_INFO;
//通道信息
typedef struct _dac_channel_info_
{
	nUInt8			state;						//状态（连接、断开）
	nUInt32			lastUpTime;					//最近一次由DOWN->UP的时间
	nUInt32			lastDownTime;				//最近一次由UP->DOWN的时间
	nUInt32			txByteCnt;					//发送字节数
	nUInt32			rxByteCnt;					//接收字节数
	nUInt32			dayOkTime;					//日运行时间
	nUInt32			dayErrCnt;					//日故障次数
	nUInt8			reset;						//复位标志
	nBool			manStop;					//人工停止标志
	nInt32			activeAddr;					//活动地址（在remoteAddr中的序号）
	nUInt32			curRouteOrder;				//当前路径的在routes中的序号，用于发送cmd
	nUInt32			routeNum;					//与通道相连的路径个数
	nInt32			routes[DAC_ROUTE_IN_CHANNEL];//与通道相连的路径号	
}DAC_CHANNEL_INFO;



//接收装置
typedef struct _dac_recvdev_info_
{
	nInt32			channo;							//对应的通道号
	nUInt32			SdevNum;						//与接收装置相连接的发送装置个数
	nInt32			sdevs[DAC_ROUTE_IN_CHANNEL];	//与接收装置相连的发送装置号	
}DAC_RECVDEV_INFO;

//数据组信息
typedef struct _dac_group_info_
{
	nUInt8			state;						//状态（正常）
	nUInt32			lastUpTime;					//最近一次由DOWN->UP的时间
	nUInt32			lastDownTime;				//最近一次由UP->DOWN的时间
	nUInt32			dayOkTime;					//日运行时间
	nUInt32			dayErrCnt;					//日故障次数
	nBool			manStop;					//人工停止标志
	nBool			initFlag;					//初始化标志
	nUInt32			routeNum;					//与GROUP相连的路径个数
	nInt32			routes[DAC_ROUTE_IN_GROUP];	//与GROUP相连的路径号
	nInt32			mainRoute;					//主路径号
	DAC_COMMAND		txCmd;						//发送的命令
	DAC_COMMAND		rxCmd;						//接收的命令
}DAC_GROUP_INFO;
//ROUTE信息
typedef struct _dac_route_info_
{
	nUInt8			state;						//状态
	nUInt32			lastUpTime;					//最近一次由DOWN->UP的时间
	nUInt32			lastDownTime;				//最近一次由UP->DOWN的时间
	nUInt32			lastDataOkTime;				//最近一次数据正确时刻
	nUInt32			txFrmCnt;					//发送帧数
	nUInt32			rxFrmCnt;					//接收帧数
	nUInt32			okFrmCnt;					//接收的正确帧数
	nUInt32			errFrmCnt;					//接收的错误帧数
	nUInt32			errTime;					//误码率高的持续时间
	nUInt32			dayOkTime;					//日运行时间
	nUInt32			dayErrCnt;					//日故障次数
	nBool			manStop;					//人工停止标志
	nBool			switchFlag;					//由规约设置，用于一个通道多条路径情况(end+timeout+err)
	nBool			delProtocol;				//销毁通讯规约对象标志
	nBool			initFlag;					//初始化标志
	nBool			mainFlag;					//主路径标识
	DAC_COMMAND		cmd;						//命令信息
}DAC_ROUTE_INFO;
//CHAN变化信息
typedef	struct _dac_chg_chan_info_ 
{
	nUInt32				no;
	DAC_CHANNEL_INFO	info;
}DAC_CHG_CHANNEL_INFO;
//GROUP变化信息
typedef	struct _dac_chg_group_info_
{
	nUInt32			no;
	DAC_GROUP_INFO	info;
}DAC_CHG_GROUP_INFO;
//ROUTE变化信息
typedef struct _dac_chg_route_info_ 
{
	nUInt32			no;
	DAC_ROUTE_INFO	info;
}DAC_CHG_ROUTE_INFO;
//YC原始数据——模拟量
typedef struct _dac_yc_data_
{
	nFloat		val;							//值
	nUInt32		quality;						//质量码
	nUInt32		updateTime;						//最后更新时间（0：无效）
}DAC_YC_DATA;
//YX原始数据——数字量
typedef struct _dac_yx_data_
{
	nUInt8		val;							//值
	nUInt32		quality;						//质量码
	nUInt32		updateTime;						//最后更新时间（0：无效）
}DAC_YX_DATA;
//KWH原始数据——累积量
typedef struct _dac_kwh_data_
{
	nDouble		val;							//值
	nUInt32		quality;						//质量码
	nUInt32		updateTime;						//最后更新时间（0：无效）
}DAC_KWH_DATA;
//SOE数据
typedef struct _dac_soe_data_
{
	nDateTime	soeTime;						//时间
	nUInt32		no;								//序号
	nUInt8		val;							//值
	nUInt32		quality;						//状态（质量标志）
}DAC_SOE_DATA;
//变化YC
typedef	struct _dac_yc_chg_data_
{
	nUInt32			no;							//序号
	nFloat			lastVal;					//上一次值
	DAC_YC_DATA		data;						//数据
}DAC_YC_CHG_DATA;
//变化YX
typedef	struct _dac_yx_chg_data_
{
	nUInt32			no;							//序号
	nUInt8			lastVal;					//上一次值
	DAC_YX_DATA		data;						//数据
}DAC_YX_CHG_DATA;
//变化KWH
typedef	struct _dac_kwh_chg_data_
{
	nUInt32			no;							//序号
	nDouble			lastVal;					//上一次值
	DAC_KWH_DATA	data;						//数据
}DAC_KWH_CHG_DATA;
//-------------------------------------------------------------------------------
//(6)接收报文缓存
typedef struct _dac_recv_buffer_
{ 
	nUChar			data[DAC_RECV_BUFSIZE];		//接收缓冲数据16k
	nUInt32			write;						//接收缓冲写指针  
	hUInt32			read;						//接收缓冲读指针 yff
}DAC_RECV_BUFFER;

//(7)发送报文缓存
typedef struct _dac_send_buffer_
{
	nUChar			data[DAC_SEND_BUFSIZE];		//发送缓冲数据
	nUInt32			write;						//发送缓冲写指针
	hUInt32			read;						//发送缓冲读指针 yff

}DAC_SEND_BUFFER;

//报文解析缓冲结构
typedef struct _dac_message_
{
	nDateTime		time;						//发送或接收时间　　　		　　　　 
	nUInt8			type;						//帧类型(如：0:发送1:接收)
	char			desc[DAC_DESC_LEN];			//帧描述
	nUChar			data[DAC_MSG_DATA_LEN];		//命令缓存
	nUInt32			length;						//命令长度
}DAC_MESSAGE;
//报文解析缓冲区 
typedef struct _dac_message_buffer_
{
	DAC_MESSAGE			message[DAC_MSG_NUM];
	hUInt32				write;							//写指针
}DAC_MESSAGE_BUFFER;

//公共参数
typedef	struct _dac_com_para_
{
	DAC_SYSTEM			system;							//系统参数
	DAC_STATION			station[DAC_STATION_NUM];		//厂站表
	DAC_SENDDEV			senddev[DAC_SENDDEV_NUM];		//监测点
	DAC_RECVDEV         recvdev[DAC_RECVDEV_NUM];		//接收装置
	DAC_CHANNEL			channel[DAC_CHANNEL_NUM];		//通道表
	DAC_GROUP			group[DAC_GROUP_NUM];			//数据组表
	DAC_ROUTE			route[DAC_ROUTE_NUM];			//路径表
	DAC_PROTOCOL		protocol[DAC_PROTOCOL_NUM];		//规约表
	DAC_YK				yk[DAC_YK_NUM];					//遥控表--与遥测不同，非no序
	DAC_YT				yt[DAC_YK_NUM];					//遥调表--与遥测不同，非no序
	DAC_YCCON			yccon[DAC_YK_NUM];				//测点常量表
}DAC_COM_PARA;
//公共信息
typedef	struct _dac_com_info_
{
	DAC_SYSTEM_INFO		system;							//系统参数
	DAC_CHANNEL_INFO	channel[DAC_CHANNEL_NUM];		//通道表
	DAC_GROUP_INFO		group[DAC_GROUP_NUM];			//数据组表
	DAC_ROUTE_INFO		route[DAC_ROUTE_NUM];			//路径表
	DAC_CHG_CHANNEL_INFO chgChannel[2*DAC_CHANNEL_NUM];	//通道变化信息缓冲区
	DAC_CHG_GROUP_INFO	chgGroup[2*DAC_GROUP_NUM];		//数据组变化信息缓冲区
	DAC_CHG_ROUTE_INFO	chgRoute[2*DAC_ROUTE_NUM];		//路径变化信息缓冲区
	hUInt32				chgChannelWrite;				//通道变化信息缓冲区写指针
	hUInt32				chgGroupWrite;					//数据组变化信息缓冲区写指针
	hUInt32				chgRouteWrite;					//路径变化信息缓冲区写指针
	DAC_RECV_BUFFER		recvBuffer[DAC_CHANNEL_NUM];	//接收报文缓冲区
	DAC_SEND_BUFFER		sendBuffer[DAC_CHANNEL_NUM];	//发送报文缓冲区
	DAC_MESSAGE_BUFFER	messageBuf[DAC_ROUTE_NUM];		//解析后报文缓冲区
	DAC_RECVDEV_INFO	recvdev[DAC_RECVDEV_NUM];		//接收装置表
}DAC_COM_INFO;
//---------------------------------------------------------------------------------------
//公共共享内存
typedef	struct _dac_shm_com_
{
	DAC_COM_PARA		para;						//参数
	DAC_COM_INFO		info;						//信息
	DAC_CTRL_CMD		ctrl[DAC_CTRL_NUM];			//控制命令缓冲区
	DAC_CTRL_CMD		ack[DAC_CTRL_NUM];			//控制命令返校区
}DAC_SHM_COM;

//当前的MAP信息
typedef	struct	_dac_map_info_
{
	hUInt32		num;								//当前开辟的路径数据区中对象个数
	hUInt32		routeNum;							//与数据组相连的路径个数
	hInt32		routes[DAC_ROUTE_IN_GROUP];			//与数据组相连的路径号
}DAC_MAP_INFO;
//YC共享内存
typedef	struct _dac_shm_yc_
{
	bool				lock[DAC_ROUTE_NUM];		//锁
	hUInt32				write[DAC_ROUTE_NUM];		//变化数据区写偏移量
	size_t				para[DAC_GROUP_NUM];		//参数区指针偏移
	size_t				data[DAC_ROUTE_NUM];		//数据区指针偏移
	size_t				chgData[DAC_ROUTE_NUM];		//变化数据区指针偏移
	DAC_MAP_INFO		mapInfo[DAC_GROUP_NUM];		//当前MAP信息
}DAC_SHM_YC;
//YX共享内存
typedef	struct _dac_shm_yx_
{
	bool				lock[DAC_ROUTE_NUM];		//锁
	hUInt32				write[DAC_ROUTE_NUM];		//变化数据区写偏移量
	size_t				para[DAC_GROUP_NUM];		//参数区指针偏移
	size_t				data[DAC_ROUTE_NUM];		//数据区指针偏移
	size_t				chgData[DAC_ROUTE_NUM];		//变化数据区指针偏移
	DAC_MAP_INFO		mapInfo[DAC_GROUP_NUM];		//当前MAP信息
}DAC_SHM_YX;
//KWH共享内存
typedef	struct _dac_shm_kwh_
{
	bool				lock[DAC_ROUTE_NUM];		//锁
	hUInt32				write[DAC_ROUTE_NUM];		//变化数据区写偏移量
	size_t				para[DAC_GROUP_NUM];		//参数区指针偏移
	size_t				data[DAC_ROUTE_NUM];		//数据区指针偏移
	size_t				chgData[DAC_ROUTE_NUM];		//变化数据区指针偏移
	DAC_MAP_INFO		mapInfo[DAC_GROUP_NUM];		//当前MAP信息
}DAC_SHM_KWH;
//SOE共享内存
typedef	struct _dac_shm_soe_
{
	bool				lock[DAC_ROUTE_NUM];		//锁
	hUInt32				write[DAC_ROUTE_NUM];		//写偏移量
	size_t				para[DAC_GROUP_NUM];		//参数区指针偏移
	size_t				data[DAC_ROUTE_NUM];		//数据区指针偏移
	DAC_MAP_INFO		mapInfo[DAC_GROUP_NUM];		//当前MAP信息
}DAC_SHM_SOE;

#endif //PDT_DAC_H_

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
#include "pasmacro.h"
//---------------------------------------------------------------------------------------

//高级计算任务
typedef struct _pas_task_
		{
			hBool		valid;									//有效标志
			hInt32		taskno;
			hBool		useflag;
			hChar		algcode[32];
			hUInt8		tasktype;
			CDateTime	ymdhms;
			hChar		para1[64];
			hChar		para2[64];
			hChar		para3[64];
			hChar		para4[64];
			hChar		para5[64];
		}PAS_TASK,*PAS_TASK_PTR;

//高级计算算法
		typedef struct _pas_algorithm_
		{
			hBool		valid;									//有效标志
			hChar		algcode[32];
			hChar		name[64];
			hChar		dllname[32];
			hChar		featuretable[32];
		}PAS_ALGORITHM,*PAS_ALGORITHM_PTR;

//高级计算算法参数
		typedef struct _pas_iopara_
		{
			hBool		valid;									//有效标志
			hChar		algcode[32];
			hInt32		id;
			hUChar		paratype;
			hUChar		meastype;
			hint32		measno;
			hChar		grpcode[32];
			hChar		pointcode[32];
			hChar		value[16];
			//add info
			hFloat		fval;
			hInt32		ival;
		}PAS_IOPARA,*PAS_IOPARA_PTR;


//=======================================================================================
//系统信息
typedef	struct _pas_system_info_
{
	hBool			serverFlag;					//是否是PAS服务器
	hUInt32			algorithmNum;				//系统实际算法个数
	hUInt32			taskNum;					//系统实际任务个数
	hUInt32			loadParaFlag;				//装载参数表标志，根据位不同区分参数表
}PAS_SYSTEM_INFO;

//算法信息
typedef struct _dac_algorithm_info_
{
	nUInt8			state;						//状态（连接、断开）
	nUInt32			lastCalcTime;				//最近一次计算时间
	nUInt32			lastUsedTime;				//最近一次计算消耗时间
	nUInt32			txYkCnt;					//发送遥控指令数
	nUInt32			rxYtCnt;					//接收遥调指令数
	nUInt32			dayOkTime;					//日运行时间
	nUInt32			dayErrCnt;					//日故障次数
	nUInt8			reset;						//复位标志
	nBool			manStop;					//人工停止标志
	nUInt32			routeNum;					//与算法相关的参数个数
	nInt32			routes[PAS_PARA_IN_ALGORITHM];//与算法相关的参数组号
}DAC_ALGORITHM_INFO;

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
	DAC_COMMAND		txCmd;						//发送的命令
	DAC_COMMAND		rxCmd;						//接收的命令
}DAC_GROUP_INFO;

//公共参数
typedef	struct _dac_com_para_
{
	DAC_SYSTEM			system;							//系统参数
	DAC_STATION			station[DAC_STATION_NUM];		//厂站表
	DAC_CHANNEL			channel[DAC_CHANNEL_NUM];		//通道表
	DAC_GROUP			group[DAC_GROUP_NUM];			//数据组表
	DAC_ROUTE			route[DAC_ROUTE_NUM];			//路径表
	DAC_PROTOCOL		protocol[DAC_PROTOCOL_NUM];		//规约表
	DAC_YK				yk[DAC_YK_NUM];					//遥控表--与遥测不同，非no序
	DAC_YT				yt[DAC_YK_NUM];					//遥调表--与遥测不同，非no序
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
	size_t				data[DAC_ROUTE_NUM];		//数据区指针偏移
	DAC_MAP_INFO		mapInfo[DAC_GROUP_NUM];		//当前MAP信息
}DAC_SHM_SOE;

#endif //PDT_DAC_H_

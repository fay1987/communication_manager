/*==============================================================================
* 文件名称 : webdef.h
* 模块 : web定义
* 创建时间 : 2009-10-20 15:55:23
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
* 注意：本文件使用的时间格式统一为：距1970年秒数(UTL时间)
==============================================================================*/
#ifndef	_BFS_WEB_DEF_H_
#define	_BFS_WEB_DEF_H_

#include "utl/datatype.h"
#include "dac/dacmacro.h"

#define WEB_LOG_MAX_NUM			4096		//日志缓冲区最大个数
#define WEB_LOG_TYPE_NUM		50000		//日志最大类型号
#define WEB_LOG_DATA_LEN		1024		//日志内容长度
#define WEB_EVENT_LEN			256			//事项内容长度
#define WEB_CTRL_COMMOND_LEN	200			//控制命令长度
#define BUFlEN_ACK				128			//蓄电池ack buf长度 (cye add at 20100420)
#define WEB_CODE_LEN			32
#define WEB_NAME_LEN			64
#define WEB_DESCR_LEN			256

//遥测数据结构
typedef struct _web_yc_data_ 
{
	int				no;
	float			val;			//值
	int				quality;		//质量码
	int				updateTime;		//距离1970年的秒数
}WEB_YC_DATA;

//遥信数据结构
typedef struct _web_yx_data_ 
{
	int				no;
	int				val;			//值
	int				quality;		//质量码
	int				updateTime;		//距离1970年的秒数
}WEB_YX_DATA;

//报文解析缓冲结构
typedef struct _web_message_
{
	hUInt32			time;						//发送或接收时间(距离1970年的秒数)　		　　　　 
	hUInt8			type;						//帧类型(如：0:发送1:接收)
	char			desc[DAC_DESC_LEN];			//帧描述
	hUChar			data[DAC_MSG_DATA_LEN];		//缓存
	hUInt32			length;						//长度
}WEB_MESSAGE;

//通道状态
enum	WEB_CHANNEL_STATE
{
	WEB_CHANNEL_STATE_DOWN		=	0,			//断开
	WEB_CHANNEL_STATE_LISTEN	=	1,			//监听
	WEB_CHANNEL_STATE_UP		=	2,			//链接
	WEB_CHANNEL_STATE_DISABLED	=	3,			//禁用
};

//通道信息
typedef struct _web_channel_info_
{
	hUInt16			state;						//状态（连接、断开）
	hUInt32			lastUpTime;					//最近一次由DOWN->UP的时间
	hUInt32			lastDownTime;				//最近一次由UP->DOWN的时间
	hUInt32			txByteCnt;					//发送字节数
	hUInt32			rxByteCnt;					//接收字节数
	hUInt32			dayOkTime;					//日运行时间
	hUInt32			dayErrCnt;					//日故障次数
	hUInt32			routeNum;					//与通道相连的路径个数
}WEB_CHANNEL_INFO;

//路径状态
enum	WEB_ROUTE_STATE
{
	WEB_ROUTE_STATE_DOWN		=	0,			//停止
	WEB_ROUTE_STATE_UP			=	1,			//运行
	WEB_ROUTE_STATE_STANDBY		=	2,			//热备
	WEB_ROUTE_STATE_FREE		=	3,			//空闲
	WEB_ROUTE_STATE_DISABLED	=	4,			//禁用
};

//ROUTE信息
typedef struct _web_route_info_
{
	hUInt16			state;						//状态
	hUInt32			lastUpTime;					//最近一次由DOWN->UP的时间
	hUInt32			lastDownTime;				//最近一次由UP->DOWN的时间
	hUInt32			lastDataOkTime;				//最近一次数据正确时刻
	hUInt32			txFrmCnt;					//发送帧数
	hUInt32			rxFrmCnt;					//接收帧数
	hUInt32			okFrmCnt;					//接收的正确帧数
	hUInt32			errFrmCnt;					//接收的错误帧数
	hUInt32			errTime;					//误码率高的持续时间
	hUInt32			dayOkTime;					//日运行时间
	hUInt32			dayErrCnt;					//日故障次数
}WEB_ROUTE_INFO;

//event
typedef struct _web_event_
{
	hUInt16		sort;							//事件分类号
	hUInt16		type;							//事件类型
	hUInt32		updateTime;						//时间(UTL)
	hUInt8		security;						//安全级
	hUInt8		volLevel;						//电压等级
	nUInt16		stateValue;						//当前状态值
	nUInt16		stateValue1;					//上一次状态
	nFloat		floatValue;						//当前浮点值
	nFloat		floatValue1;					//上一次浮点值
	nUInt8		isRevert;						//是否复归(yc,yx)
}WEB_EVENT;

//电压合格率命令类型
enum CMD_TYPE
{
	CMD_CALL_HOUR		=	0,		//整点补采
	CMD_CALL_DAY_STA,				//日统计补采
	CMD_CALL_DAY_STA_XB,			//日谐波统计补采
	CMD_CALL_MONTH_STA,				//月统计补采
	CMD_CALL_MONTH_STA_XB,			//月谐波统计补采
	CMD_CALL_VERSION,               //电压表版本(add lj20100106)
	CMD_SET_HOUR		=	10,		//整点设置
	CMD_SET_DAY_STA,				//日统计设置
	CMD_SET_DAY_STA_XB,				//日谐波设置
	CMD_SET_MONTH_STA,				//月统计设置
	CMD_SET_MONTH_STA_XB,			//月谐波统计设置
	CMD_SET_RESTART,                //重启（add lj20100106）
	CMD_SET_VOLLEVEL,               //设置电压等级&上下限（add lj20100106）
	CMD_SET_SYNCDATA,				//数据同步
	CMD_SET_TIME					//校时
};

//命令头
typedef struct volhead
{
	hUInt32 type;					//命令类型--具体参看CMD_TYPE
	hUInt32 cmdtime;				//命令中的时间参数，如7-1的日统计，时间格式为距1970年秒数
	hUInt32 luNo;					//命令对应的序号/路号
}VOLHEAD;

//反校结果
typedef struct volack
{
	hUInt8 state;					//0--失败;1--成功
}VOLACK;

//统计设置命令结构
typedef struct volsta	//电压合格率统计值
{
	hFloat	valavg;					//平均值
	hFloat	valmax;					//电压最大值
	hFloat	valmin;					//电压最小值
	hUInt32 maxvoltime;				//最大值出现的时刻
	hUInt32 minvoltime;				//最小值出现的时刻
	hFloat	checkout_qty;			//合格率
	hUInt16 checkout_time;			//合格时间
	hFloat	up_qty;					//超上限率
	hUInt16 up_time;  				//超上限时间
	hFloat	low_qty;				//超下限率
	hUInt16 low_time;				//超下限时间
	hUInt16 total_time;				//统计时间
}VOLSTA;

//谐波数据结构
typedef struct volsta_xb
{
	hFloat	xbavg;					//谐波畸变率平均值
	hFloat	xbmax;					//谐波畸变率最大值
	hUInt32	maxxbtime;				//最大值出现的时刻
	hFloat	xbhgl;					//谐波畸变率合格率
	hFloat	xbuptime;				//谐波畸变率合格率
}VOLSTA_XB;

//电压等级
typedef struct vol_level		
{
	hUInt32	level;					//电压等级(V)
	hInt16	upLimit;				//上限值(如:7代表level*(1+%7))
	hInt16	lowLimit;				//下限值(如:-7代表level*(1-%7))
}VOL_LEVEL;

//目前只支持一次设置一个点
typedef struct vol_hour
{
	hFloat	hourval;				//整点值
}VOL_HOUR;


//---------------------------------------------------------
//蓄电池采集设备(DJX&BMM&采集板)命令类型
enum BTY_CMD_TYPE	
{
	//控制命令
	BTY_MAINTAIN_PLUS			=	0,		//脉冲维护
	BTY_MAINTAIN_DISCHARGE		=	1,		//放电维护
	BTY_MAINTAIN_CHARGE			=	2,		//充电维护
	BTY_MAINTAIN_NORMAL			=	3,		//普通维护

	BTY_DISCHARGE_DYNAMIC		=	6,		//动态放电
	BTY_DISCHARGE_STATIC		=	7,		//静态放电
	BTY_DISCHARGE_STATIC_END	=	8,		//结束静态放电
	BTY_TEST_RES				=	9,		//内阻测试
	BTY_MAINTAIN_SYSTEM_END		=	10,		//停止系统维护
	BTY_TEST_NULLOPEN			=	11,		//测试空开
        BTY_DISCHARGE_SYSPARAM                  = 12,             //设置放电参数 黄丽英 2011-6-3

	//召唤命令
	BTY_CALL_SYSPARAM			=	20,		//获取系统参数（浮点数）
	BTY_CALL_SERRATE			=	21,		//读取浮充阀值（浮点数）
	BTY_CALL_FULLVOL			=	22,		//读取维护时浮充电压值（浮点数）
	BTY_CALL_BENCHMARKRES		=	23,		//读取基准内阻值
	BTY_CALL_RES				=	24,		//内阻采集	
	BTY_CALL_VOL				=	25,		//电压采集

	//设置命令
	BTY_SET_SYSPARAM			=	50,		//设定系统参数（浮点数）
	BTY_SET_SERRATE				=	51,		//设定浮充阀值（浮点数）
	BTY_SET_FULLVOL				=	52,		//设定维护时浮充电压值（浮点数）
	BTY_SET_RULERELVALUES		=	53,		//设定基准内阻值（浮点数）

	//维护命令
	BTY_COLL_RES				=	60,		//启动一次内阻采集命令
	BTY_COLL_VOL				=	61,		// 启动一次电压采集
	BTY_STR_SYS					=	62,		// 启动一次系统维护
	BTY_STP_SYS					=	63		// 停止一次系统维护
};

//蓄电池采集设备(DJX&BMM&采集板)参数类型
enum BTY_PARAM_TYPE
{
	BTY_PARAM_VOL_UP		=	0,			//电压上限 
	BTY_PARAM_VOL_LOW,						//电压下限
	BTY_PARAM_TEMP_UP,						//温度上限
	BTY_PARAM_TEMP_LOW,						//温度下限
	BTY_PARAM_CHARGECUR,					//充电电流上限
	BTY_PARAM_GROUPVOL_UP,					//组端电压上限
	BTY_PARAM_GROUPVOL_LOW,					//组端电压下限
	BTY_PARAM_RES_UP,						//单节内阻上限
	BTY_PARAM_NUM,							//电池节数 
	BTY_PARAM_GROUPNUM,						//监测蓄电池组数
	BTY_PARAM_CURMEASURE					//电流量程
};

typedef struct btyhead
{
	hUInt32		type;						//命令类型
	hUInt32		btyGroupNo;					//蓄电池组号
	hUInt32		btyNo;						//蓄电池节号
}BTYHEAD;

typedef struct btydischarge
{
	hUInt32		time;						//放电时长，分钟为单位
	hUInt32		capacity;					//放出容量
	float		stopvol;					//结束电压
	float		stopgrpvol;					//结束组端电压
	float		currency;					//电流
	float		remainingCapacity;			//剩余容量
	float		remainingTime;				//剩余时间

}BTYDISCHARGE;

typedef struct btyparam
{
	hUInt32		paraType;					//参数类型
	hUChar		groupNo;					//电池组号
	float		volUp;						//电压上限 
	float		volLow;						//电压下限
	float		tempUp;						//温度上限
	float		tempLow;					//温度下限
	float		dischargeCurUp;				//充电电流上限
	float		groupVolUp;					//组端电压上限
	float		groupVolLow;				//组端电压下限
	float		oddResUp;					//单节内阻上限
	hUInt32		btyNum;						//电池节数 
	float		curMeasure;					//电流量程
}BTYPARAM;

typedef struct	btyvalue
{
	float	  val;
}BTYVALUE;

typedef struct	btymatain
{
	hUInt32	  maintainType;
}BTYMATAIN;

typedef struct btyack
{
	hUInt8		state;			//0--失败;1--成功
	char        buff[BUFlEN_ACK];
}BTY_ACKCMD;

//---------------------------------------------------------
//无功补偿

//命令头
typedef struct cmd_wgbchead
{
	hUInt32 type;
	hUInt32 cmdtime;		//距1970年秒数
}CMD_WGBCHEAD;

//反校结果
typedef struct wgbcack
{
	hUInt8 state;			//0--失败;1--成功
}WGBCACK;

enum WGBC_CMD_TYPE
{
	CMD_WGBCSETTIME,						//校时
	CMD_WGBCSETSETTING,						//设置整定值
	CMD_WGBCACTIONCAP,						//遥控动作电容
	CMD_WGBCRESET,							//复位
	CMD_WGBCGETTIME							//获设备时间命令(added by phs 20100714)
};

enum ACTIONMODE
{
	VOLTAGE = 1,						//电压
	TIME,								//时间
	VOLTIME,							//电压时间
	POWERFACTOR,						//功率因数
	REACTIVE							//电压无功
};

enum CTL_OPTION
{
	noOperate,							//无操作
	clearTotalTimes,					//清除动作累计次数
	clearRunTime						//清除电容器累计投运时间
};

enum ACTIONCAP_MODE						//动作电容动作方式
{
	ACTIONCAP_INPUT,
	ACTIONCAP_CUTOFF
};
typedef struct actionCap_web  
{
	char	no;							//电容组数编号
	char	mode;						//动作方式
}ACTIONCAP_WEB;
//高压无功控制器整定值web中的结构
typedef struct wgbcSet_web     
{
	hUInt16	volLevel;					//电压等级
	float	volAmend;					//电压修正
	hUInt16 lineCurrentRatio;       	//线路电流变比
	hUInt16 capCurrentRatio;	    	//电容器电流变比
	float	currentRatioAmend_a;		//电流比矫正a
	float   currentRatioAmend_b;		//电流比矫正b
	float	currentRatioAmend_c;		//电流比矫正c
	float   currentRatioAmend_d;		//电流比矫正d
	float	inputLimit;					//投入门限
	float   cutoffLimit;				//切除门限
	float   inputTime_1;				//投入时间1
	float	cutoffTime_1;				//切除时间1
	float	inputTime_2;				//投入时间2
	float	cutoffTime_2;				//切除时间2
	float   overCurrentSet;				//过流设定
	float   overCurrentTimeSet;			//过流持续时间设定
	float	quickBreakSet;				//速断设定
	float   quickBreakTimeSet;			//速断持续时间设定
	float	underVoltageSet;            //欠压设定
	float   underVolTimeSet;  		    //欠压持续时间设定
	float	overVoltageSet;             //过压设定
	float   overVolTimeSet;  		    //过压持续时间设定
	hUInt16 switchRestrictions;			//日投切限制次数
	hUInt16	actionMode;					//动作方式选择
	float	switchAffect;		        //投切影响值
	hUInt16	controllerOption;	        //控制器处理选择
	float   lowPowerFactor;				//功率因素下限
	float   upPowerFactor;              //功率因素上限
	float 	reactivePowerBacklash;		//无功功率回差
	float   lowReactivePower;			//无功功率下限
	float	capCapacity;				//1组电容器容量
	hUInt16 protectionType;  			//保护类型
	//hUInt16	reservedByte;			//保留字
}WGBCSET_WEB;


/***********************************************************/
//门禁
typedef struct doorvalue
{
	hUInt16	 syspass;                    //系统密码
    hUInt32  keypass;                   //键盘密码
	hUInt8   msddqzx;                    //门口机门锁继电器执行时间
    hUInt8   ddjrys;                    // 门口机开门后等待进入的延时时间
	hUInt8   hwbjsc;                     //门口机红外报警输出的被确认时间
	hUInt8   bdbjys;                     //设定门口机本地报警的延时时间
    hUInt8   hwjchddys;                  //红外监控后的等待延时
    hUInt8	 sfjkmkg;                   //是否监控门开关状态
	hUInt8   sfjkhw;                    //是否监控红外
    hUInt8   mcszds;                       //电磁锁在断电后（或加电后）自动锁上
	hUInt8   scxyjdxdj;                  //输入线与地线短接是否能开门
	hUInt8   mcscdpzt;                  //门磁输出电平状态
    hUInt8   hwgyqscdpzt;              //红外感应器输出电平状态
	hUInt8   skhfkm;                    //刷卡合法能开门
	hUInt8   ysjpentkm;                 //允许键盘按ENT开门
	hUInt8   week1;						//星期内休息日一
    hUInt8   week2;						//星期内休息日二
    hUInt8   holidaymonth;				 //节假日(月)
    hUInt8   holiday;                  //节假日(日)
	hUInt8   opendoor;                 //远程开门     
    hUInt8   defaultopen;			   //将准进时段、管理时段设置成缺省值  0:置所有时段无效,1: 设//置工作日准进时段：00：00—23：59全天能进等


}DOORVALUE;

enum DOOR_CMD_TYPE	
{
	ORDER_DOORPASS       = 1,  //修改SM的访问密码
	SET_DEFAULTTIME      = 2,  //将准进时段、管理时段设置成缺省值
	SET_ALLTIMEINVALD    = 3,  // 置所有时段无效
	SET_OPENTIME         = 4,  // 设置工作日的准进时段
	SET_MSDDQZX          = 5,  // 门口机门锁继电器执行时间(单独设定)
	SET_DDJRYS           = 6,  // 门口机开门后等待进入的延时时间(单独设定)
	SET_HWBJSC           = 7,  // 门口机红外报警输出的被确认时间(单独设定)
	SET_BDBJYS           = 8,  // 设定门口机本地报警的延时时间(单独设定)
	SET_HWJCHDDYS        = 9,  // 门口机开启红外监控后的等待延时
	SET_SFJKHW           = 10, // 门口机红外监控的布防与撤防
	SET_SFJKMKG          = 11, // 门口机的门开关状态监控的布防与撤防
	SET_WEEK             = 12, //设定门口机星期内的休息日
	SET_ADDHOLIDAY       = 13, // 设定门口机国家法定节假日（不包括星期内休息日）
	SET_DELHOLIDAY       = 14, // 删除门口机内的一组节假日
	SET_DELALLHOLIDAY    = 15, //清空门口机的节假日列表(全部删除)
	SET_OPEN             = 16, //单一放行(不带系统操作员信息)
	SET_DOORADDR         = 17, // 设置门口机地址的命令
	SET_ADDUSER          = 18, // 增加一个用户（至门口机）
	SET_DELUSERCARD      = 19, //删除用户卡（从门控制器内）
	SET_DELUSERID        = 20, //删除用户编号（ID）（从门控制器内）
	SET_DELALLUSER       = 21,  //全部删除用户（从门控制器内）
	SET_NOWORKOPEN		 = 31,	//设置非工作日准进时间段
	SET_MCHWGYQ			 = 32	//门口机门磁、红外等感应器的特性（单独设定）
		//yhh20110512 去掉一个逗号
	//READ_WORKOPENTIME    = 22, //yhh20110512 重复
};
typedef struct opentime            //准进时间段设置
{
	hUInt8   isworkday;                     //0是非工作日，是工作日
	hUInt8   groupno;						//组号        
	hUInt8	beginhour1;						//开始时间段
	hUInt8	beginmin1;						//开始时间段
	hUInt8	endhour1;						//结束时间段
	hUInt8	endmin1;						//结束时间段
	hUInt8	beginhour2;						//开始时间段
	hUInt8	beginmin2;						//开始时间段
	hUInt8	endhour2;						//结束时间段
	hUInt8	endmin2;						//结束时间段
	hUInt8	beginhour3;						//开始时间段
	hUInt8	beginmin3;						//开始时间段
	hUInt8	endhour3;						//结束时间段
	hUInt8	endmin3;						//结束时间段
	hUInt8	beginhour4;						//开始时间段
	hUInt8	beginmin4;						//开始时间段
	hUInt8	endhour4;						//结束时间段
	hUInt8	endmin4;						//结束时间段
}OPENTIME;

typedef struct userparam				//用户参数表
{
	unsigned char cardid[16];				//卡片编号
	hUInt32 userid;							//用户编号ID        
	hUInt16 userpass;                       //密码
	hUInt16	year;							//有效期年
	hUInt8	month;							//有效期月
	hUInt8	day;							//有效期日
	hUInt8  permission;						//权限
}USERPARAM;

//命令头
typedef struct doorhead
{
	hUInt8  grpno;                      //数据组序号
	hUInt8  dooraddr;                        //门口机地址
	hUInt32 type;                          //命令类型
}DOORHEAD;

//反校结果
typedef struct doorack
{
	hUInt8 state;			//0--失败;1--成功
	char        buff[BUFlEN_ACK];
}DOORACK;
//--------------------------------------------------------------------------

//普莱德命令类型
enum PLD_CMD_TYPE
{
	CMD_WRITERUN	  =0,		//写运行状态
	CMD_START         =1,		//恢复初始化
	CMD_RESET         =2,		//软件强制复位
	CMD_WRITEPARA     =3,		//所有值设置
	CMD_READPARA      =4		//读参数值
	
};

//命令头
typedef struct xdc_head
{
	hUInt8  grpno;                      //电池组号
	hUInt8  singleno;                   //电池节号
        int CmdNo;			    //设置的参数序号
}XDC_HEAD;
// 当电池节号为0时表示广播命令，对整组进行设置（从机地址不可以整组一次设置），不可以整组一次读取

//反校结果
typedef struct pldack
{
	hUInt8 state;			//0--失败;1--成功
	char        buff[BUFlEN_ACK];
}PLDACK;

//普莱德蓄电池
typedef struct Remotecontrol			//遥控参数
{
	int setval;				//设置参数值

}REMOTECONTROL;
/*当命令号为0时：
  Setval = 0   均压
         = 1   测内阻
         = 2   待机
         = 3   核容
当命令号为1时：
Setval = 1   恢复初始化 
当命令号为2时：
Setval = 1   软件强制复位 */


typedef struct Setvalue	//遥调参数					
{
	int  Address;						//从机地址
	int  BaudRate;						//波特率选择
	int  ADVoltagemin;					//AD 电压最低参考值
	int  ADVoltagemax;					//AD 电压最高参考值
	int ADTemperaturemin;					//AD 温度最低参考值
	int ADTemperaturemax;					//AD 温度最高参考值
	int ADIdlemin;						//AD 空闲最低参考值
	int ADIdlemax;						//AD 空闲最高参考值
	int jyqwdy;						//均压期望电压
	int cnzfddlkzdy;					//测内阻放电电流控制电压
	int cnzfdsj;						//测内阻放电时间
	int jyfdzdz;						//均压放电最大值
	int wdbjsx;						//温度报警上限
	int hrfddlkz;						//核容放电电流控制电压
	int hrfdxxz;						//核容放电下限值
}SETVALUE;



#endif

#ifndef	_BFS_EVENTTYPE_
#define	_BFS_EVENTTYPE_

enum	EventSort
{
	EVENT_SORT_SYS=1,
	EVENT_SORT_OPT,
	EVENT_SORT_DEVICE_OPT,
	EVENT_SORT_RUN,
	EVENT_SORT_YC,
	EVENT_SORT_YX,
	EVENT_SORT_SOE
};
enum	EventType
{
	EVENT_UNKOWN=0,			//未定义

	//系统事项（1——100）
	EVENT_SYS=1,
	EVENT_SYS_SERVER=2,		//服务器
	EVENT_SYS_HOST,			//主机
	EVENT_SYS_PROC,			//进程
	EVENT_SYS_MEM,			//内存
	EVENT_SYS_CPU,			//CPU
	EVENT_SYS_DISK,			//DISK
	EVENT_SYS_MAIL,			//邮件
	EVENT_SYS_FILE,			//文件

	//操作事项（100——199）
	EVENT_OPT = 100,
	EVENT_OPT_SERVER,		//服务器
	EVENT_OPT_HOST,			//主机
	EVENT_OPT_PROC,			//进程
	EVENT_OPT_FILE,			//文件
	EVENT_OPT_PARA,			//装载参数
	EVENT_OPT_CHANNEL,		//通道
	EVENT_OPT_GROUP,		//数据组
	EVENT_OPT_ROUTE,		//路径
	EVENT_OPT_YC,			//YC
	EVENT_OPT_YX,			//YX
	EVENT_OPT_KWH,			//KWH
	EVENT_OPT_PALN,			//PLAN
	EVENT_OPT_YK,			//yaokong
	EVENT_OPT_YT,			//yaotiao

	//设备操作事项(200--299)
	EVENT_DEVICE_OPT=200,	//普通设备操作事项
	EVENT_DEVICE_OPT_DTFD,	//静态放电
	EVENT_DEVICE_OPT_JTFD,	//动态放电

	//运行事项(300-399)
	EVENT_RUN=300,			//普通运行事项
	EVENT_RUN_NET,			//网络事项
	EVENT_RUN_SERVER,		//服务器事项
	EVENT_RUN_PROC,			//进程事项
	EVENT_RUN_CHANNEL,		//通道事项
	EVENT_RUN_GROUP,		//数据组事项
	EVENT_RUN_ROUTE,		//路径事项

	//YC事项(500-599)
	EVENT_YC=501,			//YC越限
	EVENT_YC_I,             //电流不合格
	EVENT_YC_U,             //电压不合格
	
	

	//YX事项(1000-1999)
	EVENT_YX=1000,			//YX普通事项
	EVENT_YX_GZZT=1001,		//故障状态
	EVENT_YX_YCZT,			//异常状态
	EVENT_YX_KGBW=1010,		//开关正常变位
	EVENT_YX_KGBH,			//开关开闭状态
	EVENT_YX_KGSG,			//开关事故变位
	EVENT_YX_DZBW,			//刀闸变位
	//电压状态
	EVENT_YX_DYZT=1020,		//电压状态
	EVENT_YX_DYQY,			//电压欠压状态
	EVENT_YX_DYGY,			//电压过压状态
	EVENT_YX_DYGQY,			//电压过欠压状态
	//电池状态
	EVENT_YX_DCZT=1030,		//电池状态
	//设备状态
	EVENT_YX_SBZW=1040,		//设备在位状态
	EVENT_YX_SBTQ,			//设备切投状态
	
	EVENT_YX_YXZT=1050,		//运行状态

	EVENT_YX_KGJZT=1052,	//开关机状态
	EVENT_YX_DZZT,			//动作状态

	//交流
	EVENT_YX_JLSD=1060,		//交流失电
	EVENT_YX_JLQX,			//交流缺相
	EVENT_YX_RDQZT,			//熔断器状态
	EVENT_YX_XLJD,			//线路接地状态
	EVENT_YX_XLSY,			//线路使用状态

	EVENT_YX_SDZT=2000,		//市电状态



	//数据库事项（4000——4999）
	EVENT_HISDB = 4000,

	//实时库事项（5000——5999）
	EVENT_MEMDB = 5000,

	//控制服务器事项（6000——6999）
	EVENT_CTLSRV = 6000,

	//转换文件通知事项
	EVENT_TRANS_FILE = 10000,

	//通用事件（30000——30999）
	EVENT_NORMAL = 30000
};

#endif

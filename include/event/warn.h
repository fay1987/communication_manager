#ifndef __WARN_WARN_H__
#define	__WARN_WARN_H__

//告警对象类型定义
#define	WARN_OBJ_TYPE_YC		0		//遥测点
#define	WARN_OBJ_TYPE_YX		1		//遥信点
#define	WARN_OBJ_TYPE_KWH		2		//电度点
#define	WARN_OBJ_TYPE_GROUP		3		//数据组（如：数据组状态）
#define	WARN_OBJ_TYPE_ROUTE		4		//路径（如：路径状态）
#define	WARN_OBJ_TYPE_CHANNEL	5		//通道（如：通道状态）
#define	WARN_OBJ_TYPE_SERVICE	6		//服务（如：服务启动、主备服务切换等）
#define	WARN_OBJ_TYPE_PROCESS	7		//进程（如：进程启动、退出等）
#define	WARN_OBJ_TYPE_HOST		8		//主机（如：硬盘、CPU、内存状况等）
#define	WARN_OBJ_TYPE_SYSTEM	9		//系统（如：平台网络状况等）

//告警等级
#define	WARN_LEVEL_WARNNING		0		//普通告警
#define WARN_LEVEL_ERROR		1		//严重告警
#define	WARN_LEVEL_FATAL		2		//致命告警

//条件类型
#define WARN_CONDICTION_TYPE_INVALID			0		//无效
#define WARN_CONDICTION_TYPE_BATTERYSTATE		1		//蓄电池状态
#define WARN_CONDICTION_TYPE_OTHER				2		//其它输入值

//蓄电池状态
#define	WARN_BATTERY_STATE_CHARGE_FLOAT			0		//浮充（运行监测状态）
#define	WARN_BATTERY_STATE_CHARGE_EQUAL			1		//均充（充电状态）
#define	WARN_BATTERY_STATE_DISCHARGE_DYNAMIC	2		//动态放电（人为）
#define	WARN_BATTERY_STATE_DISCHARGE_STATIC		3		//静态放电（人为，核对性放电）
#define	WARN_BATTERY_STATE_BREAK				4		//开路
#define	WARN_BATTERY_STATE_HANG					5		//悬空
#define	WARN_BATTERY_STATE_DISCHARGE_AUTO		6		//自放

#endif

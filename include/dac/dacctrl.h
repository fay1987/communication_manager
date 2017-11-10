/*==============================================================================
* 文件名称 : fdcctrl.h
* 模块 : 前置控制接口
* 创建时间 : 2008-09-16 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_DACCTRL_H_
#define	_PDT_DAC_DACCTRL_H_

#include <ace/config-all.h>
//=============================================================================
#if defined (_DAC_CTRL_)
#  define	DACCTRL_EXPORT	ACE_Proper_Export_Flag
#else
#  define	DACCTRL_EXPORT	ACE_Proper_Import_Flag
#endif	// _DAC_CTRL_
//=============================================================================
#include "utl/datatype.h"
#include "dac/dacdef.h"
#include "dacmacro.h"
#include "dac/comminf.h"
#include "dac/ctrlinf.h"
#include "dac/dacctrldef.h"

namespace PDT
{
	class DACCTRL_EXPORT CDacCtrl
	{
	public:
		CDacCtrl();
		~CDacCtrl();

		//----------------参数加载-------------------------------
		//根据表名等加载参数:tablName--表名;index1--一级索引;index2--二级索引
		bool	load(const char* tableName,const char* index1=0,const char* index2=0);
		//加载所有参数
		bool	loadAll();
		//加载厂站参数
		bool	loadStation();
		//加载设备
		bool	loadDevice();
		//加载设备类型
		bool	loadDeviceType();
		//加载设备参数表
		bool	loadDevicePara();
		//加载通道参数
		bool	loadChannel();
		//加载数据组参数
		bool	loadGroup();
		//加载路径参数
		bool	loadRoute();
		//加载规约参数
		bool	loadProtocol();
		//加载遥测参数
		bool	loadYc(const char* group=0);
		//加载遥信参数
		bool	loadYx(const char* group=0);
		//加载电度参数
		bool	loadKwh(const char* group=0);
		//加载遥控表
		bool	loadYk();
		//加载遥调表
		bool	loadYt();

		//----------------通道控制-------------------------------
		//人工启动通道
		bool manStartChannel(const char* channelCode);
		//人工停止通道
		bool manStopChannel(const char* channelCode);
		//人工启用通道
		bool manEnableChannel(const char* channelCode);
		//人工禁用通道
		bool manDisableChannel(const char* channelCode);

		//----------------路径控制-------------------------------
		//人工启动路径
		bool manStartRoute(const char* routeCode);
		//人工停止路径
		bool manStopRoute(const char* routeCode);
		//人工启用路径
		bool manEnableRoute(const char* routeCode);
		//人工禁用路径
		bool manDisableRoute(const char* routeCode);
		//人工重启指定路径的规约
		bool manResetProtocol(const char* routeCode);

		//----------------数据组控制-----------------------------
		//人工启动数据组
		bool manStartGroup(const char* groupCode);
		//人工停止数据组
		bool manStopGroup(const char* groupCode);
		//人工启用数据组
		bool manEnableGroup(const char* groupCode);
		//人工禁用数据组
		bool manDisableGroup(const char* groupCode);

		//----------------规约下行指令----------------------------
		//复位RTU
		bool resetRtu(const char* groupCode);
		//对时
		bool syncTime(const char* groupCode);
		//总召唤
		bool callAllData(const char* groupCode);
		//总召遥测
		bool callAllDataYc(const char* groupCode);
		//总召遥信
		bool callAllDataYx(const char* groupCode);
		//总召电度
		bool callAllDataKwh(const char* groupCode);
		//分组召唤(某些规约需要)
		bool callSubgroupData(const char* groupCode,hInt32 subgroupNo);
		//遥控
		bool yk(const ctrl_pro_yk* pYk);
		//遥调
		bool yt(const ctrl_pro_yk* pYt);
		//直接输出
		bool directOutput(const ctrl_pro_direct_operate* pDirectOutput);
		//设点(local为true则设置本地内存数据，false则使用规约的设点功能，输出到RTU)
		bool setYc(const char* groupCode,const char* pointCode,hFloat value,bool local=true);
		bool setYx(const char* groupCode,const char* pointCode,hUInt8 value,bool local=true);
		bool setKwh(const char* groupCode,const char* pointCode,hFloat value,bool local=true);

		//----------------通用buf规约下行指令----------------------------
		bool commonCmd(const ctrl_pro_common* pCommon);
		bool commonCmdAck(const ctrl_pro_common_ack* pCommonAck);

		//---------------规约返校信息-------------------------------
		//遥控返校
		bool ykAck(const ctrl_pro_yk_ack* ykAck);

	private:
		CCommInf	m_commInf;
		CCtrlInf	m_ctrlInf;
	};
}

#endif	//_PDT_DAC_DACCTRL_H_

/*==============================================================================
* 文件名称 : comminf.h
* 文件功能 ：前置数据访问接口，支持C代码
* 模块 : 前置采集接口
* 创建时间 : 2017-03-31
* 作者 : wlq
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_COMMINF_C_H_
#define	_PDT_DAC_COMMINF_C_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef	struct _dac_system_c_
{
	char		name[64];				//名称
	char		desc[64];				//描述
}DAC_SYSTEM_C;

//系统信息
typedef	struct _dac_system_info_c_
{
	unsigned char			serverFlag;					//是否是DAC服务器
	unsigned int			stationNum;					//系统实际厂站数码
	unsigned int			channelNum;					//系统实际通道数目
	unsigned int			groupNum;					//系统实际数据组数目
	unsigned int			routeNum;					//系统实际路径数目
	unsigned char			initFlag;					//初始化标志
	unsigned int			loadParaFlag;				//装载参数表标志，根据位不同区分参数表
}DAC_SYSTEM_INFO_C;


DAC_SYSTEM_INFO_C*		systemInfo_c();



#ifdef __cplusplus
}
#endif

#endif	//_PDT_DAC_COMMINF_H_

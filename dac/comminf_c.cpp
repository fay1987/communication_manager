/*==============================================================================
* 文件名称 : comminf_c.cpp
* 模块 : 前置数据访问接口，支持C代码
* 创建时间 : 2017-03-31 15:55:23
* 作者 : www
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "dac/comminf.h"
#include "dac/comminf_c.h"


///////////////////////////参数///////////////////////////////////////////////
//DAC_SYSTEM_C*	system()
//{
//	PDT::CCommInf	comminf;
//	return (DAC_SYSTEM_C*)comminf.system();
//}

DAC_SYSTEM_INFO_C*		systemInfo_c()
{
	PDT::CCommInf	comminf;
	return (DAC_SYSTEM_INFO_C*)comminf.systemInfo();
}


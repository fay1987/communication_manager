/*==============================================================================
* 文件名称 : loadpara.h
* 模块 : 前置参数加载
* 创建时间 : 2009-06-28 15:55:23
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_LOADPARA_H_
#define	_PDT_DAC_LOADPARA_H_

#include "dac/dacshm.h"
#include "dac/comminf.h"
#include "rdbop/rdbop.h"

namespace PDT
{
	class DAC_EXPORT CLoadPara
	{
	public:
		CLoadPara();
		~CLoadPara();

		//加载所有参数
		bool	loadAll();
		//价值厂站参数
		bool	loadStation();
		//加载链路参数
		bool	loadChannel();
		//加载数据组参数
		bool	loadGroup(bool withData=true);
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
		//加载测点模型表
		bool	loadYccon();
		//加载监测装置表
		bool	loadSendDev();

	protected:
		CShareMemory		m_dacShm;
		CCommInf			m_commInf;
		CRdbOp*				m_pRdbOp;
	private:
		hInt32 assignStation(hInt32 no,const CDataset& ds,hInt32 rowIdx);
		hInt32 assignChannel(hInt32 no,const CDataset& ds,hInt32 rowIdx);
		hInt32 assignGroup(hInt32 no,const CDataset& ds,hInt32 rowIdx,bool withData=true);
		hInt32 assignRoute(hInt32 no,const CDataset& ds,hInt32 rowIdx);
		hInt32 assignProtocol(hInt32 no,const CDataset& ds,hInt32 rowIdx);
		hInt32 assignSendDev(hInt32 no,const CDataset& ds,hInt32 rowIdx);

	};
}
#endif //_PDT_DAC_LOADPARA_H_

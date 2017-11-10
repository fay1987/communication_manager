/*==============================================================================
* 文件名称 : algorithm.h
* 模块 : 算法基类
* 创建时间 : 2015-01-07 10:08:23
* 作者 : wlq
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_ALG_ALGORITHM_H_
#define	_PDT_ALG_ALGORITHM_H_

#include "pas/pas_export.h"
#include "dac/comminf.h"
#include "dac/datainf.h"
#include "dac/ctrlinf.h"
#include "dac/dacctrldef.h"

#define PAS_ALGORI_RUN_DELAY			100000		//100ms
#define	ALGORI_ONESEC_TIMES	(1000000/PAS_ALGORI_RUN_DELAY)		//算法任务每秒的调度次数

namespace PDT
{
		class DAC_EXPORT CAlgorithm
		{
		public:
			CAlgorithm();
			virtual ~CAlgorithm();
			//是否打开
			virtual	bool	isOpen() const = 0;
			//打开规约
			virtual bool	open() = 0;
			//关闭规约
			virtual void	close() = 0;
			//计算主入口
			virtual void	mainCompute() = 0;

		public:
			//初始化时调用
			bool			init(hInt32 channel,hInt32 route);
		protected:
			/**
			* @function			检查参数有效性
			* @param isSend		true:该参数有效；false:该参数无效，不存在或者当前无效值
			* @simple example	每次用到参数的时候，先检查一次
			*/
			hBool			ycParaValid(hInt32 no) const;
			hBool			ycParaValid(hInt32 startno , hInt32 endno);
			hBool			yxParaValid(hInt32 no) const;
			hBool			yxParaValid(hInt32 startno , hInt32 endno);
			hBool			ykParaValid(hInt32 no) const;
			hBool			ykParaValid(hInt32 startno , hInt32 endno);
			hBool			ytParaValid(hInt32 no) const;
			hBool			ytParaValid(hInt32 startno , hInt32 endno);
			/**
			* @function			获取参数值
			* @param no		测点在参数表中配置的位置
			*/
			hFloat			ycPara(hInt32 no);
			hUInt8			yxPara(hInt32 no);

			/**
			* @function			设置参数值
			* @param no		测点在参数表中配置的位置
			*/
			hBool			setYcPara(hInt32 no, hFloat val);
			hBool			setYxPara(hInt32 no, hUInt8 val);
			hBool			setYkPara(hInt32 no, hUInt8 val);
			hBool			setYtPara(hInt32 no, hFloat val);
						
		protected:
			CCommInf		m_commInf;
			CDataInf		m_dataInf;
			CCtrlInf		m_ctrlInf;
			hInt32			m_task;		//任务号
		};
}
#endif //_PDT_DAC_PROTOCOL_H_

/*==============================================================================
* 文件名称 : dacshm.h
* 模块 : 数据采集
* 创建时间 : 2009-06-07 15:55:23
* 作者 : BBB
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DACSHM_H_
#define	_PDT_DACSHM_H_

#include <ace/Malloc_T.h>
#include <ace/PI_Malloc.h>	
#include <ace/MMAP_Memory_Pool.h>
#include <ace/Process_Mutex.h>
#include "dac/dac_export.h"
#include "dac/dac.h"
#include "ace/Global_Macros.h"

namespace PDT
{
	class	DAC_EXPORT	CShareMemory
	{
		friend class CLoadPara;
		friend class CDataInf;
		friend class CCtrlInf;
		friend class CCommInf;
		friend class CBuffer;
		friend class CMessage;
	private:
		//typedef	ACE_Malloc<ACE_MMAP_MEMORY_POOL,ACE_SYNCH_MUTEX>	Malloc;
		//typedef	ACE_Malloc_T<ACE_MMAP_MEMORY_POOL,ACE_SYNCH_MUTEX,ACE_PI_Control_Block>	Malloc;
		typedef	ACE_Malloc_T<ACE_MMAP_MEMORY_POOL,ACE_SYNCH_PROCESS_MUTEX,ACE_PI_Control_Block>	Malloc;
	public:
		CShareMemory();
		virtual	~CShareMemory();
	public:
		//参数区指针
		const	DAC_YC				*ycPara(hInt32 groupNo)	const;
		const	DAC_YX				*yxPara(hInt32 groupNo)	const;
		const	DAC_KWH				*kwhPara(hInt32 groupNo)	const;
		const	DAC_SOE				*soePara(hInt32 groupNo)	const;
		//数据区指针
		const	DAC_YC_DATA			*ycData(hInt32 routeNo)	const;
		const	DAC_YX_DATA			*yxData(hInt32 routeNo)	const;
		const	DAC_KWH_DATA		*kwhData(hInt32 routeNo)	const;
		const	DAC_SOE_DATA		*soeData(hInt32 routeNo)	const;
		//变化数据区指针
		const	DAC_YC_CHG_DATA		*ycChgData(hInt32 routeNo)	const;
		const	DAC_YX_CHG_DATA		*yxChgData(hInt32 routeNo)	const;
		const	DAC_KWH_CHG_DATA	*kwhChgData(hInt32 routeNo)	const;
	protected:
		//映射某个数据组的参数、数据、变化数据区
		void	mapYc(int groupNo);
		void	mapYx(int groupNo);
		void	mapKwh(int groupNo);
	protected:
		//参数区指针
		DAC_YC				*ycPara(hInt32 groupNo);
		DAC_YX				*yxPara(hInt32 groupNo);
		DAC_KWH				*kwhPara(hInt32 groupNo);
		DAC_SOE				*soePara(hInt32 groupNo);
		//数据区指针
		DAC_YC_DATA			*ycData(hInt32 routeNo);
		DAC_YX_DATA			*yxData(hInt32 routeNo);
		DAC_KWH_DATA		*kwhData(hInt32 routeNo);
		DAC_SOE_DATA		*soeData(hInt32 routeNo);
		//变化数据区指针
		DAC_YC_CHG_DATA		*ycChgData(hInt32 routeNo);
		DAC_YX_CHG_DATA		*yxChgData(hInt32 routeNo);
		DAC_KWH_CHG_DATA	*kwhChgData(hInt32 routeNo);
	private:
		//映射整个前置内存
		void	map();	

		//释放整个前置内存
		void	unMap();					
	protected:
		static	DAC_SHM_COM		*m_ptrCom;				//数据指针区（内存名：公共）
		static	DAC_SHM_YC		*m_ptrYC;				//数据指针区（内存名：YC）
		static	DAC_SHM_YX		*m_ptrYX;				//数据指针区（内存名：YX）
		static	DAC_SHM_KWH		*m_ptrKWH;				//数据指针区（内存名：KWH）
		static	DAC_SHM_SOE		*m_ptrSOE;				//数据指针区（内存名：SOE）
	private:
		static	Malloc			*m_shmCom;				//公共
		static	Malloc			*m_shmYC;				//YC
		static	Malloc			*m_shmYX;				//YX
		static	Malloc			*m_shmKWH;				//KWH
		static	Malloc			*m_shmSOE;				//SOE
	};
}

#endif //_PDT_DACSHM_H_

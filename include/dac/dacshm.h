/*==============================================================================
* �ļ����� : dacshm.h
* ģ�� : ���ݲɼ�
* ����ʱ�� : 2009-06-07 15:55:23
* ���� : BBB
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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
		//������ָ��
		const	DAC_YC				*ycPara(hInt32 groupNo)	const;
		const	DAC_YX				*yxPara(hInt32 groupNo)	const;
		const	DAC_KWH				*kwhPara(hInt32 groupNo)	const;
		const	DAC_SOE				*soePara(hInt32 groupNo)	const;
		//������ָ��
		const	DAC_YC_DATA			*ycData(hInt32 routeNo)	const;
		const	DAC_YX_DATA			*yxData(hInt32 routeNo)	const;
		const	DAC_KWH_DATA		*kwhData(hInt32 routeNo)	const;
		const	DAC_SOE_DATA		*soeData(hInt32 routeNo)	const;
		//�仯������ָ��
		const	DAC_YC_CHG_DATA		*ycChgData(hInt32 routeNo)	const;
		const	DAC_YX_CHG_DATA		*yxChgData(hInt32 routeNo)	const;
		const	DAC_KWH_CHG_DATA	*kwhChgData(hInt32 routeNo)	const;
	protected:
		//ӳ��ĳ��������Ĳ��������ݡ��仯������
		void	mapYc(int groupNo);
		void	mapYx(int groupNo);
		void	mapKwh(int groupNo);
	protected:
		//������ָ��
		DAC_YC				*ycPara(hInt32 groupNo);
		DAC_YX				*yxPara(hInt32 groupNo);
		DAC_KWH				*kwhPara(hInt32 groupNo);
		DAC_SOE				*soePara(hInt32 groupNo);
		//������ָ��
		DAC_YC_DATA			*ycData(hInt32 routeNo);
		DAC_YX_DATA			*yxData(hInt32 routeNo);
		DAC_KWH_DATA		*kwhData(hInt32 routeNo);
		DAC_SOE_DATA		*soeData(hInt32 routeNo);
		//�仯������ָ��
		DAC_YC_CHG_DATA		*ycChgData(hInt32 routeNo);
		DAC_YX_CHG_DATA		*yxChgData(hInt32 routeNo);
		DAC_KWH_CHG_DATA	*kwhChgData(hInt32 routeNo);
	private:
		//ӳ������ǰ���ڴ�
		void	map();	

		//�ͷ�����ǰ���ڴ�
		void	unMap();					
	protected:
		static	DAC_SHM_COM		*m_ptrCom;				//����ָ�������ڴ�����������
		static	DAC_SHM_YC		*m_ptrYC;				//����ָ�������ڴ�����YC��
		static	DAC_SHM_YX		*m_ptrYX;				//����ָ�������ڴ�����YX��
		static	DAC_SHM_KWH		*m_ptrKWH;				//����ָ�������ڴ�����KWH��
		static	DAC_SHM_SOE		*m_ptrSOE;				//����ָ�������ڴ�����SOE��
	private:
		static	Malloc			*m_shmCom;				//����
		static	Malloc			*m_shmYC;				//YC
		static	Malloc			*m_shmYX;				//YX
		static	Malloc			*m_shmKWH;				//KWH
		static	Malloc			*m_shmSOE;				//SOE
	};
}

#endif //_PDT_DACSHM_H_

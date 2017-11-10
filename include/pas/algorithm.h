/*==============================================================================
* �ļ����� : algorithm.h
* ģ�� : �㷨����
* ����ʱ�� : 2015-01-07 10:08:23
* ���� : wlq
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_PDT_ALG_ALGORITHM_H_
#define	_PDT_ALG_ALGORITHM_H_

#include "pas/pas_export.h"
#include "dac/comminf.h"
#include "dac/datainf.h"
#include "dac/ctrlinf.h"
#include "dac/dacctrldef.h"

#define PAS_ALGORI_RUN_DELAY			100000		//100ms
#define	ALGORI_ONESEC_TIMES	(1000000/PAS_ALGORI_RUN_DELAY)		//�㷨����ÿ��ĵ��ȴ���

namespace PDT
{
		class DAC_EXPORT CAlgorithm
		{
		public:
			CAlgorithm();
			virtual ~CAlgorithm();
			//�Ƿ��
			virtual	bool	isOpen() const = 0;
			//�򿪹�Լ
			virtual bool	open() = 0;
			//�رչ�Լ
			virtual void	close() = 0;
			//���������
			virtual void	mainCompute() = 0;

		public:
			//��ʼ��ʱ����
			bool			init(hInt32 channel,hInt32 route);
		protected:
			/**
			* @function			��������Ч��
			* @param isSend		true:�ò�����Ч��false:�ò�����Ч�������ڻ��ߵ�ǰ��Чֵ
			* @simple example	ÿ���õ�������ʱ���ȼ��һ��
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
			* @function			��ȡ����ֵ
			* @param no		����ڲ����������õ�λ��
			*/
			hFloat			ycPara(hInt32 no);
			hUInt8			yxPara(hInt32 no);

			/**
			* @function			���ò���ֵ
			* @param no		����ڲ����������õ�λ��
			*/
			hBool			setYcPara(hInt32 no, hFloat val);
			hBool			setYxPara(hInt32 no, hUInt8 val);
			hBool			setYkPara(hInt32 no, hUInt8 val);
			hBool			setYtPara(hInt32 no, hFloat val);
						
		protected:
			CCommInf		m_commInf;
			CDataInf		m_dataInf;
			CCtrlInf		m_ctrlInf;
			hInt32			m_task;		//�����
		};
}
#endif //_PDT_DAC_PROTOCOL_H_

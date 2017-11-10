/*==============================================================================
* �ļ����� : loadpara.h
* ģ�� : ǰ�ò�������
* ����ʱ�� : 2009-06-28 15:55:23
* ���� : ����ƽ
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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

		//�������в���
		bool	loadAll();
		//��ֵ��վ����
		bool	loadStation();
		//������·����
		bool	loadChannel();
		//�������������
		bool	loadGroup(bool withData=true);
		//����·������
		bool	loadRoute();
		//���ع�Լ����
		bool	loadProtocol();
		//����ң�����
		bool	loadYc(const char* group=0);
		//����ң�Ų���
		bool	loadYx(const char* group=0);
		//���ص�Ȳ���
		bool	loadKwh(const char* group=0);
		//����ң�ر�
		bool	loadYk();
		//����ң����
		bool	loadYt();
		//���ز��ģ�ͱ�
		bool	loadYccon();
		//���ؼ��װ�ñ�
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

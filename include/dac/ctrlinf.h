/*==============================================================================
* �ļ����� : ctrlinf.h
* ģ�� : ���ƴ���ӿ�
* ����ʱ�� : 2009-07-20 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_PDT_DAC_CTRLINF_H_
#define	_PDT_DAC_CTRLINF_H_

#include "dac/dacshm.h"
#include "dac/comminf.h"

#define CTRL_CMD_TIMEOUT	20			//20s
#define CTRL_ACK_TIMEOUT	20			//20s

namespace PDT
{
	class DAC_EXPORT CCtrlInf 
	{
	public:
		CCtrlInf();
		virtual ~CCtrlInf();

		/*  1�����º������������������
		*	2��buf������:ctrl_head + ���������ṹ,�� dacctrl.h
		*/

		//�������,����true-�ɹ�,false-ʧ��
		bool	add(hUInt32 type,void* buf,hInt32 len,hInt32 groupNo=-1);
		//ɾ����������,����true-�ɹ�,false-ʧ��
		bool	remove();
		//������ʱ�����
		void	check();
		//��ȡһ����Ч����,���ػ�ȡ����������Ч�ֽ���, <=0ʧ�ܣ�>0�ɹ�
		int		get(void* buf,hInt32 len );
		//��ȡָ���������һ����Ч����,���ػ�ȡ����������Ч�ֽ���, <=0ʧ�ܣ�>0�ɹ�
		int		get(hInt32 groupNo,void* buf,hInt32 len);
		int		get(const char* groupCode,void* buf,hInt32 len);
		//��ȡָ��������ָ�����͵�һ����Ч����,���ػ�ȡ����������Ч�ֽ���, <=0ʧ�ܣ�>0�ɹ�
		int		get(hInt32 groupNo,hUInt32 type,void* buf,hInt32 len);
		int		get(const char* groupCode,hUInt32 type,void* buf,hInt32 len);

		//��ȡ������������
		int		getParaLoad(void* buf,hInt32 len);
		//��ȡ�˹���������
		int		getManOperate(void* buf,hInt32 len);
		//��ȡ��Լ��������
		int		getProtocol(hInt32 groupNo,void* buf,hInt32 len);

		/*  1�����º��������������У������
		*	2������ֵfalse-ʧ�ܣ�true-�ɹ�
		*	3��buf������:ctrl_head + ctrl_pro_yk_ack,�� fdcctrl.h
		*/
		//���һ����У����
		bool	addAck(hUInt32 type,void* buf,hInt32 len,hUInt32 groupNo=-1);
		//ɾ����У����
		bool	removeAck();
		//��鷴У��ʱ�����
		void	checkAck();
		//��ȡһ����У����
		int		getAck(void* buf,hInt32 len);
		//��ȡָ���������һ����У����
		int		getAck(hInt32 groupNo,void* buf,hInt32 len);
		int		getAck(const char* groupCode,void* buf,hInt32 len);
		//��ȡָ��������ָ�����ƺŵ�һ����У����
		//int		getAck(hInt32 groupNo,hInt32 ctrlNo,void *buf,hInt32 len);
		//int		getAck(const char* groupCode,hInt32 ctrlNo,void* buf,hInt32 len);
	public:
		static const char*		describe(hUInt32 type);	
	protected:
		CShareMemory	m_shm;
		CCommInf		m_commInf;

		DAC_CTRL_CMD*	getCtrl();
		int				getCtrl(hUInt32 beginType,hUInt32 endType,void* buf,hInt32 len);
		int				getCtrl(hInt32 groupNo,hUInt32 beginType,hUInt32 endType,void* buf,hInt32 len);
		DAC_CTRL_CMD*	getCtrlAck();
	};
}

#endif	//_ECON_FDC_CTRLINF_H_

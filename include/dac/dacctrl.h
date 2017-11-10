/*==============================================================================
* �ļ����� : fdcctrl.h
* ģ�� : ǰ�ÿ��ƽӿ�
* ����ʱ�� : 2008-09-16 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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

		//----------------��������-------------------------------
		//���ݱ����ȼ��ز���:tablName--����;index1--һ������;index2--��������
		bool	load(const char* tableName,const char* index1=0,const char* index2=0);
		//�������в���
		bool	loadAll();
		//���س�վ����
		bool	loadStation();
		//�����豸
		bool	loadDevice();
		//�����豸����
		bool	loadDeviceType();
		//�����豸������
		bool	loadDevicePara();
		//����ͨ������
		bool	loadChannel();
		//�������������
		bool	loadGroup();
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

		//----------------ͨ������-------------------------------
		//�˹�����ͨ��
		bool manStartChannel(const char* channelCode);
		//�˹�ֹͣͨ��
		bool manStopChannel(const char* channelCode);
		//�˹�����ͨ��
		bool manEnableChannel(const char* channelCode);
		//�˹�����ͨ��
		bool manDisableChannel(const char* channelCode);

		//----------------·������-------------------------------
		//�˹�����·��
		bool manStartRoute(const char* routeCode);
		//�˹�ֹͣ·��
		bool manStopRoute(const char* routeCode);
		//�˹�����·��
		bool manEnableRoute(const char* routeCode);
		//�˹�����·��
		bool manDisableRoute(const char* routeCode);
		//�˹�����ָ��·���Ĺ�Լ
		bool manResetProtocol(const char* routeCode);

		//----------------���������-----------------------------
		//�˹�����������
		bool manStartGroup(const char* groupCode);
		//�˹�ֹͣ������
		bool manStopGroup(const char* groupCode);
		//�˹�����������
		bool manEnableGroup(const char* groupCode);
		//�˹�����������
		bool manDisableGroup(const char* groupCode);

		//----------------��Լ����ָ��----------------------------
		//��λRTU
		bool resetRtu(const char* groupCode);
		//��ʱ
		bool syncTime(const char* groupCode);
		//���ٻ�
		bool callAllData(const char* groupCode);
		//����ң��
		bool callAllDataYc(const char* groupCode);
		//����ң��
		bool callAllDataYx(const char* groupCode);
		//���ٵ��
		bool callAllDataKwh(const char* groupCode);
		//�����ٻ�(ĳЩ��Լ��Ҫ)
		bool callSubgroupData(const char* groupCode,hInt32 subgroupNo);
		//ң��
		bool yk(const ctrl_pro_yk* pYk);
		//ң��
		bool yt(const ctrl_pro_yk* pYt);
		//ֱ�����
		bool directOutput(const ctrl_pro_direct_operate* pDirectOutput);
		//���(localΪtrue�����ñ����ڴ����ݣ�false��ʹ�ù�Լ����㹦�ܣ������RTU)
		bool setYc(const char* groupCode,const char* pointCode,hFloat value,bool local=true);
		bool setYx(const char* groupCode,const char* pointCode,hUInt8 value,bool local=true);
		bool setKwh(const char* groupCode,const char* pointCode,hFloat value,bool local=true);

		//----------------ͨ��buf��Լ����ָ��----------------------------
		bool commonCmd(const ctrl_pro_common* pCommon);
		bool commonCmdAck(const ctrl_pro_common_ack* pCommonAck);

		//---------------��Լ��У��Ϣ-------------------------------
		//ң�ط�У
		bool ykAck(const ctrl_pro_yk_ack* ykAck);

	private:
		CCommInf	m_commInf;
		CCtrlInf	m_ctrlInf;
	};
}

#endif	//_PDT_DAC_DACCTRL_H_

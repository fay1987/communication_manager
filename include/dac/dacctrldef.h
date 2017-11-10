/*===================================================================
*	ģ�����ƣ�	���ݲɼ�
*	��������:	2013��11��27�� 9:34
*	�� �� ��: 	E:\pdt3000\include\dac\dacctrldef.h
*	�ļ�·��:	E:\pdt3000\include\dac
*	ģ������:	dacctrldef
*	�ļ�����:	h
	
*	��    ��:	wlq
*	��    ��:	V2.0
	
*	ģ��˵��:	���ݿ������Ͷ���

*	�޸İ汾��	V2.1
*	�޸����ߣ�	wlq
*	�޸����ڣ�	27/11/2013
*	�޸�ժҪ��

====================================================================*/

#ifndef	_PDT_DAC_DACCTRLDEF_H_
#define	_PDT_DAC_DACCTRLDEF_H_

#include "utl/datatype.h"

#define CTRL_COMMON_LEN		200		//ͨ�ÿ��������,��ֹ��ctrl buf len=256

namespace PDT
{
	//������Ϣͷ
	typedef struct _ctrl_head_ 
	{
		nUInt32	type;				//��������,��� fdcdef.h �е� CTRL_TYPE
	}ctrl_head;

	//����ң�⡢ң�š���Ȳ���
	typedef struct _ctrl_loadpara_data_ 
	{
		char	group[DAC_CODE_LEN];		//���������
	}ctrl_loadpara_data;

	//�˹�������·��·����������
	typedef struct _ctrl_man_operate_
	{
		char	code[DAC_CODE_LEN];			//��·����·���������������
	}ctrl_man_operate;

	//�л���������·��
	typedef struct _ctrl_switch_mainroute_ 
	{	
		char	groupCode[DAC_CODE_LEN];			//���������
		char	newMainRouteCode[DAC_CODE_LEN];		//�µ���·����
	}ctrl_switch_mainroute ;

	//�����������л�ģʽ
	typedef struct _ctrl_set_switch_mode_ 
	{
		char	groupCode[DAC_CODE_LEN];	//���������
		nUInt8	mode;						//�µ��л�ģʽ
	}ctrl_set_switch_mode;

	//��Լ�������������縴λ����ʱ�����ٻ���
	typedef struct _ctrl_pro_group_cmd_ 
	{
		char	groupCode[DAC_CODE_LEN];	//���������
		nInt32	subgroupNo;					//�����ٻ���ʹ�õ��ķ����
	}ctrl_pro_group_cmd;

	//��Լң��
	typedef struct _ctrl_pro_yk_ 
	{
		char	groupCode[DAC_CODE_LEN];	//���������
		nUInt8	ctrlType;					//�������ͣ���ң��Ԥ�á�ִ�С�����
		nInt32	ctrlNo;						//���ƺ�
		nInt32	ctrlOnNo;					//�غϺ�
		nInt32	ctrlOffNo;					//�طֺ�
		nUInt32	ctrlDuration;				//����ʱ��
		nUInt8	ctrlState;					//ϣ�����Ƶ�״̬
		nUInt8	ackState;					//����״̬
		char	ackReciever;				//������Ϣ������(�磺SCADA_SERVER,FDC_SERVER)
		nUInt8	elapseTime;					//����δ��ִ�ж����ŵ�ʱ��
	}ctrl_pro_yk;

	//��Լң�ط�У
	typedef struct _ctrl_pro_yk_ack_
	{
		nUInt8	funcCode;					//������
		nInt32	groupNo;					//�������
		nInt32	ctrlNo;						//����
		nUInt8	ackState;					//����״̬
	}ctrl_pro_yk_ack;

	//��Լ���
	typedef struct _ctrl_pro_setpoint_
	{
		char	groupCode[DAC_CODE_LEN];	//���������
		char	pointCode[DAC_CODE_LEN];	//����
		nFloat	floatValue;					//������ĸ���ֵ
		nInt32	int32Value;					//�����������ֵ
		nUInt8	localFlag;					//�Ƿ�ֻ���ñ����ڴ�
	}ctrl_pro_setpoint;

	//��Լ�����չ
	typedef struct _ctrl_pro_setpointex_
	{
		char	groupCode[DAC_CODE_LEN];	//���������
		char	pointCode[DAC_CODE_LEN];	//����		
		nFloat	floatValue;					//������ĸ���ֵ
		nInt32	ctrlNo;						//���ú�
		nUInt8	ctrlType;					//�������ͣ���ң��Ԥ�á�ִ�С�����
		nUInt8	localFlag;					//�Ƿ�ֻ���ñ����ڴ�
	}ctrl_pro_setpointex;

	//��Լֱ�Ӳ���
	typedef struct _ctrl_pro_direct_operate_
	{
		nUInt8	type;						//��������
		char	groupCode[DAC_CODE_LEN];	//���������
		char	pointCode[DAC_CODE_LEN];	//����
		nUInt32	pulseCount;					//�������
		nUInt32	onTime;						//
		nUInt32	offTime;					//
	}ctrl_pro_direct_operate;

	//add by yaoff on 20150502
	//��ֵ����
	typedef struct _ctrl_pro_constantvalue_
	{
		char	groupCode[DAC_CODE_LEN];	//�������
		nUInt8	ctrlType;
		nUInt32	length;						//�ⲿ�Զ�������峤��
		char	cmdBuf[CTRL_COMMON_LEN];	//�ⲿ�Զ��������
	}ctrl_pro_constantvalue;

	typedef struct _ctrl_pro_constantvalue_ack_
	{
		nUInt8	ackState;					//����״̬
		nInt32	groupNo;					//�������
		//nUInt8	ctrlType;
		nUInt32	length;						//�ⲿ�Զ�������峤��
		char	ackBuf[DAC_LNAME_LEN];		//�ⲿ�Զ��������	
	}ctrl_pro_constantvalue_ack;

	typedef struct _ctrl_pro_calldata_
	{
		char	groupCode[DAC_CODE_LEN];					//�������
		nUInt8	ctrlNo;						//�����
		//char	dataType[DAC_CODE_LEN];		//��������
	}ctrl_pro_calldata;

	typedef struct _ctrl_pro_calldata_ack_
	{
		nInt32	groupNo;					//�������
		//nUInt8	ctrlNo;
		nUInt8	ackState;						//�ⲿ�Զ�������峤��
		//char	cmdBuf[CTRL_COMMON_LEN];	//�ⲿ�Զ��������
	}ctrl_pro_calldata_ack;

	typedef struct _ctrl_pro_data_soe_
	{
		nInt32	nDataNo;				//�����������
		nUInt8	nErrorType;				//��������
		nUInt32	nAction1;				//����ֵ1
		nUInt32	nAction2;				//����ֵ2
		nUInt32 nAction3;				//����ֵ3
		nUInt32 datetime;				//ʱ��
		
	}ctrl_pro_data_soe;


	//add end.

	//��Լͨ������
	typedef struct _ctrl_pro_common_
	{
		char	groupCode[DAC_CODE_LEN];	//���������
		nUInt32	length;						//�ⲿ�Զ�������峤��
		char	cmdBuf[CTRL_COMMON_LEN];	//�ⲿ�Զ��������
	}ctrl_pro_common;

	//ͨ�����У
	typedef struct _ctrl_pro_common_ack_ 
	{
		nInt32	groupNo;					//�������
		nUInt32	length;						//�ⲿ�Զ��巴У���峤��
		char	ackBuf[CTRL_COMMON_LEN];	//�ⲿ�Զ��巴У����
	}ctrl_pro_common_ack;

}
#endif //_PDT_DAC_DACCTRLDEF_H_

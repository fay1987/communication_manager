/*==============================================================================
* �ļ����� : dac.h
* �ļ����� �����ݲɼ������ڴ�ṹ����
* ģ�� : ���ݲɼ�
* ����ʱ�� : 2009-06-08
* ���� : wlq
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_PDT_DAC_H_
#define	_PDT_DAC_H_

#include "utl/datatype.h"
#include "pasmacro.h"
//---------------------------------------------------------------------------------------

//�߼���������
typedef struct _pas_task_
		{
			hBool		valid;									//��Ч��־
			hInt32		taskno;
			hBool		useflag;
			hChar		algcode[32];
			hUInt8		tasktype;
			CDateTime	ymdhms;
			hChar		para1[64];
			hChar		para2[64];
			hChar		para3[64];
			hChar		para4[64];
			hChar		para5[64];
		}PAS_TASK,*PAS_TASK_PTR;

//�߼������㷨
		typedef struct _pas_algorithm_
		{
			hBool		valid;									//��Ч��־
			hChar		algcode[32];
			hChar		name[64];
			hChar		dllname[32];
			hChar		featuretable[32];
		}PAS_ALGORITHM,*PAS_ALGORITHM_PTR;

//�߼������㷨����
		typedef struct _pas_iopara_
		{
			hBool		valid;									//��Ч��־
			hChar		algcode[32];
			hInt32		id;
			hUChar		paratype;
			hUChar		meastype;
			hint32		measno;
			hChar		grpcode[32];
			hChar		pointcode[32];
			hChar		value[16];
			//add info
			hFloat		fval;
			hInt32		ival;
		}PAS_IOPARA,*PAS_IOPARA_PTR;


//=======================================================================================
//ϵͳ��Ϣ
typedef	struct _pas_system_info_
{
	hBool			serverFlag;					//�Ƿ���PAS������
	hUInt32			algorithmNum;				//ϵͳʵ���㷨����
	hUInt32			taskNum;					//ϵͳʵ���������
	hUInt32			loadParaFlag;				//װ�ز������־������λ��ͬ���ֲ�����
}PAS_SYSTEM_INFO;

//�㷨��Ϣ
typedef struct _dac_algorithm_info_
{
	nUInt8			state;						//״̬�����ӡ��Ͽ���
	nUInt32			lastCalcTime;				//���һ�μ���ʱ��
	nUInt32			lastUsedTime;				//���һ�μ�������ʱ��
	nUInt32			txYkCnt;					//����ң��ָ����
	nUInt32			rxYtCnt;					//����ң��ָ����
	nUInt32			dayOkTime;					//������ʱ��
	nUInt32			dayErrCnt;					//�չ��ϴ���
	nUInt8			reset;						//��λ��־
	nBool			manStop;					//�˹�ֹͣ��־
	nUInt32			routeNum;					//���㷨��صĲ�������
	nInt32			routes[PAS_PARA_IN_ALGORITHM];//���㷨��صĲ������
}DAC_ALGORITHM_INFO;

//��������Ϣ
typedef struct _dac_group_info_
{
	nUInt8			state;						//״̬��������
	nUInt32			lastUpTime;					//���һ����DOWN->UP��ʱ��
	nUInt32			lastDownTime;				//���һ����UP->DOWN��ʱ��
	nUInt32			dayOkTime;					//������ʱ��
	nUInt32			dayErrCnt;					//�չ��ϴ���
	nBool			manStop;					//�˹�ֹͣ��־
	nBool			initFlag;					//��ʼ����־
	nUInt32			routeNum;					//��GROUP������·������
	nInt32			routes[DAC_ROUTE_IN_GROUP];	//��GROUP������·����
	DAC_COMMAND		txCmd;						//���͵�����
	DAC_COMMAND		rxCmd;						//���յ�����
}DAC_GROUP_INFO;

//��������
typedef	struct _dac_com_para_
{
	DAC_SYSTEM			system;							//ϵͳ����
	DAC_STATION			station[DAC_STATION_NUM];		//��վ��
	DAC_CHANNEL			channel[DAC_CHANNEL_NUM];		//ͨ����
	DAC_GROUP			group[DAC_GROUP_NUM];			//�������
	DAC_ROUTE			route[DAC_ROUTE_NUM];			//·����
	DAC_PROTOCOL		protocol[DAC_PROTOCOL_NUM];		//��Լ��
	DAC_YK				yk[DAC_YK_NUM];					//ң�ر�--��ң�ⲻͬ����no��
	DAC_YT				yt[DAC_YK_NUM];					//ң����--��ң�ⲻͬ����no��
}DAC_COM_PARA;
//������Ϣ
typedef	struct _dac_com_info_
{
	DAC_SYSTEM_INFO		system;							//ϵͳ����
	DAC_CHANNEL_INFO	channel[DAC_CHANNEL_NUM];		//ͨ����
	DAC_GROUP_INFO		group[DAC_GROUP_NUM];			//�������
	DAC_ROUTE_INFO		route[DAC_ROUTE_NUM];			//·����
	DAC_CHG_CHANNEL_INFO chgChannel[2*DAC_CHANNEL_NUM];	//ͨ���仯��Ϣ������
	DAC_CHG_GROUP_INFO	chgGroup[2*DAC_GROUP_NUM];		//������仯��Ϣ������
	DAC_CHG_ROUTE_INFO	chgRoute[2*DAC_ROUTE_NUM];		//·���仯��Ϣ������
	hUInt32				chgChannelWrite;				//ͨ���仯��Ϣ������дָ��
	hUInt32				chgGroupWrite;					//������仯��Ϣ������дָ��
	hUInt32				chgRouteWrite;					//·���仯��Ϣ������дָ��
	DAC_RECV_BUFFER		recvBuffer[DAC_CHANNEL_NUM];	//���ձ��Ļ�����
	DAC_SEND_BUFFER		sendBuffer[DAC_CHANNEL_NUM];	//���ͱ��Ļ�����
	DAC_MESSAGE_BUFFER	messageBuf[DAC_ROUTE_NUM];		//�������Ļ�����
}DAC_COM_INFO;
//---------------------------------------------------------------------------------------
//���������ڴ�
typedef	struct _dac_shm_com_
{
	DAC_COM_PARA		para;						//����
	DAC_COM_INFO		info;						//��Ϣ
	DAC_CTRL_CMD		ctrl[DAC_CTRL_NUM];			//�����������
	DAC_CTRL_CMD		ack[DAC_CTRL_NUM];			//�������У��
}DAC_SHM_COM;

//��ǰ��MAP��Ϣ
typedef	struct	_dac_map_info_
{
	hUInt32		num;								//��ǰ���ٵ�·���������ж������
	hUInt32		routeNum;							//��������������·������
	hInt32		routes[DAC_ROUTE_IN_GROUP];			//��������������·����
}DAC_MAP_INFO;
//YC�����ڴ�
typedef	struct _dac_shm_yc_
{
	bool				lock[DAC_ROUTE_NUM];		//��
	hUInt32				write[DAC_ROUTE_NUM];		//�仯������дƫ����
	size_t				para[DAC_GROUP_NUM];		//������ָ��ƫ��
	size_t				data[DAC_ROUTE_NUM];		//������ָ��ƫ��
	size_t				chgData[DAC_ROUTE_NUM];		//�仯������ָ��ƫ��
	DAC_MAP_INFO		mapInfo[DAC_GROUP_NUM];		//��ǰMAP��Ϣ
}DAC_SHM_YC;
//YX�����ڴ�
typedef	struct _dac_shm_yx_
{
	bool				lock[DAC_ROUTE_NUM];		//��
	hUInt32				write[DAC_ROUTE_NUM];		//�仯������дƫ����
	size_t				para[DAC_GROUP_NUM];		//������ָ��ƫ��
	size_t				data[DAC_ROUTE_NUM];		//������ָ��ƫ��
	size_t				chgData[DAC_ROUTE_NUM];		//�仯������ָ��ƫ��
	DAC_MAP_INFO		mapInfo[DAC_GROUP_NUM];		//��ǰMAP��Ϣ
}DAC_SHM_YX;
//KWH�����ڴ�
typedef	struct _dac_shm_kwh_
{
	bool				lock[DAC_ROUTE_NUM];		//��
	hUInt32				write[DAC_ROUTE_NUM];		//�仯������дƫ����
	size_t				para[DAC_GROUP_NUM];		//������ָ��ƫ��
	size_t				data[DAC_ROUTE_NUM];		//������ָ��ƫ��
	size_t				chgData[DAC_ROUTE_NUM];		//�仯������ָ��ƫ��
	DAC_MAP_INFO		mapInfo[DAC_GROUP_NUM];		//��ǰMAP��Ϣ
}DAC_SHM_KWH;
//SOE�����ڴ�
typedef	struct _dac_shm_soe_
{
	bool				lock[DAC_ROUTE_NUM];		//��
	hUInt32				write[DAC_ROUTE_NUM];		//дƫ����
	size_t				data[DAC_ROUTE_NUM];		//������ָ��ƫ��
	DAC_MAP_INFO		mapInfo[DAC_GROUP_NUM];		//��ǰMAP��Ϣ
}DAC_SHM_SOE;

#endif //PDT_DAC_H_

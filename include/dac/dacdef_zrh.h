/*==============================================================================
* �ļ����� : dacdef.h
* �ļ����� �����ݲɼ��������Ͷ���
* ģ�� : ���ݲɼ�
* ����ʱ�� : 2009-06-08
* ���� : 
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_PDT_DAC_DACDEF_H_
#define	_PDT_DAC_DACDEF_H_

namespace	PDT
{
	//CHANNEL�û�
	enum	CHANNEL_USER
	{
		CHANNEL_TASK_USER		=	0x01,		//channelִ����
		CHANNEL_TASK_CLIENT		=	0x02,		//channel client
		CHANNEL_TASK_SERVER		=	0x04,		//channel server
		CHANNEL_TASK_UDP		=	0x06,		//channel dgram=udp
		CHANNEL_TASK_SERIAL		=	0x08,		//channel serial
		CHANNEL_TASK_ALL		=	0XFF		//��������
	};
	//װ�ز������־
	enum	LOADPARA_FLAG
	{
		LOADPARA_CHANNEL		=	0x00000001,	//ͨ����
		LOADPARA_GROUP			=	0x00000002,	//�������
		LOADPARA_ROUTE			=	0x00000004,	//·����
		LOADPARA_YC				=	0x00000008,	//YC��
		LOADPARA_YX				=	0x00000010,	//YX��
		LOADPARA_KWH			=	0x00000020,	//KWH��
		LOADPARA_ZFYC			=	0x00000040,	//ת��YC��
		LOADPARA_ZFYX			=	0x00000080,	//ת��YX��
		LOADPARA_ZFKWH			=	0x00000100,	//ת��KWH��
		LOADPARA_PROTOCOL		=	0x00000200,	//Protocol
		LOADPARA_STATION		=	0x00000400,	//��վ��
		LOADPARA_DEVICE			=	0x00000800,	//�豸��--scdʹ��
		LOADPARA_DEVICETYPE		=	0x00001000,	//�豸���ͱ�--scdʹ��
		LOADPARA_DEVICEPARA		=	0x00002000,	//�豸������--scdʹ��
		LOADPARA_YK				=	0x00004000,	//ң�ر�
		LOADPARA_YT				=	0x00008000,	//ң����
		LOADPARA_ALL			=	0xFFFFFFFF	//ȫ������
	};
	//ͨ��״̬
	enum	CHANNEL_STATE
	{
		CHANNEL_STATE_DOWN		=	0,			//�Ͽ�
		CHANNEL_STATE_LISTEN	=	1,			//����
		CHANNEL_STATE_UP		=	2,			//����
		CHANNEL_STATE_DISABLED	=	3,			//����
	};
	enum	CHANNEL_TYPE
	{
		CHANNEL_TYPE_SERIAL		=	0,			//����
		CHANNEL_TYPE_TCPCLIENT	=	1,			//TCP�ͻ���
		CHANNEL_TYPE_TCPSERVER	=	2,			//TCP�����
		CHANNEL_TYPE_UDP		=	3,			//UDP
		CHANNEL_TYPE_GATECLIENT	=	4,			//���ؿͻ���
		CHANNEL_TYPE_GATESERVER =   5,			//���ط����
		CHANNEL_TYPE_VIRTUAL	=	10			//��������(�������շ���ֱ�ӵ��ȹ�Լ)
	};
	//����������
	enum	GROUP_TYPE
	{
		GROUP_TYPE_STATIC_RX	= 0,			//������
		GROUP_TYPE_STATIC_TX	= 1,			//������
	};
	//������״̬
	enum	GROUP_STATE
	{
		GROUP_STATE_DOWN		=	0,			//ֹͣ
		GROUP_STATE_UP			=	1,			//�Ͽ�
		GROUP_STATE_DISABLED	=	2,			//����
	};
	//·������
	enum	ROUTE_TYPE
	{
		ROUTE_TYPE_RX			= 0,			//����
		ROUTE_TYPE_TX			= 1				//����
	};
	//·��״̬
	enum	ROUTE_STATE
	{
		ROUTE_STATE_DOWN		=	0,			//ֹͣ
		ROUTE_STATE_UP			=	1,			//����
		ROUTE_STATE_STANDBY		=	2,			//�ȱ�
		ROUTE_STATE_FREE		=	3,			//����
		ROUTE_STATE_DISABLED	=	4,			//����
	};
	//ң������
	enum	YC_TYPE
	{
		YC_TYPE_U				=	0,			//��ѹ����
		YC_TYPE_I				=	1,			//����
		YC_TYPE_P				=	2,			//�й�
		YC_TYPE_Q				=	3,			//�޹�
		YC_TYPE_F				=	4,			//Ƶ�� 
		YC_TYPE_COS				=	5,			//��������
		YC_TYPE_BAT_U			=	6,			//�����ѹ
		YC_TYPE_BAT_R			=	7,			//��������
		YC_TYPE_BAT_TOL_U		=	8,			//��˵�ѹ
		YC_TYPE_BAT_I			=	9,			//���ص���
		YC_TYPE_TEMP			=	10,			//�¶�
		YC_TYPE_HUMI			=	11			//ʪ��
	};
	//ң������
	enum	YX_TYPE
	{
		YX_TYPE_ALARM			=	0,			//�澯����
		YX_TYPE_SWITCH			=	1,			//��������
		YX_TYPE_WORK			=	2			//��������
	};
	//���Ļ���������
	enum	BUFFER_TYPE
	{
		BUFFER_RX				=	0,			//����
		BUFFER_TX				=	1			//����
	};
	//����֡����
	enum	FRAME_TYPE
	{
		FRAME_RX				=	0,			//����
		FRAME_RX_ERROR			=	1,			//���յĴ���֡
		FRAME_TX				=	2			//����
	};
	//������λ����
	enum	QUALITY_BIT
	{
		QUALITY_BIT_OV			=	0x00000001,	//���λ		:1--���
		QUALITY_BIT_ES			=	0x00000002,	//����λ		:1--����
		QUALITY_BIT_MS			=	0x00000004,	//�˹�����		:1--�˹�����
		QUALITY_BIT_WAR			=	0x00000008,	//�澯λ		:1--�澯
		QUALITY_BIT_BL			=	0x00000010,	//����λ		:1--����
		QUALITY_BIT_SB			=	0x00000020,	//ȡ��λ		:1--ȡ��
		QUALITY_BIT_NT			=	0x00000040,	//����λ		:1--�и���
		QUALITY_BIT_IV			=	0x00000080,	//��Чλ		:0--��Ч,1--��Ч
		//����
		QUALITY_BIT_INIT		=	0x10000000	//��ʼ��λ		:1--��ʼ��
	};
	enum	TIME_MODE
	{//��������ʱ����8Сʱ
		TIME_BEIJING			=	0,			//����ʱ��
		TIME_GREENWICH			=	1			//��������ʱ��
	};
	//YK״̬
	enum	YK_STATE
	{
		YK_ERR					=	0,			//����
		YK_ON					=	1,			//��
		YK_OFF					=	2			//��
	};

	//������������
	enum CTRL_TYPE
	{
		//���ز������� ( 1-100 )
		CTRL_LOADPARA_ALL				=	1,			//���ز���ALL
		CTRL_LOADPARA_CHANNEL			=	2,			//���ز���CHANNEL
		CTRL_LOADPARA_GROUP				=	3,			//���ز���GROUP
		CTRL_LOADPARA_ROUTE				=	4,			//���ز���ROUTE
		CTRL_LOADPARA_PROTOCOL			=	5,			//���ز���PROTOCOL
		CTRL_LOADPARA_COM				=	6,			//���ز���ͨѶ����
		CTRL_LOADPARA_YC				=	7,			//���ز���YC
		CTRL_LOADPARA_YX				=	8,			//���ز���YX
		CTRL_LOADPARA_KWH				=	9,			//���ز���KWH
		CTRL_LOADPARA_STATION			=	10,			//���ز���station
		CTRL_LOADPARA_DEVICE			=	11,			//���ز���device
		CTRL_LOADPARA_DEVICETYPE		=	12,			//���ز���devicetype
		CTRL_LOADPARA_DEVICEPARA		=	13,			//���ز���devicepara
		CTRL_LOADPARA_YK				=	14,			//���ز���yk
		CTRL_LOADPARA_YT				=	15,			//���ز���yt
		//ͨ����·������������� ( 101-200 )
		CTRL_MAN_START_CHANNEL			=	101,		//�˹�����ͨ��
		CTRL_MAN_STOP_CHANNEL			=	102,		//�˹�ֹͣͨ��
		CTRL_MAN_ENABLE_CHANNEL			=	103,		//�˹�����ͨ��
		CTRL_MAN_DISABLE_CHANNEL		=	104,		//�˹�����ͨ��
		CTRL_MAN_START_ROUTE			=	111,		//�˹�����·��
		CTRL_MAN_STOP_ROUTE				=	112,		//�˹�ֹͣ·��
		CTRL_MAN_ENABLE_ROUTE			=	113,		//�˹�����·��
		CTRL_MAN_DISABLE_ROUTE			=	114,		//�˹�����·��
		CTRL_MAN_RESET_PROTOCOL			=	115,		//�˹������ƶ�·���Ĺ�Լ
		CTRL_MAN_START_GROUP			=	121,		//�˹�����������
		CTRL_MAN_STOP_GROUP				=	122,		//�˹�ֹͣ������
		CTRL_MAN_ENABLE_GROUP			=	123,		//�˹�����������
		CTRL_MAN_DISABLE_GROUP			=	124,		//�˹�����������
		//��Լ���� ( 201-300 )
		CTRL_PRO_RESET_RTU				=	201,		//��λRTU
		CTRL_PRO_SYNC_TIME				=	202,		//��ʱ
		CTRL_PRO_CALL_ALL_DATA			=	203,		//���ٻ�
		CTRL_PRO_CALL_ALL_DATA_YC		=	204,		//����ң��
		CTRL_PRO_CALL_ALL_DATA_YX		=	205,		//����ң��
		CTRL_PRO_CALL_ALL_DATA_KWH		=	206,		//���ٵ��
		CTRL_PRO_CALL_SUBGROUP_DATA		=	207,		//�����ٻ�(101��Լ)
		CTRL_PRO_YK						=	208,		//ң��
		CTRL_PRO_YT						=	209,		//ң��
		CTRL_PRO_SETPOINT_YC			=	210,		//���ң��
		CTRL_PRO_SETPOINT_YX			=	211,		//���ң��
		CTRL_PRO_SETPOINT_KWH			=	212,		//�����
		CTRL_PRO_DIRECT_OPERATE			=	213,		//ֱ�Ӳ���
		//ͨ������
		CTRL_PRO_COMMON					=	299,		//�Զ���ͨ������
		CTRL_PRO_UNKNOWN				=	300,		//����ʶ���ͽ���
		//��Լ���Ʒ�У��Ϣ( 301-400 )
		CTRL_PRO_ACK_YK					=	301,		//ң�ط�У	
		CTRL_PRO_COMMON_ACK				=	303			//�Զ���ͨ�����У
	};

	//��־����
	enum DAC_LOG_TYPE
	{
		LOG_NET_CLIENT_BASE				=	1300,		//tcp client base
		LOG_NET_CLIENT_RECV				=	1301,		//recv
		LOG_NET_CLIENT_SEND				=	1302,		//send
		LOG_NET_SERVER_BASE				=	1305,		//tcp server base
		LOG_NET_SERVER_RECV				=	1306,		//recv
		LOG_NET_SERVER_SEND				=	1307,		//send
		LOG_NET_UDP_BASE				=	1310,		//udp base
		LOG_NET_UDP_RECV				=	1311,		//udp recv
		LOG_NET_UDP_SEND				=	1312,		//udp send
		LOG_SERIAL_BASE					=	1320,		//serial base
		LOG_SERIAL_RECV					=	1321,		//serial recv
		LOG_SERIAL_SEND					=	1322,		//serial send

		LOG_DACSHM_BASE					=	1400,		//dacshm��־����ֵ
		LOG_COMMINF_BASE				=	1410,		//comminf����ֵ
		LOG_DATAINF_BASE				=	1420,		//datainf����ֵ
		LOG_CTRLINF_BASE				=	1430,		//ctrlinf����ֵ
		LOG_LOADPARA_BASE				=	1440,		//loadpara��־����ֵ

		/////////////dacsrv////////
		LOG_DACSRV_BASE					=	1500,		//dacsrv������־����ֵ
		LOG_DACSRV_CONFIG				=	1501,		//�����ļ�����
		LOG_DACSRV_LINKPOOL				=	1502,		//linktaskpool
		
		LOG_DACSRV_CTRLTASK				=	1503,		//��������
		LOG_DACSRV_LINK					=	1510,		//��·
		LOG_DACSRV_STATETASK			=	1520,		//״̬���
		LOG_DACSRV_ZFTASK				=	1521		//ת��
	};
}

#endif //_PDT_DAC_DACDEF_H_

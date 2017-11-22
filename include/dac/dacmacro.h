/*==============================================================================
* �ļ����� : dacmacro.h
* �ļ����� �����ݲɼ����ֺ궨��
* ģ�� : ���ݲɼ�
* ����ʱ�� : 2009-06-08
* ���� : BBB
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	PDT_DAC_MACRO_H_
#define	PDT_DAC_MACRO_H_

#define DAC_CODE_LEN				32			//���볤��
#define DAC_LCODE_LEN				64			//���볤��(�ӳ���)
#define	DAC_SNAME_LEN				32			//���ֳ���(short��)
#define	DAC_NAME_LEN				64			//���ֳ���
#define	DAC_LNAME_LEN				128			//���ֳ���(�ӳ���)
#define	DAC_DESC_LEN				64			//��������
#define DAC_LDESC_LEN				128			//��������(�ӳ���)
#define DAC_USER_LEN				64			//�û�������
#define DAC_PWD_LEN					64			//���볤��	
#define	DAC_ADDR_LEN				32			//��ַ����
#define	DAC_HOSTNAME_LEN			32			//����������

#ifdef _ARM_LINUX
#define	DAC_ROUTE_IN_CHANNEL		16			//ÿCHANNEL֧�ֹҽӵ�·����
#define	DAC_ROUTE_IN_GROUP			16			//ÿGROUP֧�ֹҽӵ�·����
#else
#define	DAC_ROUTE_IN_CHANNEL		16			//ÿCHANNEL֧�ֹҽӵ�·����
#define	DAC_ROUTE_IN_GROUP			16			//ÿGROUP֧�ֹҽӵ�·����
#endif

#define	DAC_ADDR_NUM				2			//ÿCHANNEL�����õ�Զ����ַ����
#define DAC_CTRL_LEN				256			//����ָ��ṹ��Ч������󳤶�
#define DAC_CTRL_NUM				128			//����ָ�������󻺳��������
#define DAC_ALARM_SYNTAX_LEN		32			//�澯�﷨����
#define DAC_UNIT_LEN				8			//��λ����
#define DAC_RECV_BUFSIZE			8192		//���ջ�������С(8k)
#define DAC_SEND_BUFSIZE			8192		//���ͻ�������С(8k)
#define DAC_MSG_DATA_LEN			512			//�������ĵĴ�С(512bytes)
#define DAC_MSG_NUM					32			//ÿroute msg����

#define DAC_EIISERIAL_LEN			16			//eii�豸���кų���
#define DAC_EIIPARA_LEN				32			//eii�豸��������

//notify by wlq 20170307 ֧��1024ͨ��
#ifdef _ARM_LINUX
#define DAC_STATION_NUM				1		//��վ����
#define	DAC_CHANNEL_NUM				8		//ͨ������
#define	DAC_GROUP_NUM				128		//���������
#define	DAC_ROUTE_NUM				128		//·������
#define DAC_SENDDEV_NUM				128		//�������

#else

//#define DAC_STATION_NUM				256			//��վ����
//#define	DAC_CHANNEL_NUM				128		//ͨ������
//#define	DAC_GROUP_NUM				128		//���������
//#define	DAC_ROUTE_NUM				160		//·������
#define DAC_STATION_NUM				256			//��վ����
#define	DAC_CHANNEL_NUM				128		//ͨ������
#define	DAC_GROUP_NUM				128		//���������
#define	DAC_ROUTE_NUM				160		//·������
#define DAC_SENDDEV_NUM				128		//�������

#endif


#define	DAC_PROTOCOL_NUM			128			//��Լ����

//wfp add at 20110826
#define DAC_YK_NUM					2048		//ң�ظ���
#define DAC_YT_NUM					2048		//ң������

#define DAC_PROTO_RUN_DELAY			10000		//10ms

//���̼������--�޸����ݿ�
#define DAC_INTERVAL_1_MIN_VAL		1			//1���Ӵ��̵�
#define DAC_INTERVAL_5_MIN_VAL		5			//5���Ӵ��̵�
#define DAC_INTERVAL_10_MIN_VAL		10			//10���Ӵ��̵�
#define DAC_INTERVAL_15_MIN_VAL		15			//15���Ӵ��̵�
#define DAC_INTERVAL_30_MIN_VAL		30			//30���Ӵ��̵�
#define DAC_INTERVAL_HOUR_VAL		60			//ʱ�ɼ���
#define DAC_INTERVAL_DAY_VAL		1440		//�ղɼ���
#define DAC_INTERVAL_MONTH_VAL		17280		//�²ɼ���

#define DAC_OVER_INC_TIME			5			//ͻ������ʱ��


#endif //PDT_DAC_MACRO_H_

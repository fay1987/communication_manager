/*==============================================================================
* �ļ����� : dacmacro.h
* �ļ����� �����ݲɼ����ֺ궨��
* ģ�� : ���ݲɼ�
* ����ʱ�� : 2009-06-08
* ���� : wlq
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	PDT_PAS_MACRO_H_
#define	PDT_PAS_MACRO_H_

#define PAS_CODE_LEN				32			//���볤��
#define PAS_LCODE_LEN				64			//���볤��(�ӳ���)
#define	PAS_SNAME_LEN				32			//���ֳ���(short��)
#define	PAS_NAME_LEN				64			//���ֳ���
#define	PAS_LNAME_LEN				128			//���ֳ���(�ӳ���)
#define	PAS_DESC_LEN				64			//��������
#define PAS_LDESC_LEN				128			//��������(�ӳ���)
#define PAS_USER_LEN				64			//�û�������
#define PAS_PWD_LEN					64			//���볤��	
#define	PAS_ADDR_LEN				32			//��ַ����
#define	PAS_HOSTNAME_LEN			32			//����������
#define PAS_CTRL_LEN				256			//����ָ��ṹ��Ч������󳤶�
#define PAS_CTRL_NUM				128			//����ָ�������󻺳��������
#define PAS_ALARM_SYNTAX_LEN		32			//�澯�﷨����
#define PAS_UNIT_LEN				8			//��λ����

#define	PAS_GROUP_NUM				1024		//�㷨��������
#define	PAS_ROUTE_NUM				32			//�㷨�������
#define	PAS_PROTOCOL_NUM			32			//�㷨����


#define PAS_PROTO_RUN_DELAY			10000		//10ms

//�㷨��������
enum ALGORI_TASKTYPE
{
	TASKTYPE_PERIOD		=	0,			//����
	DATATYPE_TIMING		=	1,			//��ʱ
	DATATYPE_TRIGGER	=	2			//����
};

enum ALGORI_PRARTYPE
{
	PRARTYPE_INPUT		=	0,			//����
	PRARTYPE_OUTPUT						//���
	PRARTYPE_PRESETVALUE			//�̶�ֵ
};

enum ALGORI_MEASTYPE
{
	MEASTYPE_YC		=	0,			//ң��
	MEASTYPE_YX,					//ң��
	MEASTYPE_YK,					//ң��
	MEASTYPE_YT						//ң��
};


#endif //PDT_PAS_MACRO_H_

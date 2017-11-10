/*==============================================================================
* �ļ����� : scddef.h
* �ļ����� ��scada�������Ͷ���
* ģ�� : scada
* ����ʱ�� : 2009-08-08
* ���� : wfp
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_PDT_SCD_SCDDEF_H_
#define	_PDT_SCD_SCDDEF_H_

#include "dac/dac.h"
#include "scd/hisdatadef.h"
#include "event/csea.h"

namespace	PDT
{
	typedef	struct _scd_data_head_
	{
		nUInt32			dataclass;			//������������(�������ֱ��ο�SCD_DATA_TYPEö��)
	}SCD_DATA_HEAD;

	typedef struct _scd_data_yc_
	{
		nInt32			group;				//���
		nInt32			no;					//���
		nUInt32			dataType;			//��ʷ��������1Ϊ��Ч2Ϊ��Ч
		nUInt32			ycdataType;			//ң���������� 1-30Ϊ����ָ�� 31-34����ָ�� 35-40г��ָ�� 41-51����ָ��
		DAC_YC_DATA		data;
	}SCD_DATA_YC;

	//ͳ������,�������¼�ͳ��
	typedef struct _scd_data_sta_
	{
		nInt32			group;				//���
		HIS_STA_DATA	data;
	}SCD_DATA_STA;

	typedef struct _scd_data_event_
	{
		nInt32			evtClass;				//�����
		nInt32			evtType;				//��������
		nUInt32			occurTime;				//ʱ��
		char			staCode[DAC_NAME_LEN];	//��վ����
		char			staName[DAC_NAME_LEN];	//��վ����
		char			parentCode[DAC_NAME_LEN];//������
		char			parentName[DAC_NAME_LEN];//������
		char			code[DAC_NAME_LEN];		//����
		char			name[DAC_NAME_LEN];		//����
		char			user[16];				//�û���
		nUInt8			level;					//�ȼ�
		nUInt8			stateValue;				//��ǰ״ֵ̬
		nUInt8			stateValue1;			//״ֵ̬1
		nFloat			floatValue;				//��ǰ����ֵ
		nFloat			floatValue1;			//����ֵ1
		nUInt8			alarmFlag;				//�澯��־
		char			info[FULLDESCRIBE_LEN];	//��������
	}SCD_DATA_EVENT;

	//���̼������
	enum SCD_SAVE_INTERVAL
	{
		INTERVAL_1_MIN					=	0x00000001,	//1���Ӵ��̵�
		INTERVAL_5_MIN					=	0x00000002,	//5���Ӵ��̵�
		INTERVAL_10_MIN					=	0x00000004,	//10���Ӵ��̵�
		INTERVAL_15_MIN					=	0x00000008,	//15���Ӵ��̵�
		INTERVAL_30_MIN					=	0x00000010,	//30���Ӵ��̵�
		INTERVAL_HOUR					=	0x00000020,	//ʱ�ɼ���
		INTERVAL_DAY					=	0x00000040,	//�ղɼ���
		INTERVAL_MONTH					=	0x00000080,	//�²ɼ���
		INTERVAL_ALL					=	0xFFFFFFFF	//��Ӧ���м��
	};

	//YCԽ����������
	enum SCD_YC_OVERLIMIT_TYPE
	{
		YC_OVLIMIT_NORMAL				=	0,			//����
		YC_OVLIMIT_HIGH					=	1,			//Խ����
		YC_OVLIMIT_LOW					=	2,			//Խ����
		YC_OVLIMIT_HIHI					=	3,			//Խ������
		YC_OVLIMIT_LOLO					=	4			//Խ������
	};

	//����״̬
	enum BATTERY_STATE_TYPE
	{
		BAT_STATE_EQUAL					=		0,		//����
		BAT_STATE_DRIFT					=		1,		//����(����)
		BAT_STATE_DRIFT_Q				=		2,		//����(Ƿ��) 
		BAT_STATE_DRIFT_G				=		3,		//����(����)
		BAT_STATE_DYNAMIC				=		4,		//��̬�ŵ翪ʼ
		//BAT_STATE_DYNAMIC_END			=		5,		//��̬�ŵ����
		BAT_STATE_DISCHARGE				=		6,		//�ŵ�(�˶��Էŵ�/��̬�ŵ�)
		//BAT_STATE_DISCHARGE_END		=		7,		//�ŵ����(�˶��Էŵ�/��̬�ŵ�)
		BAT_STATE_BREAK					=		8,		//��· 
		BAT_STATE_OTHER 				=		250		//���� 
	};
	//������״̬
	enum BATTERY_SINGLE_STATE
	{
		BAT_SINGLE_NORMAL	=	0,	//����������״̬
		BAT_SINGLE_WH		=	1	//������ά��״̬
	};

	//��־����
	enum SCD_LOG_TYPE
	{
		LOG_SCD_BASE					=	2100,		//scd base
		LOG_SCD_HISDATA_TASK			=	2101,		//hisdata task
		LOG_SCD_CALC_TASK				=	2105,		//calc task
		LOG_SCD_LIMIT_TASK				=	2110,		//limit task
		LOG_SCD_EVENT_TASK				=	2120,		//event task
		LOG_SCD_FILEREAD_TASK			=	2130,		//file read task
		LOG_SCD_DBSAVE_TASK				=	2140,		//dbsave task
		LOG_SCD_DBSAVE_TASK_ERR			=	2141,		//dbsave task save error
		LOG_SCD_DBPROCEDURE_TASK		=	2145,		//dbprocedure task(ͳ�ƴ洢����)
		LOG_SCD_FILEREAD				=	2150,		//file read
		LOG_SCD_FILEWRITE				=	2151		//file write
	};

}

#endif //_PDT_SCD_SCDDEF_H_

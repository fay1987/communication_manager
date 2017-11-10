/*==============================================================================
* �ļ����� : hisdatainf.h
* �ļ����� ��������ʷ����/ͳ�Ʋ����ӿ�
* ģ�� : ��ʷͳ�ƽӿ�
* ����ʱ�� : 2009-06-08
* ���� : wfp
* �汾 : v1.0

* һ���޸ĺ�汾 : 1.1
* �޸�ժҪ : �Ժ���HIS_DATA_CLASS��HIS_DATA_TYPE��������
* �޸��� :  hll
* �޸����� : 2017-01-18 09:30:12

* �����޸ĺ�汾 : 1.2
* �޸�ժҪ : �Ժ���HIS_DATA_CLASS��������
* �޸��� :  hll
* �޸����� : 2017-02-10 15:32:16
==============================================================================*/
#ifndef	_PDT_SCD_HISDATA_DEF_H_
#define	_PDT_SCD_HISDATA_DEF_H_

#include "utl/datatype.h"
#include "dac/dacmacro.h"

#define HIS_STA_NUM			512								//ÿ�������ͳ�Ƹ���

//��ʷͳ��+����
typedef struct _his_sta_data_
{
	nInt32		no;			//���
	nUInt32		dataclass;	//��ʷ���ݵȼ�
	nUInt32		type;		//��ʷͳ������
	nFloat		val;		//ͳ��ֵ
	nUInt32		valstate;	//ֵ״̬(���ص�ʹ��)/yxֵ
	nUInt32		occurTime;	//��ǰֵ����ʱ�̣�ʹ������ֵ����ʱ��
	nUInt32		saveTime;	//����ʱ��
}HIS_STA_DATA;

//��ʷͳ�ƣ�����ͳ�վͳ�ƣ�
typedef struct _his_sta_rs_data_
{
	char		rsname[32];			//����ͳ�վ����
	char		code[32];			//ͳ��ֵ����
	nUInt32		dataclass;	//��ʷ���ݵȼ�
	nUInt32		type;		//��ʷͳ������
	nFloat		val;		//ͳ��ֵ
	nUInt32		valstate;	//ֵ״̬(���ص�ʹ��)/yxֵ
	nUInt32		occurTime;	//��ǰֵ����ʱ�̣�ʹ������ֵ����ʱ��
	nUInt32		saveTime;	//����ʱ��
}HIS_STA_RS_DATA;

//��ʷͳ��
typedef struct _his_sta_info_
{
	HIS_STA_DATA	data[HIS_STA_NUM];						//ͳ��ֵ
	hUInt32			write;									//дλ��
}HIS_STA_INFO;

//��ʷͳ�ƹ����ڴ�
typedef	struct _his_sta_shm_
{
	HIS_STA_INFO	info[DAC_GROUP_NUM];	
	HIS_STA_RS_DATA	rsdata[HIS_STA_NUM*10];
	hUInt32			rswrite;
}HIS_STA_SHM;

//�������ͼ���
enum HIS_DATA_CLASS
{
	HIS_CLASS_YC					=	1,		//ң����ʷ��������
	HIS_CLASS_EVENT					=	2,		//��ʷ����
	HIS_CLASS_DAY_STA				=	3,		//��ͳ��
	HIS_CLASS_MONTH_STA				=	4,		//��ͳ��
	HIS_CLASS_SEASON_STA			=	5,		//��ͳ��
	HIS_CLASS_YEAR_STA				=	6,		//��ͳ��
	HIS_CLASS_RUN					=	7,		//�������м��
	HIS_CLASS_CHARGE				=	8,		//��ŵ��������
	HIS_CLASS_DYNAMIC				=	9,		//��̬�ŵ��������
	HIS_CLASS_STATE					=	10,		//��ʷ״̬��
	HIS_CLASS_PEFORM_RESULT			=	11,		//���ܽ����
	HIS_CLASS_PEFORM_TMP_RESULT		=	12,		//���ܷ����м�����
	HIS_CLASS_RSYC                  =   13,     //��վ����ͳ����������    2017-01-18 09:30:12  by hll
	HIS_CLASS_RSDAY_STA             =   14,     //��վ������ͳ����������  2017-01-18 09:30:12  by hll
	HIS_CLASS_DAY_CAL               =   15,     //����塢�ȡ��⡢ƽ�����������С��ƽ��ֵ���� 2017/02/10  by hll 
	HIS_CLASS_MONTH_CAL             =   16      //��վ�����������С��ƽ��ֵ���� 2017/02/10  by hll 
	
};

//��ʷͳ������
enum HIS_DATA_TYPE
{
	//ң����ʷ���ݱ�ʱ����
	HIS_TYPE_REAL					=	1,		//��Чʵʱֵ
	HIS_TYPE_INVALID_REAL			=	2,		//��Чʵʱֵ
	HIS_TYPE_LOCAL_MANSET			=	3,		//�����˹�����
	HIS_TYPE_REMOTE_MANSET			=	4,		//Զ���˹�����
	HIS_TYPE_DEAD					=	5,		//����(��Ч)
	HIS_TYPE_NET_ERROR				=	6,		//�������
	HIS_TYPE_HIGH					=	7,		//ң��Խ����
	HIS_TYPE_LOW					=	8,		//ң��Խ����
	HIS_TYPE_VAL_NOQUAHIGH			=	9,		//��ѹԽ���ϸ�����
	HIS_TYPE_VAL_NOQUALOW			=	10,		//��ѹԽ���ϸ�����
	HIS_TYPE_MAN_MODIFY				=	11,		//�˹��޸�
	HIS_TYPE_MAN_INSERT				=	12,		//�˹�����
	//ң����ʷ���ݱ�����ֵ
	HIS_TYPE_HOUR_VAL				=	21,		//����ֵ
	HIS_TYPE_XB_HOUR_VAL			=	22,		//г������ֵ
	//ң����ʷ���ݱ�ƻ�ֵ
	HIS_TYPE_PLAN					=	31,		//�ƻ�ֵ
	//ң����ʷ���ݱ�ͳ��ֵ
	HIS_TYPE_STA_AVGVAL				=	41,		//ƽ��ֵ
	HIS_TYPE_STA_MAXVAL				=	42,		//���ֵ
	HIS_TYPE_STA_MAXTIME			=	43,		//���ʱ��
	HIS_TYPE_STA_MINVAL				=	44,		//��Сֵ
	HIS_TYPE_STA_MINTIME			=	45,		//��Сʱ��
	HIS_TYPE_STA_TOTALTIME			=	46,		//��ͳ��ʱ��
	HIS_TYPE_STA_QUARATE			=	47,		//�ϸ��ʣ�ƽ������
	HIS_TYPE_STA_QUATIME			=	48,		//�ϸ�ʱ��
	HIS_TYPE_STA_HIGHRATE			=	49,		//��������(�����)
	HIS_TYPE_STA_HIGHNUM			=	50,		//�����޴���
	HIS_TYPE_STA_HIGHTIME			=	51,		//������ʱ��
	HIS_TYPE_STA_HIHIRATE			=	52,		//����������
	HIS_TYPE_STA_HIHINUM			=	53,		//�������޴���
	HIS_TYPE_STA_HIHITIME			=	54,		//��������ʱ��
	HIS_TYPE_STA_LOWRATE			=	55,		//�������ʣ��ȵ�����
	HIS_TYPE_STA_LOWNUM				=	56,		//�����޴���
	HIS_TYPE_STA_LOWTIME			=	57,		//������ʱ��
	HIS_TYPE_STA_LOLOWRATE			=	58,		//����������
	HIS_TYPE_STA_LOLOWNUM			=	59,		//�������޴���
	HIS_TYPE_STA_LOLOWTIME			=	60,		//��������ʱ��
	HIS_TYPE_STA_POWERCUTTIME		=	61,		//��ͣ��ʱ��(����)
	HIS_TYPE_STA_LOADRATE			=	62,		//������
	HIS_TYPE_STA_DEADTIME			=	63,		//������ʱ��

	//���� (2017/1/18 11:14  by hll  ͳ�ƴ���ֵ)
	HIS_TYPE_VALLEYVAL              =   70, //��    105��106��107
	HIS_TYPE_PEAKVAL                =   71, //��
	HIS_TYPE_SHARPVAL               =   72, //��
	HIS_TYPE_FLATVAL                =   73, //ƽ
	HIS_TYPE_DAY_POWER              =   74,  //�ձ��������  177
	HIS_TYPE_MONTH_POWER            =   75,  //�±��������  178
	HIS_TYPE_YEAR_POWER             =   76,  //����������  179
	HIS_TYPE_RSHOUR_VAL             =   77,  //ͳ��������ֵ  




	//��ͳ�Ʊ�
	HIS_TYPE_DAY_START				=	100,	//��ͳ����ʼ���ͺ�
	HIS_TYPE_DAY_AVGVAL				=	101,	//��ƽ��ֵ
	HIS_TYPE_DAY_MAXVAL				=	102,	//�����ֵ�����ֵ����ʱ��
	HIS_TYPE_DAY_MINVAL				=	103,	//����Сֵ����Сֵ����ʱ��
	HIS_TYPE_DAY_TOTALTIME			=	104,	//��ͳ��ʱ��
	HIS_TYPE_DAY_QUARATE			=	105,	//�պϸ��ʣ���ƽ������
	HIS_TYPE_DAY_QUATIME			=	106,	//�պϸ�ʱ��
	HIS_TYPE_DAY_HIGHRATE			=	107,	//�ճ������ʣ��շ������
	HIS_TYPE_DAY_HIGHNUM			=	108,	//�ճ����޴���
	HIS_TYPE_DAY_HIGHTIME			=	109,	//�ճ�����ʱ��
	HIS_TYPE_DAY_HIHIRATE			=	110,	//�ճ���������
	HIS_TYPE_DAY_HIHINUM			=	111,	//�ճ������޴���
	HIS_TYPE_DAY_HIHITIME			=	112,	//�ճ�������ʱ��
	HIS_TYPE_DAY_LOWRATE			=	113,	//�ճ������ʣ��չȵ�����
	HIS_TYPE_DAY_LOWNUM				=	114,	//�ճ����޴���
	HIS_TYPE_DAY_LOWTIME			=	115,	//�ճ�����ʱ��
	HIS_TYPE_DAY_LOLOWRATE			=	116,	//�ճ���������
	HIS_TYPE_DAY_LOLOWNUM			=	117,	//�ճ������޴���
	HIS_TYPE_DAY_LOLOWTIME			=	118,	//�ճ�������ʱ��
	HIS_TYPE_DAY_POWERCUTTIME		=	119,	//��ͣ��ʱ��(����)
	HIS_TYPE_DAY_LOADRATE			=	120,	//�ո�����
	HIS_TYPE_DAY_XB_AVGVAL			=	121,	//��г��ƽ��ֵ
	HIS_TYPE_DAY_XB_MAXVAL			=	122,	//��г�����ֵ
	HIS_TYPE_DAY_XB_QUARATE			=	123,	//��г���ϸ���
	HIS_TYPE_DAY_XB_HIGHTIME		=	124,	//��г��������ʱ��
	HIS_TYPE_DAY_END				=	129,	//��ͳ�ƵĽ������ͺ�
	//��ͳ��ֵ
	HIS_TYPE_MONTH_START			=	130,	//��ͳ����ʼ���ͺ�
	HIS_TYPE_MONTH_AVGVAL			=	131,	//��ƽ��ֵ
	HIS_TYPE_MONTH_MAXVAL			=	132,	//�����ֵ�����ֵ����ʱ��
	HIS_TYPE_MONTH_MINVAL			=	133,	//����Сֵ����Сֵ����ʱ��
	HIS_TYPE_MONTH_TOTALTIME		=	134,	//��ͳ��ʱ��
	HIS_TYPE_MONTH_QUARATE			=	135,	//�ºϸ��ʣ���ƽ������
	HIS_TYPE_MONTH_QUATIME			=	136,	//�ºϸ�ʱ��
	HIS_TYPE_MONTH_HIGHRATE			=	137,	//�³������ʣ��·������
	HIS_TYPE_MONTH_HIGHNUM			=	138,	//�³����޴���
	HIS_TYPE_MONTH_HIGHTIME			=	139,	//�³�����ʱ��
	HIS_TYPE_MONTH_HIHIRATE			=	140,	//�³���������
	HIS_TYPE_MONTH_HIHINUM			=	141,	//�³������޴���
	HIS_TYPE_MONTH_HIHITIME			=	142,	//�³�������ʱ��
	HIS_TYPE_MONTH_LOWRATE			=	143,	//�³������ʣ��¹ȵ�����
	HIS_TYPE_MONTH_LOWNUM			=	144,	//�³����޴���
	HIS_TYPE_MONTH_LOWTIME			=	145,	//�³�����ʱ��
	HIS_TYPE_MONTH_LOLOWRATE		=	146,	//�³���������
	HIS_TYPE_MONTH_LOLOWNUM			=	147,	//�³������޴���
	HIS_TYPE_MONTH_LOLOWTIME		=	148,	//�³�������ʱ��
	HIS_TYPE_MONTH_POWERCUTTIME		=	149,	//��ͣ��ʱ��(����)
	HIS_TYPE_MONTH_LOADRATE			=	150,	//�¸�����
	HIS_TYPE_MONTH_XB_AVGVAL		=	151,	//��г��ƽ��ֵ
	HIS_TYPE_MONTH_XB_MAXVAL		=	152,	//��г�����ֵ
	HIS_TYPE_MONTH_XB_QUARATE		=	153,	//��г���ϸ���
	HIS_TYPE_MONTH_XB_HIGHTIME		=	154,	//��г��������ʱ��
	HIS_TYPE_MONTH_END				=	159,	//��ͳ�ƵĽ������ͺ�
	//��ͳ��ֵ
	HIS_TYPE_SEASON_START			=	160,	//��ͳ����ʼ���ͺ�
	HIS_TYPE_SEASON_AVGVAL			=	161,	//��ƽ��ֵ
	HIS_TYPE_SEASON_MAXVAL			=	162,	//�����ֵ�����ֵ����ʱ��
	HIS_TYPE_SEASON_MINVAL			=	163,	//����Сֵ����Сֵ����ʱ��
	HIS_TYPE_SEASON_TOTALTIME		=	164,	//��ͳ��ʱ��
	HIS_TYPE_SEASON_QUARATE			=	165,	//���ϸ��ʣ���ƽ������
	HIS_TYPE_SEASON_QUATIME			=	166,	//���ϸ�ʱ��
	HIS_TYPE_SEASON_HIGHRATE		=	167,	//���������ʣ����������
	HIS_TYPE_SEASON_HIGHNUM			=	168,	//�������޴���
	HIS_TYPE_SEASON_HIGHTIME		=	169,	//��������ʱ��
	HIS_TYPE_SEASON_HIHIRATE		=	170,	//������������
	HIS_TYPE_SEASON_HIHINUM			=	171,	//���������޴���
	HIS_TYPE_SEASON_HIHITIME		=	172,	//����������ʱ��
	HIS_TYPE_SEASON_LOWRATE			=	173,	//���������ʣ����ȵ�����
	HIS_TYPE_SEASON_LOWNUM			=	174,	//�������޴���
	HIS_TYPE_SEASON_LOWTIME			=	175,	//��������ʱ��
	HIS_TYPE_SEASON_LOLOWRATE		=	176,	//������������
	HIS_TYPE_SEASON_LOLOWNUM		=	177,	//���������޴���
	HIS_TYPE_SEASON_LOLOWTIME		=	178,	//����������ʱ��
	HIS_TYPE_SEASON_POWERCUTTIME	=	179,	//��ͣ��ʱ��(����)
	HIS_TYPE_SEASON_LOADRATE		=	180,	//��������
	HIS_TYPE_SEASON_END				=	189,	//��ͳ�ƵĽ������ͺ�
	//��ͳ��ֵ
	HIS_TYPE_YEAR_START				=	190,	//��ͳ����ʼ���ͺ�
	HIS_TYPE_YEAR_AVGVAL			=	191,	//��ƽ��ֵ
	HIS_TYPE_YEAR_MAXVAL			=	192,	//�����ֵ�����ֵ����ʱ��
	HIS_TYPE_YEAR_MINVAL			=	193,	//����Сֵ����Сֵ����ʱ��
	HIS_TYPE_YEAR_TOTALTIME			=	194,	//��ͳ��ʱ��
	HIS_TYPE_YEAR_QUARATE			=	195,	//��ϸ��ʣ���ƽ������
	HIS_TYPE_YEAR_QUATIME			=	196,	//��ϸ�ʱ��
	HIS_TYPE_YEAR_HIGHRATE			=	197,	//�곬�����ʣ���������
	HIS_TYPE_YEAR_HIGHNUM			=	198,	//�곬���޴���
	HIS_TYPE_YEAR_HIGHTIME			=	199,	//�곬����ʱ��
	HIS_TYPE_YEAR_HIHIRATE			=	200,	//�곬��������
	HIS_TYPE_YEAR_HIHINUM			=	201,	//�곬�����޴���
	HIS_TYPE_YEAR_HIHITIME			=	202,	//�곬������ʱ��
	HIS_TYPE_YEAR_LOWRATE			=	203,	//�곬�����ʣ���ȵ�����
	HIS_TYPE_YEAR_LOWNUM			=	204,	//�곬���޴���
	HIS_TYPE_YEAR_LOWTIME			=	205,	//�곬����ʱ��
	HIS_TYPE_YEAR_LOLOWRATE			=	206,	//�곬��������
	HIS_TYPE_YEAR_LOLOWNUM			=	207,	//�곬�����޴���
	HIS_TYPE_YEAR_LOLOWTIME			=	208,	//�곬������ʱ��
	HIS_TYPE_YEAR_POWERCUTTIME		=	209,	//��ͣ��ʱ��(����)
	HIS_TYPE_YEAR_LOADRATE			=	210,	//�긺����
	HIS_TYPE_YEAR_END				=	219		//��ͳ�ƵĽ������ͺ�
};

#endif	//_PDT_SCD_HISDATA_DEF_H_

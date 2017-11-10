/*==============================================================================
* �ļ����� : webdef.h
* ģ�� : web����
* ����ʱ�� : 2009-10-20 15:55:23
* ���� : ����ƽ
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
* ע�⣺���ļ�ʹ�õ�ʱ���ʽͳһΪ����1970������(UTLʱ��)
==============================================================================*/
#ifndef	_BFS_WEB_DEF_H_
#define	_BFS_WEB_DEF_H_

#include "utl/datatype.h"
#include "dac/dacmacro.h"

#define WEB_LOG_MAX_NUM			4096		//��־������������
#define WEB_LOG_TYPE_NUM		50000		//��־������ͺ�
#define WEB_LOG_DATA_LEN		1024		//��־���ݳ���
#define WEB_EVENT_LEN			256			//�������ݳ���
#define WEB_CTRL_COMMOND_LEN	200			//���������
#define BUFlEN_ACK				128			//����ack buf���� (cye add at 20100420)
#define WEB_CODE_LEN			32
#define WEB_NAME_LEN			64
#define WEB_DESCR_LEN			256

//ң�����ݽṹ
typedef struct _web_yc_data_ 
{
	int				no;
	float			val;			//ֵ
	int				quality;		//������
	int				updateTime;		//����1970�������
}WEB_YC_DATA;

//ң�����ݽṹ
typedef struct _web_yx_data_ 
{
	int				no;
	int				val;			//ֵ
	int				quality;		//������
	int				updateTime;		//����1970�������
}WEB_YX_DATA;

//���Ľ�������ṹ
typedef struct _web_message_
{
	hUInt32			time;						//���ͻ����ʱ��(����1970�������)��		�������� 
	hUInt8			type;						//֡����(�磺0:����1:����)
	char			desc[DAC_DESC_LEN];			//֡����
	hUChar			data[DAC_MSG_DATA_LEN];		//����
	hUInt32			length;						//����
}WEB_MESSAGE;

//ͨ��״̬
enum	WEB_CHANNEL_STATE
{
	WEB_CHANNEL_STATE_DOWN		=	0,			//�Ͽ�
	WEB_CHANNEL_STATE_LISTEN	=	1,			//����
	WEB_CHANNEL_STATE_UP		=	2,			//����
	WEB_CHANNEL_STATE_DISABLED	=	3,			//����
};

//ͨ����Ϣ
typedef struct _web_channel_info_
{
	hUInt16			state;						//״̬�����ӡ��Ͽ���
	hUInt32			lastUpTime;					//���һ����DOWN->UP��ʱ��
	hUInt32			lastDownTime;				//���һ����UP->DOWN��ʱ��
	hUInt32			txByteCnt;					//�����ֽ���
	hUInt32			rxByteCnt;					//�����ֽ���
	hUInt32			dayOkTime;					//������ʱ��
	hUInt32			dayErrCnt;					//�չ��ϴ���
	hUInt32			routeNum;					//��ͨ��������·������
}WEB_CHANNEL_INFO;

//·��״̬
enum	WEB_ROUTE_STATE
{
	WEB_ROUTE_STATE_DOWN		=	0,			//ֹͣ
	WEB_ROUTE_STATE_UP			=	1,			//����
	WEB_ROUTE_STATE_STANDBY		=	2,			//�ȱ�
	WEB_ROUTE_STATE_FREE		=	3,			//����
	WEB_ROUTE_STATE_DISABLED	=	4,			//����
};

//ROUTE��Ϣ
typedef struct _web_route_info_
{
	hUInt16			state;						//״̬
	hUInt32			lastUpTime;					//���һ����DOWN->UP��ʱ��
	hUInt32			lastDownTime;				//���һ����UP->DOWN��ʱ��
	hUInt32			lastDataOkTime;				//���һ��������ȷʱ��
	hUInt32			txFrmCnt;					//����֡��
	hUInt32			rxFrmCnt;					//����֡��
	hUInt32			okFrmCnt;					//���յ���ȷ֡��
	hUInt32			errFrmCnt;					//���յĴ���֡��
	hUInt32			errTime;					//�����ʸߵĳ���ʱ��
	hUInt32			dayOkTime;					//������ʱ��
	hUInt32			dayErrCnt;					//�չ��ϴ���
}WEB_ROUTE_INFO;

//event
typedef struct _web_event_
{
	hUInt16		sort;							//�¼������
	hUInt16		type;							//�¼�����
	hUInt32		updateTime;						//ʱ��(UTL)
	hUInt8		security;						//��ȫ��
	hUInt8		volLevel;						//��ѹ�ȼ�
	nUInt16		stateValue;						//��ǰ״ֵ̬
	nUInt16		stateValue1;					//��һ��״̬
	nFloat		floatValue;						//��ǰ����ֵ
	nFloat		floatValue1;					//��һ�θ���ֵ
	nUInt8		isRevert;						//�Ƿ񸴹�(yc,yx)
}WEB_EVENT;

//��ѹ�ϸ�����������
enum CMD_TYPE
{
	CMD_CALL_HOUR		=	0,		//���㲹��
	CMD_CALL_DAY_STA,				//��ͳ�Ʋ���
	CMD_CALL_DAY_STA_XB,			//��г��ͳ�Ʋ���
	CMD_CALL_MONTH_STA,				//��ͳ�Ʋ���
	CMD_CALL_MONTH_STA_XB,			//��г��ͳ�Ʋ���
	CMD_CALL_VERSION,               //��ѹ��汾(add lj20100106)
	CMD_SET_HOUR		=	10,		//��������
	CMD_SET_DAY_STA,				//��ͳ������
	CMD_SET_DAY_STA_XB,				//��г������
	CMD_SET_MONTH_STA,				//��ͳ������
	CMD_SET_MONTH_STA_XB,			//��г��ͳ������
	CMD_SET_RESTART,                //������add lj20100106��
	CMD_SET_VOLLEVEL,               //���õ�ѹ�ȼ�&�����ޣ�add lj20100106��
	CMD_SET_SYNCDATA,				//����ͬ��
	CMD_SET_TIME					//Уʱ
};

//����ͷ
typedef struct volhead
{
	hUInt32 type;					//��������--����ο�CMD_TYPE
	hUInt32 cmdtime;				//�����е�ʱ���������7-1����ͳ�ƣ�ʱ���ʽΪ��1970������
	hUInt32 luNo;					//�����Ӧ�����/·��
}VOLHEAD;

//��У���
typedef struct volack
{
	hUInt8 state;					//0--ʧ��;1--�ɹ�
}VOLACK;

//ͳ����������ṹ
typedef struct volsta	//��ѹ�ϸ���ͳ��ֵ
{
	hFloat	valavg;					//ƽ��ֵ
	hFloat	valmax;					//��ѹ���ֵ
	hFloat	valmin;					//��ѹ��Сֵ
	hUInt32 maxvoltime;				//���ֵ���ֵ�ʱ��
	hUInt32 minvoltime;				//��Сֵ���ֵ�ʱ��
	hFloat	checkout_qty;			//�ϸ���
	hUInt16 checkout_time;			//�ϸ�ʱ��
	hFloat	up_qty;					//��������
	hUInt16 up_time;  				//������ʱ��
	hFloat	low_qty;				//��������
	hUInt16 low_time;				//������ʱ��
	hUInt16 total_time;				//ͳ��ʱ��
}VOLSTA;

//г�����ݽṹ
typedef struct volsta_xb
{
	hFloat	xbavg;					//г��������ƽ��ֵ
	hFloat	xbmax;					//г�����������ֵ
	hUInt32	maxxbtime;				//���ֵ���ֵ�ʱ��
	hFloat	xbhgl;					//г�������ʺϸ���
	hFloat	xbuptime;				//г�������ʺϸ���
}VOLSTA_XB;

//��ѹ�ȼ�
typedef struct vol_level		
{
	hUInt32	level;					//��ѹ�ȼ�(V)
	hInt16	upLimit;				//����ֵ(��:7����level*(1+%7))
	hInt16	lowLimit;				//����ֵ(��:-7����level*(1-%7))
}VOL_LEVEL;

//Ŀǰֻ֧��һ������һ����
typedef struct vol_hour
{
	hFloat	hourval;				//����ֵ
}VOL_HOUR;


//---------------------------------------------------------
//���زɼ��豸(DJX&BMM&�ɼ���)��������
enum BTY_CMD_TYPE	
{
	//��������
	BTY_MAINTAIN_PLUS			=	0,		//����ά��
	BTY_MAINTAIN_DISCHARGE		=	1,		//�ŵ�ά��
	BTY_MAINTAIN_CHARGE			=	2,		//���ά��
	BTY_MAINTAIN_NORMAL			=	3,		//��ͨά��

	BTY_DISCHARGE_DYNAMIC		=	6,		//��̬�ŵ�
	BTY_DISCHARGE_STATIC		=	7,		//��̬�ŵ�
	BTY_DISCHARGE_STATIC_END	=	8,		//������̬�ŵ�
	BTY_TEST_RES				=	9,		//�������
	BTY_MAINTAIN_SYSTEM_END		=	10,		//ֹͣϵͳά��
	BTY_TEST_NULLOPEN			=	11,		//���Կտ�
        BTY_DISCHARGE_SYSPARAM                  = 12,             //���÷ŵ���� ����Ӣ 2011-6-3

	//�ٻ�����
	BTY_CALL_SYSPARAM			=	20,		//��ȡϵͳ��������������
	BTY_CALL_SERRATE			=	21,		//��ȡ���䷧ֵ����������
	BTY_CALL_FULLVOL			=	22,		//��ȡά��ʱ�����ѹֵ����������
	BTY_CALL_BENCHMARKRES		=	23,		//��ȡ��׼����ֵ
	BTY_CALL_RES				=	24,		//����ɼ�	
	BTY_CALL_VOL				=	25,		//��ѹ�ɼ�

	//��������
	BTY_SET_SYSPARAM			=	50,		//�趨ϵͳ��������������
	BTY_SET_SERRATE				=	51,		//�趨���䷧ֵ����������
	BTY_SET_FULLVOL				=	52,		//�趨ά��ʱ�����ѹֵ����������
	BTY_SET_RULERELVALUES		=	53,		//�趨��׼����ֵ����������

	//ά������
	BTY_COLL_RES				=	60,		//����һ������ɼ�����
	BTY_COLL_VOL				=	61,		// ����һ�ε�ѹ�ɼ�
	BTY_STR_SYS					=	62,		// ����һ��ϵͳά��
	BTY_STP_SYS					=	63		// ֹͣһ��ϵͳά��
};

//���زɼ��豸(DJX&BMM&�ɼ���)��������
enum BTY_PARAM_TYPE
{
	BTY_PARAM_VOL_UP		=	0,			//��ѹ���� 
	BTY_PARAM_VOL_LOW,						//��ѹ����
	BTY_PARAM_TEMP_UP,						//�¶�����
	BTY_PARAM_TEMP_LOW,						//�¶�����
	BTY_PARAM_CHARGECUR,					//����������
	BTY_PARAM_GROUPVOL_UP,					//��˵�ѹ����
	BTY_PARAM_GROUPVOL_LOW,					//��˵�ѹ����
	BTY_PARAM_RES_UP,						//������������
	BTY_PARAM_NUM,							//��ؽ��� 
	BTY_PARAM_GROUPNUM,						//�����������
	BTY_PARAM_CURMEASURE					//��������
};

typedef struct btyhead
{
	hUInt32		type;						//��������
	hUInt32		btyGroupNo;					//�������
	hUInt32		btyNo;						//���ؽں�
}BTYHEAD;

typedef struct btydischarge
{
	hUInt32		time;						//�ŵ�ʱ��������Ϊ��λ
	hUInt32		capacity;					//�ų�����
	float		stopvol;					//������ѹ
	float		stopgrpvol;					//������˵�ѹ
	float		currency;					//����
	float		remainingCapacity;			//ʣ������
	float		remainingTime;				//ʣ��ʱ��

}BTYDISCHARGE;

typedef struct btyparam
{
	hUInt32		paraType;					//��������
	hUChar		groupNo;					//������
	float		volUp;						//��ѹ���� 
	float		volLow;						//��ѹ����
	float		tempUp;						//�¶�����
	float		tempLow;					//�¶�����
	float		dischargeCurUp;				//����������
	float		groupVolUp;					//��˵�ѹ����
	float		groupVolLow;				//��˵�ѹ����
	float		oddResUp;					//������������
	hUInt32		btyNum;						//��ؽ��� 
	float		curMeasure;					//��������
}BTYPARAM;

typedef struct	btyvalue
{
	float	  val;
}BTYVALUE;

typedef struct	btymatain
{
	hUInt32	  maintainType;
}BTYMATAIN;

typedef struct btyack
{
	hUInt8		state;			//0--ʧ��;1--�ɹ�
	char        buff[BUFlEN_ACK];
}BTY_ACKCMD;

//---------------------------------------------------------
//�޹�����

//����ͷ
typedef struct cmd_wgbchead
{
	hUInt32 type;
	hUInt32 cmdtime;		//��1970������
}CMD_WGBCHEAD;

//��У���
typedef struct wgbcack
{
	hUInt8 state;			//0--ʧ��;1--�ɹ�
}WGBCACK;

enum WGBC_CMD_TYPE
{
	CMD_WGBCSETTIME,						//Уʱ
	CMD_WGBCSETSETTING,						//��������ֵ
	CMD_WGBCACTIONCAP,						//ң�ض�������
	CMD_WGBCRESET,							//��λ
	CMD_WGBCGETTIME							//���豸ʱ������(added by phs 20100714)
};

enum ACTIONMODE
{
	VOLTAGE = 1,						//��ѹ
	TIME,								//ʱ��
	VOLTIME,							//��ѹʱ��
	POWERFACTOR,						//��������
	REACTIVE							//��ѹ�޹�
};

enum CTL_OPTION
{
	noOperate,							//�޲���
	clearTotalTimes,					//��������ۼƴ���
	clearRunTime						//����������ۼ�Ͷ��ʱ��
};

enum ACTIONCAP_MODE						//�������ݶ�����ʽ
{
	ACTIONCAP_INPUT,
	ACTIONCAP_CUTOFF
};
typedef struct actionCap_web  
{
	char	no;							//�����������
	char	mode;						//������ʽ
}ACTIONCAP_WEB;
//��ѹ�޹�����������ֵweb�еĽṹ
typedef struct wgbcSet_web     
{
	hUInt16	volLevel;					//��ѹ�ȼ�
	float	volAmend;					//��ѹ����
	hUInt16 lineCurrentRatio;       	//��·�������
	hUInt16 capCurrentRatio;	    	//�������������
	float	currentRatioAmend_a;		//�����Ƚ���a
	float   currentRatioAmend_b;		//�����Ƚ���b
	float	currentRatioAmend_c;		//�����Ƚ���c
	float   currentRatioAmend_d;		//�����Ƚ���d
	float	inputLimit;					//Ͷ������
	float   cutoffLimit;				//�г�����
	float   inputTime_1;				//Ͷ��ʱ��1
	float	cutoffTime_1;				//�г�ʱ��1
	float	inputTime_2;				//Ͷ��ʱ��2
	float	cutoffTime_2;				//�г�ʱ��2
	float   overCurrentSet;				//�����趨
	float   overCurrentTimeSet;			//��������ʱ���趨
	float	quickBreakSet;				//�ٶ��趨
	float   quickBreakTimeSet;			//�ٶϳ���ʱ���趨
	float	underVoltageSet;            //Ƿѹ�趨
	float   underVolTimeSet;  		    //Ƿѹ����ʱ���趨
	float	overVoltageSet;             //��ѹ�趨
	float   overVolTimeSet;  		    //��ѹ����ʱ���趨
	hUInt16 switchRestrictions;			//��Ͷ�����ƴ���
	hUInt16	actionMode;					//������ʽѡ��
	float	switchAffect;		        //Ͷ��Ӱ��ֵ
	hUInt16	controllerOption;	        //����������ѡ��
	float   lowPowerFactor;				//������������
	float   upPowerFactor;              //������������
	float 	reactivePowerBacklash;		//�޹����ʻز�
	float   lowReactivePower;			//�޹���������
	float	capCapacity;				//1�����������
	hUInt16 protectionType;  			//��������
	//hUInt16	reservedByte;			//������
}WGBCSET_WEB;


/***********************************************************/
//�Ž�
typedef struct doorvalue
{
	hUInt16	 syspass;                    //ϵͳ����
    hUInt32  keypass;                   //��������
	hUInt8   msddqzx;                    //�ſڻ������̵���ִ��ʱ��
    hUInt8   ddjrys;                    // �ſڻ����ź�ȴ��������ʱʱ��
	hUInt8   hwbjsc;                     //�ſڻ����ⱨ������ı�ȷ��ʱ��
	hUInt8   bdbjys;                     //�趨�ſڻ����ر�������ʱʱ��
    hUInt8   hwjchddys;                  //�����غ�ĵȴ���ʱ
    hUInt8	 sfjkmkg;                   //�Ƿ����ſ���״̬
	hUInt8   sfjkhw;                    //�Ƿ��غ���
    hUInt8   mcszds;                       //������ڶϵ�󣨻�ӵ���Զ�����
	hUInt8   scxyjdxdj;                  //����������߶̽��Ƿ��ܿ���
	hUInt8   mcscdpzt;                  //�Ŵ������ƽ״̬
    hUInt8   hwgyqscdpzt;              //�����Ӧ�������ƽ״̬
	hUInt8   skhfkm;                    //ˢ���Ϸ��ܿ���
	hUInt8   ysjpentkm;                 //������̰�ENT����
	hUInt8   week1;						//��������Ϣ��һ
    hUInt8   week2;						//��������Ϣ�ն�
    hUInt8   holidaymonth;				 //�ڼ���(��)
    hUInt8   holiday;                  //�ڼ���(��)
	hUInt8   opendoor;                 //Զ�̿���     
    hUInt8   defaultopen;			   //��׼��ʱ�Ρ�����ʱ�����ó�ȱʡֵ  0:������ʱ����Ч,1: ��//�ù�����׼��ʱ�Σ�00��00��23��59ȫ���ܽ���


}DOORVALUE;

enum DOOR_CMD_TYPE	
{
	ORDER_DOORPASS       = 1,  //�޸�SM�ķ�������
	SET_DEFAULTTIME      = 2,  //��׼��ʱ�Ρ�����ʱ�����ó�ȱʡֵ
	SET_ALLTIMEINVALD    = 3,  // ������ʱ����Ч
	SET_OPENTIME         = 4,  // ���ù����յ�׼��ʱ��
	SET_MSDDQZX          = 5,  // �ſڻ������̵���ִ��ʱ��(�����趨)
	SET_DDJRYS           = 6,  // �ſڻ����ź�ȴ��������ʱʱ��(�����趨)
	SET_HWBJSC           = 7,  // �ſڻ����ⱨ������ı�ȷ��ʱ��(�����趨)
	SET_BDBJYS           = 8,  // �趨�ſڻ����ر�������ʱʱ��(�����趨)
	SET_HWJCHDDYS        = 9,  // �ſڻ����������غ�ĵȴ���ʱ
	SET_SFJKHW           = 10, // �ſڻ������صĲ����볷��
	SET_SFJKMKG          = 11, // �ſڻ����ſ���״̬��صĲ����볷��
	SET_WEEK             = 12, //�趨�ſڻ������ڵ���Ϣ��
	SET_ADDHOLIDAY       = 13, // �趨�ſڻ����ҷ����ڼ��գ���������������Ϣ�գ�
	SET_DELHOLIDAY       = 14, // ɾ���ſڻ��ڵ�һ��ڼ���
	SET_DELALLHOLIDAY    = 15, //����ſڻ��Ľڼ����б�(ȫ��ɾ��)
	SET_OPEN             = 16, //��һ����(����ϵͳ����Ա��Ϣ)
	SET_DOORADDR         = 17, // �����ſڻ���ַ������
	SET_ADDUSER          = 18, // ����һ���û������ſڻ���
	SET_DELUSERCARD      = 19, //ɾ���û��������ſ������ڣ�
	SET_DELUSERID        = 20, //ɾ���û���ţ�ID�������ſ������ڣ�
	SET_DELALLUSER       = 21,  //ȫ��ɾ���û������ſ������ڣ�
	SET_NOWORKOPEN		 = 31,	//���÷ǹ�����׼��ʱ���
	SET_MCHWGYQ			 = 32	//�ſڻ��Ŵš�����ȸ�Ӧ�������ԣ������趨��
		//yhh20110512 ȥ��һ������
	//READ_WORKOPENTIME    = 22, //yhh20110512 �ظ�
};
typedef struct opentime            //׼��ʱ�������
{
	hUInt8   isworkday;                     //0�Ƿǹ����գ��ǹ�����
	hUInt8   groupno;						//���        
	hUInt8	beginhour1;						//��ʼʱ���
	hUInt8	beginmin1;						//��ʼʱ���
	hUInt8	endhour1;						//����ʱ���
	hUInt8	endmin1;						//����ʱ���
	hUInt8	beginhour2;						//��ʼʱ���
	hUInt8	beginmin2;						//��ʼʱ���
	hUInt8	endhour2;						//����ʱ���
	hUInt8	endmin2;						//����ʱ���
	hUInt8	beginhour3;						//��ʼʱ���
	hUInt8	beginmin3;						//��ʼʱ���
	hUInt8	endhour3;						//����ʱ���
	hUInt8	endmin3;						//����ʱ���
	hUInt8	beginhour4;						//��ʼʱ���
	hUInt8	beginmin4;						//��ʼʱ���
	hUInt8	endhour4;						//����ʱ���
	hUInt8	endmin4;						//����ʱ���
}OPENTIME;

typedef struct userparam				//�û�������
{
	unsigned char cardid[16];				//��Ƭ���
	hUInt32 userid;							//�û����ID        
	hUInt16 userpass;                       //����
	hUInt16	year;							//��Ч����
	hUInt8	month;							//��Ч����
	hUInt8	day;							//��Ч����
	hUInt8  permission;						//Ȩ��
}USERPARAM;

//����ͷ
typedef struct doorhead
{
	hUInt8  grpno;                      //���������
	hUInt8  dooraddr;                        //�ſڻ���ַ
	hUInt32 type;                          //��������
}DOORHEAD;

//��У���
typedef struct doorack
{
	hUInt8 state;			//0--ʧ��;1--�ɹ�
	char        buff[BUFlEN_ACK];
}DOORACK;
//--------------------------------------------------------------------------

//��������������
enum PLD_CMD_TYPE
{
	CMD_WRITERUN	  =0,		//д����״̬
	CMD_START         =1,		//�ָ���ʼ��
	CMD_RESET         =2,		//���ǿ�Ƹ�λ
	CMD_WRITEPARA     =3,		//����ֵ����
	CMD_READPARA      =4		//������ֵ
	
};

//����ͷ
typedef struct xdc_head
{
	hUInt8  grpno;                      //������
	hUInt8  singleno;                   //��ؽں�
        int CmdNo;			    //���õĲ������
}XDC_HEAD;
// ����ؽں�Ϊ0ʱ��ʾ�㲥���������������ã��ӻ���ַ����������һ�����ã�������������һ�ζ�ȡ

//��У���
typedef struct pldack
{
	hUInt8 state;			//0--ʧ��;1--�ɹ�
	char        buff[BUFlEN_ACK];
}PLDACK;

//����������
typedef struct Remotecontrol			//ң�ز���
{
	int setval;				//���ò���ֵ

}REMOTECONTROL;
/*�������Ϊ0ʱ��
  Setval = 0   ��ѹ
         = 1   ������
         = 2   ����
         = 3   ����
�������Ϊ1ʱ��
Setval = 1   �ָ���ʼ�� 
�������Ϊ2ʱ��
Setval = 1   ���ǿ�Ƹ�λ */


typedef struct Setvalue	//ң������					
{
	int  Address;						//�ӻ���ַ
	int  BaudRate;						//������ѡ��
	int  ADVoltagemin;					//AD ��ѹ��Ͳο�ֵ
	int  ADVoltagemax;					//AD ��ѹ��߲ο�ֵ
	int ADTemperaturemin;					//AD �¶���Ͳο�ֵ
	int ADTemperaturemax;					//AD �¶���߲ο�ֵ
	int ADIdlemin;						//AD ������Ͳο�ֵ
	int ADIdlemax;						//AD ������߲ο�ֵ
	int jyqwdy;						//��ѹ������ѹ
	int cnzfddlkzdy;					//������ŵ�������Ƶ�ѹ
	int cnzfdsj;						//������ŵ�ʱ��
	int jyfdzdz;						//��ѹ�ŵ����ֵ
	int wdbjsx;						//�¶ȱ�������
	int hrfddlkz;						//���ݷŵ�������Ƶ�ѹ
	int hrfdxxz;						//���ݷŵ�����ֵ
}SETVALUE;



#endif

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
#include "dacmacro.h"
//---------------------------------------------------------------------------------------
//����ɨ�����ڲ���
typedef	struct _dac_scan_cycle_
{
	nUInt32	timesync;			//ʱ��ͬ������λ:�룩
	nUInt32	all_data;			//ȫ���ݣ���λ:�룩
	nUInt32	all_yc;				//ȫYC����λ���룩
	nUInt32	all_yx;				//ȫYX����λ���룩
	nUInt32	all_kwh;			//ȫKWH����λ���룩
	nUInt32	change_data;		//�仯����
	nUInt32	change_yc;			//�仯YC����λ���룩
	nUInt32	change_yx;			//�仯YX����λ���룩
	nUInt32	change_kwh;			//�仯KWH����λ���룩
	nUInt32	soe;				//SOE����λ���룩
}DAC_SCAN_CYCLE;
//ǰ������
typedef struct _dac_command_
{
	nBool		timesync;			//ʱ��ͬ��
	nBool		all_data;			//ȫ����
	nBool		all_yc;				//ȫYC
	nBool		all_yx;				//ȫYX
	nBool		all_kwh;			//ȫKWH
	nBool		change_data;		//�仯����
	nBool		change_yc;			//�仯YC
	nBool		change_yx;			//�仯YX
	nBool		change_kwh;			//�仯KWH
	nBool		soe;				//SOE
}DAC_COMMAND;
//ǰ�ÿ���ָ��(ң�ء�ң������㡢ֱ������ȵ�)
typedef	struct _dac_ctrl_cmd
{
	nBool		valid;						//����ռ��Ƿ�ռ��,true-�Ѿ���ռ��,false-δ��ռ��
	nUInt32		type;						//����
	nUInt32		groupNo;					//�����
	nUInt32		inputTime;					//�����ӽ���ʱ��ʱ��(time(NULL)������)�����ڳ�ʱ����
	nUInt32		len;						//����������������Ч�ֽڳ���
	nUInt8		buf[DAC_CTRL_LEN];			//�������������(��� dacctrl.h,ctrl_head + other cmd struct )
}DAC_CTRL_CMD;
//=======================================================================================
//ϵͳ����
typedef	struct _dac_system_
{
	char		name[DAC_NAME_LEN];				//����
	char		desc[DAC_DESC_LEN];				//����
}DAC_SYSTEM;
//��վ��
typedef struct _dac_station_
{
	hBool		valid;									//��Ч��־
	char		code[DAC_CODE_LEN];						//����
	char		name[DAC_NAME_LEN];						//����
	hInt32		level;									//�ȼ�
	char		address[DAC_NAME_LEN];					//λ��
	char		sgzCode[DAC_NAME_LEN];					//�¹��ܱ���(grpcode.code)
	char		mrid[DAC_NAME_LEN];						//ͳһ����(wfp add at 20110826)
	long		cmpynum;								//��ҵ����
	int			cmpyid;									//��ҵ��ʶ
}DAC_STATION;

typedef	struct _dac_recvdev_
{
	hBool		valid;									//��Ч��־
	char		code[DAC_CODE_LEN];						//����
	char		name[DAC_NAME_LEN];						//����
	hInt32		recvno;									//����װ�ñ��
	char		addr[DAC_ADDR_LEN];						//�ϼ���ص���
	int			frequency;								//Ƶ��
	hBool		isInit;									//�Ƿ���������޸Ĺ���λ��Ƶ�ʵ�ַ
	char		chancode[DAC_CODE_LEN];					//����ͨ��
}DAC_RECVDEV;

typedef	struct _dac_senddev_
{
	hBool		valid;									//��Ч��־
	char		code[DAC_CODE_LEN];						//����
	char		name[DAC_NAME_LEN];						//����
	hInt32		seeno;									//��ص���
	char		upseecode[DAC_CODE_LEN];				//�ϼ���ص���
	char		rdevcode[DAC_CODE_LEN];					//��Ӧ����װ��
	char 		cmac[DAC_MACADDRESS_LEN];
	int			frequency;
	int			rtu;
	hBool		isInit;									//�Ƿ���������޸Ĺ���λ��Ƶ�ʵ�ַ
	char		grpcode[DAC_CODE_LEN];					//����������
}DAC_SENDDEV;

//ͨ����
typedef struct _dac_channel_
{
	hBool		valid;									//��Ч��־
	char		code[DAC_CODE_LEN];						//����
	char		name[DAC_NAME_LEN];						//����
	hUInt8		type;									//���ͣ�0�����ڣ�1��client��2��server��3��udp��
	hUInt8		datatype;								//�������ͣ�0����ͨ��1��eii��2����磩
	char		localAddr[DAC_ADDR_LEN];				//���ص�ַ
	char		remoteAddr[DAC_ADDR_NUM][DAC_ADDR_LEN];	//Զ�˵�ַ1
	hUInt32		timeout;								//��ʱʱ��(s)
	hUInt32		switchTimeOut;							//��·���л���ʱʱ��
	char		eiiSerialNo[DAC_EIISERIAL_LEN];			//eii�豸���к�
	char		eiiPara[DAC_EIIPARA_LEN];				//eii�豸����
	hBool		isEvent;								//�Ƿ��������
	hUInt8		eventLevel;								//�����ȼ�
	hBool		isMsgAlarm;								//�Ƿ���Ÿ澯
	hUInt8		msgAlarmLevel;							//���ű����ȼ�
	hBool		reserve1;								//Ԥ��1
	hUInt8		reserve2;								//Ԥ��2
	hUInt8		initState;								//��ʼ״̬��0:�Ͽ���1:���ӣ�
}DAC_CHANNEL;
//�������
typedef struct _dac_group_
{
	hBool		valid;						//��Ч��־
	char		code[DAC_CODE_LEN];			//����
	char		name[DAC_NAME_LEN];			//����
	hInt32		station;					//��վ
	hUInt8		type;						//���ͣ�0�����գ�1��ת����
	hUInt32		ycNum;						//YC������
	hUInt32		yxNum;						//YX������
	hUInt32		kwhNum;						//KWH������
	hUInt32		deadTime;					//����ʱ��(��λ���룬����������)
	hBool		isEvent;					//�Ƿ��������
	hUInt8		eventLevel;					//�����ȼ�
	hBool		isMsgAlarm;					//�Ƿ���Ÿ澯
	hUInt8		msgAlarmLevel;				//���ű����ȼ�
	char		mrid[DAC_NAME_LEN];			//ͳһ����(wfp add at 20110826)
	hBool		reserve1;					//Ԥ��1
	hUInt8		reserve2;					//Ԥ��2
	hUInt8		initState;					//��ʼ״̬��0����ֹͣ��1�������У�
}DAC_GROUP;
//·������
typedef struct _dac_route_
{
	hBool			valid;						//��Ч��־
	char			code[DAC_CODE_LEN];			//����
	char			name[DAC_NAME_LEN];			//����
	hUInt8			type;						//���ͣ�0�����գ�1��ת����
	hInt32			group;						//���
	hInt32			channel;					//ͨ����
	hUInt32			localAddr;					//����վַ
	hUInt32			remoteAddr;					//Զ��վַ
	hInt32			protocol;					//��Լ����
	char			featureCode[DAC_CODE_LEN];	//��Լ��������
	DAC_SCAN_CYCLE	scanInterval;				//����ɨ����
	hBool			timeSyncFlag;				//ʱ��ͬ����־
	hBool			isReadOnly;					//ֻ����־(0:��1����)
	hUInt32			stopLimit;					//ֹͣʱ��
	hUInt32			errRateLimit;				//��������ֵ
	hUInt32			errTimeLimit;				//��������ʱ��
	hUInt32			cmdTimeout;					//���У��ʱʱ��
	hUInt32			cmdRetryInterval;			//�����ط�ʱ����
	hUInt32			cmdRetryCnt;				//�����ظ�����
	hUInt8			timeMode;					//ʱ��ģʽ(0:����;1:��������)
	hBool			isEvent;					//�Ƿ��������
	hUInt8			eventLevel;					//�����ȼ�
	hBool			isMsgAlarm;					//�Ƿ���Ÿ澯
	hUInt8			msgAlarmLevel;				//���ű����ȼ�
	hBool			reserve1;					//Ԥ��1
	hUInt8			reserve2;					//Ԥ��2
	hUInt8			initState;					//��ʼ״̬
}DAC_ROUTE;
//��Լ��
typedef	struct _dac_protocol_
{
	hBool			valid;						//��Ч��־
	char			code[DAC_CODE_LEN];			//����
	char			name[DAC_NAME_LEN];			//����
	char			libName[DAC_SNAME_LEN];		//��Լ����
	char			featureTable[DAC_SNAME_LEN];//���������
}DAC_PROTOCOL;
//ǰ��ң�������
typedef	struct _dac_yc_
{
	hBool			valid;						//��Ч��־
	char			code[DAC_CODE_LEN];			//����
	char			name[DAC_NAME_LEN];			//����
	hBool			calc;						//�Ƿ������־(wfp add at 20111019)			
	char			formula[DAC_CODE_LEN];		//���㹫ʽ����
	hUInt8			type;						//����
	hInt32			srcGroup;					//Դ�����
	hInt32			srcNo;						//Դ���
	hBool			isEvent;					//�Ƿ��������
	hUInt8			eventLevel;					//�����ȼ�(wfp add 20110826)
	hUInt32			eventType;					//��������
	char			unit[DAC_UNIT_LEN];			//��λ
	hBool			isStat;						//�Ƿ�ͳ��
	hFloat			coefK;						//ϵ��
	hFloat			coefB;						//�ؾ�
	hFloat			zeroVal;					//��ֵ��Χ
	hFloat			maxChange;					//�������
	hUInt32			saveInterval;				//���̼��(min)
	hFloat			minChgVal;					//��ֵ
	hFloat			highLimit;					//����ֵ
	hFloat			lowLimit;					//����ֵ
	hFloat			hihiLimit;					//������
	hFloat			lolowLimit;					//������
	hUInt8			eventDelay;					//������ʱ(s)
	hFloat			maxIllegal;					//���Ƿ�ֵ
	hFloat			minIllegal;					//��С�Ƿ�ֵ
	hBool			isMsgAlarm;					//�Ƿ���ű���
	hUInt8			msgAlarmLevel;				//���ű����ȼ�
	hBool			isControl;					//�Ƿ�ң��
	hUInt8			ctrlType;					//ң������
	hInt32			ctrlNo;						//ң����
	char			mrid[DAC_NAME_LEN];			//ͳһ����(wfp add at 20110826)
	//info
	hBool			hasPutEvent;				//��ʱ�����Ƿ��Ѿ�����
	hUInt32			eventStartTime;				//������ʱ����ʼʱ��
	hUInt8			limitType;					//��ֵ����
	hUInt32			overIncCnt;					//ͻ�����
	//hBool			change;						//ֵ�仯��־,��dac��λ,scd��λ
	hUInt32			reserve1;					//Ԥ��1
	hUInt8			reserve2;					//Ԥ��2
}DAC_YC;
//ǰ��ң�Ų�����
typedef	struct _dac_yx_
{
	hBool			valid;						//��Ч��־
	char			code[DAC_CODE_LEN];			//����
	char			name[DAC_NAME_LEN];			//����
	hBool			calc;						//�Ƿ������־(wfp add at 20111019)	
	char			formula[DAC_CODE_LEN];		//���㹫ʽ����
	hUInt8			type;						//����
	hInt8			normal;						//����״ֵ̬(wfp add 20110826)
	hInt32			srcGroup;					//Դ�����
	hInt32			srcNo;						//Դ���
	hBool			isEvent;					//�Ƿ��������
	hUInt8			eventLevel;					//�����ȼ�(wfp add 20110826)
	hUInt32			eventType;					//��������
	hBool			isStat;						//�Ƿ�ͳ��
	hBool			reverse;					//ȡ����־
	hBool			isMsgAlarm;					//�Ƿ���ű���
	hUInt8			msgAlarmLevel;				//���ű����ȼ�
	hBool			isInSgz;					//�Ƿ����¹��ܵ������
	hBool			isControl;					//�Ƿ�ң��
	hUInt8			ctrlType;					//ң������
	hInt32			ctrlNo;						//ң�غ�
	char			mrid[DAC_NAME_LEN];			//ͳһ����(wfp add at 20110826)
	hBool			reserve1;					//Ԥ��1
	hUInt8			reserve2;					//Ԥ��2
}DAC_YX;
//ǰ�õ�Ȳ�����
typedef	struct _dac_kwh_
{
	hBool			valid;						//��Ч��־
	char			code[DAC_CODE_LEN];			//����
	char			name[DAC_NAME_LEN];			//����
	hInt32			srcGroup;					//Դ�����
	hInt32			srcNo;						//Դ���
	hFloat			coefK;						//ϵ��
	hFloat			coefB;						//�ؾ�
}DAC_KWH;
//ǰ���¼���
typedef	struct _dac_soe_
{
	hBool			valid;						//��Ч��־
	char			code[DAC_CODE_LEN];			//����
	char			alias[DAC_CODE_LEN];		//����
	hInt8			normal;						//����״ֵ̬(wfp add 20110826)
	hInt32			srcGroup;					//Դ�����
	hInt32			srcNo;						//Դ���
	hBool			isEvent;					//�Ƿ��������
	hUInt8			eventLevel;					//�����ȼ�(wfp add 20110826)
	hUInt32			eventType;					//��������
	hBool			isStat;						//�Ƿ�ͳ��
	hBool			isMsgAlarm;					//�Ƿ���ű���
	hUInt8			msgAlarmLevel;				//���ű����ȼ�
}DAC_SOE;
//ǰ��ң�ر�
typedef	struct _dac_yk_
{
	hBool			valid;						//��Ч��־
	char			routeCode[DAC_CODE_LEN];	//·������
	char			code[DAC_CODE_LEN];			//����������
	char			no;							//����������
	char			name[DAC_NAME_LEN];			//����
	hInt32			ykno;						//ң�����
	hInt32			yktype;						//ң������
	char			mrid[DAC_NAME_LEN];			//ͳһ����
}DAC_YK;
//ǰ��ң����
typedef	struct _dac_yt_
{
	hBool			valid;						//��Ч��־
	char			routeCode[DAC_CODE_LEN];	//·������
	char			code[DAC_CODE_LEN];			//����������
	char			no;							//����������
	char			name[DAC_NAME_LEN];			//����
	hInt32			ytno;						//ң�����
	char			mrid[DAC_NAME_LEN];			//ͳһ����
}DAC_YT;

//����Ҫʵʱ�ٻ��Ĳ��
typedef	struct _dac_yccon_
{
	hBool			valid;
	short			ntype;						//������� 0 ����  1 г��  2 װ�ò���   3 SOE  4 ¼��
	char			code[DAC_CODE_LEN];			//������
	char			name[DAC_NAME_LEN];			//�������
	short			ndatatype;					//�������� 0 ң��  1 ң��
}DAC_YCCON;
//=======================================================================================
//ϵͳ��Ϣ
typedef	struct _dac_system_info_
{
	hBool			serverFlag;					//�Ƿ���DAC������
	hUInt32			stationNum;					//ϵͳʵ�ʳ�վ����
	hUInt32			recvdevNum;					//ϵͳ����װ������
	hUInt32			senddevNum;					//ϵͳʵ�ʼ�����Ŀ
	hUInt32			channelNum;					//ϵͳʵ��ͨ����Ŀ
	hUInt32			groupNum;					//ϵͳʵ����������Ŀ
	hUInt32			routeNum;					//ϵͳʵ��·����Ŀ
	hBool			initFlag;					//��ʼ����־
	hUInt32			loadParaFlag;				//װ�ز������־������λ��ͬ���ֲ�����
}DAC_SYSTEM_INFO;
//ͨ����Ϣ
typedef struct _dac_channel_info_
{
	nUInt8			state;						//״̬�����ӡ��Ͽ���
	nUInt32			lastUpTime;					//���һ����DOWN->UP��ʱ��
	nUInt32			lastDownTime;				//���һ����UP->DOWN��ʱ��
	nUInt32			txByteCnt;					//�����ֽ���
	nUInt32			rxByteCnt;					//�����ֽ���
	nUInt32			dayOkTime;					//������ʱ��
	nUInt32			dayErrCnt;					//�չ��ϴ���
	nUInt8			reset;						//��λ��־
	nBool			manStop;					//�˹�ֹͣ��־
	nInt32			activeAddr;					//���ַ����remoteAddr�е���ţ�
	nUInt32			curRouteOrder;				//��ǰ·������routes�е���ţ����ڷ���cmd
	nUInt32			routeNum;					//��ͨ��������·������
	nInt32			routes[DAC_ROUTE_IN_CHANNEL];//��ͨ��������·����	
}DAC_CHANNEL_INFO;



//����װ��
typedef struct _dac_recvdev_info_
{
	nInt32			channo;							//��Ӧ��ͨ����
	nUInt32			SdevNum;						//�����װ�������ӵķ���װ�ø���
	nInt32			sdevs[DAC_ROUTE_IN_CHANNEL];	//�����װ�������ķ���װ�ú�	
}DAC_RECVDEV_INFO;

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
	nInt32			mainRoute;					//��·����
	DAC_COMMAND		txCmd;						//���͵�����
	DAC_COMMAND		rxCmd;						//���յ�����
}DAC_GROUP_INFO;
//ROUTE��Ϣ
typedef struct _dac_route_info_
{
	nUInt8			state;						//״̬
	nUInt32			lastUpTime;					//���һ����DOWN->UP��ʱ��
	nUInt32			lastDownTime;				//���һ����UP->DOWN��ʱ��
	nUInt32			lastDataOkTime;				//���һ��������ȷʱ��
	nUInt32			txFrmCnt;					//����֡��
	nUInt32			rxFrmCnt;					//����֡��
	nUInt32			okFrmCnt;					//���յ���ȷ֡��
	nUInt32			errFrmCnt;					//���յĴ���֡��
	nUInt32			errTime;					//�����ʸߵĳ���ʱ��
	nUInt32			dayOkTime;					//������ʱ��
	nUInt32			dayErrCnt;					//�չ��ϴ���
	nBool			manStop;					//�˹�ֹͣ��־
	nBool			switchFlag;					//�ɹ�Լ���ã�����һ��ͨ������·�����(end+timeout+err)
	nBool			delProtocol;				//����ͨѶ��Լ�����־
	nBool			initFlag;					//��ʼ����־
	nBool			mainFlag;					//��·����ʶ
	DAC_COMMAND		cmd;						//������Ϣ
}DAC_ROUTE_INFO;
//CHAN�仯��Ϣ
typedef	struct _dac_chg_chan_info_ 
{
	nUInt32				no;
	DAC_CHANNEL_INFO	info;
}DAC_CHG_CHANNEL_INFO;
//GROUP�仯��Ϣ
typedef	struct _dac_chg_group_info_
{
	nUInt32			no;
	DAC_GROUP_INFO	info;
}DAC_CHG_GROUP_INFO;
//ROUTE�仯��Ϣ
typedef struct _dac_chg_route_info_ 
{
	nUInt32			no;
	DAC_ROUTE_INFO	info;
}DAC_CHG_ROUTE_INFO;
//YCԭʼ���ݡ���ģ����
typedef struct _dac_yc_data_
{
	nFloat		val;							//ֵ
	nUInt32		quality;						//������
	nUInt32		updateTime;						//������ʱ�䣨0����Ч��
}DAC_YC_DATA;
//YXԭʼ���ݡ���������
typedef struct _dac_yx_data_
{
	nUInt8		val;							//ֵ
	nUInt32		quality;						//������
	nUInt32		updateTime;						//������ʱ�䣨0����Ч��
}DAC_YX_DATA;
//KWHԭʼ���ݡ����ۻ���
typedef struct _dac_kwh_data_
{
	nDouble		val;							//ֵ
	nUInt32		quality;						//������
	nUInt32		updateTime;						//������ʱ�䣨0����Ч��
}DAC_KWH_DATA;
//SOE����
typedef struct _dac_soe_data_
{
	nDateTime	soeTime;						//ʱ��
	nUInt32		no;								//���
	nUInt8		val;							//ֵ
	nUInt32		quality;						//״̬��������־��
}DAC_SOE_DATA;
//�仯YC
typedef	struct _dac_yc_chg_data_
{
	nUInt32			no;							//���
	nFloat			lastVal;					//��һ��ֵ
	DAC_YC_DATA		data;						//����
}DAC_YC_CHG_DATA;
//�仯YX
typedef	struct _dac_yx_chg_data_
{
	nUInt32			no;							//���
	nUInt8			lastVal;					//��һ��ֵ
	DAC_YX_DATA		data;						//����
}DAC_YX_CHG_DATA;
//�仯KWH
typedef	struct _dac_kwh_chg_data_
{
	nUInt32			no;							//���
	nDouble			lastVal;					//��һ��ֵ
	DAC_KWH_DATA	data;						//����
}DAC_KWH_CHG_DATA;
//-------------------------------------------------------------------------------
//(6)���ձ��Ļ���
typedef struct _dac_recv_buffer_
{ 
	nUChar			data[DAC_RECV_BUFSIZE];		//���ջ�������16k
	nUInt32			write;						//���ջ���дָ��  
	hUInt32			read;						//���ջ����ָ�� yff
}DAC_RECV_BUFFER;

//(7)���ͱ��Ļ���
typedef struct _dac_send_buffer_
{
	nUChar			data[DAC_SEND_BUFSIZE];		//���ͻ�������
	nUInt32			write;						//���ͻ���дָ��
	hUInt32			read;						//���ͻ����ָ�� yff

}DAC_SEND_BUFFER;

//���Ľ�������ṹ
typedef struct _dac_message_
{
	nDateTime		time;						//���ͻ����ʱ�䡡����		�������� 
	nUInt8			type;						//֡����(�磺0:����1:����)
	char			desc[DAC_DESC_LEN];			//֡����
	nUChar			data[DAC_MSG_DATA_LEN];		//�����
	nUInt32			length;						//�����
}DAC_MESSAGE;
//���Ľ��������� 
typedef struct _dac_message_buffer_
{
	DAC_MESSAGE			message[DAC_MSG_NUM];
	hUInt32				write;							//дָ��
}DAC_MESSAGE_BUFFER;

//��������
typedef	struct _dac_com_para_
{
	DAC_SYSTEM			system;							//ϵͳ����
	DAC_STATION			station[DAC_STATION_NUM];		//��վ��
	DAC_SENDDEV			senddev[DAC_SENDDEV_NUM];		//����
	DAC_RECVDEV         recvdev[DAC_RECVDEV_NUM];		//����װ��
	DAC_CHANNEL			channel[DAC_CHANNEL_NUM];		//ͨ����
	DAC_GROUP			group[DAC_GROUP_NUM];			//�������
	DAC_ROUTE			route[DAC_ROUTE_NUM];			//·����
	DAC_PROTOCOL		protocol[DAC_PROTOCOL_NUM];		//��Լ��
	DAC_YK				yk[DAC_YK_NUM];					//ң�ر�--��ң�ⲻͬ����no��
	DAC_YT				yt[DAC_YK_NUM];					//ң����--��ң�ⲻͬ����no��
	DAC_YCCON			yccon[DAC_YK_NUM];				//��㳣����
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
	DAC_RECVDEV_INFO	recvdev[DAC_RECVDEV_NUM];		//����װ�ñ�
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
	size_t				para[DAC_GROUP_NUM];		//������ָ��ƫ��
	size_t				data[DAC_ROUTE_NUM];		//������ָ��ƫ��
	DAC_MAP_INFO		mapInfo[DAC_GROUP_NUM];		//��ǰMAP��Ϣ
}DAC_SHM_SOE;

#endif //PDT_DAC_H_

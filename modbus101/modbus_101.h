#ifndef _PDT_DAC_MODBUS_101_H_
#define	_PDT_DAC_MODBUS_101_H_

#include "dac/protocol.h"
#include <ace/config-all.h>
#include "utl/intervaltimerset.h"
#include "rdbop/rdbop.h"
#include <QVector>
#include <QMap>

//=============================================================================
#if defined (_MODBUS_COSTOMIZE_)
# define	MODBUS_101_EXPORT	ACE_Proper_Export_Flag
#else
#  define	MODBUS_101_EXPORT	ACE_Proper_Import_Flag
#endif
//=============================================================================

namespace	PDT
{
	enum ProtocolEvent
	{
		Succeeded = 0,
		Waitting,
		TimeOut,
		CheckError,
		RetryCountOver
	};

	enum ProtocolCheckFunc
	{
		CRC16 = 0,
		CDT,
		SUM
	};

	enum ProtocolFrmType
	{
		Analog = 0 ,		//0ң��
		Discrete,			//1ң��
		Control,			//2ң��
		SetValue,			//3ң��
		SyncTime,			//4��ʱ
		YcCommand,			//5
		ElecCommand,		//6��������
		HarmonicCmd,		//7г��
		ParameterCmd,		//8����
		ConstantValue,
		SOEData,
		RecordsData,
		RecordsData_SOE,
		RecordsData_YX,
		RecordsData_Para,
		RecordsData_Record,
		EditDFqcy,			//�޸���λ��Ƶ�ʵ�ַ
	};

	enum ManualCommandType
	{
		CTRL_PRO_CONSTANTVALUE			=	214,		//���ö�ֵ
		CTRL_PRO_YCCOMMAND				/*=	215*/,		//����ң������
		CTRL_PRO_CALLSOEDATA			/*=	216*/,		//�ٻ�SOE����
		CTRL_PRO_CALLRECORDSDATA		/*= 217*/,		//�ٻ�¼������
		CTRL_PRO_CALLELEC				/*= 218*/,		//�ٻ�����
		CTRL_PRO_CALLHMC				/*=	219*/,		//�ٻ�г��
		CTRL_PRO_UDP					/*= 220*/,		//���͹㲥
		CTRL_PRO_EDITPRAR				/*= 221*/,		//�޸���λ��Ƶ�ε�ַ
		CTRL_PRO_EDITUDP				/*= 222*/,		//�޸���λ��Ƶ�ε�ַ
		CTRL_PRO_EDITPRAR1				/*= 223*/		//����һ�������ļ��޸���λ��Ƶ�ε�ַ
	};

#define LOG_PROTOCOL											20000

#define PMC_DESCRIBE_STRING_LENGTH								64
#define PMC_FEATURE_RECORD_MAXNUM								64
#define PMC_SEND_BUFFER_SIZE									512
#define PMC_RECV_BUFFER_SIZE									2048

#define	YK_ACTION_SELECT					0
#define	YK_ACTION_SELECT_V					1
#define	YK_ACTION_EXECUTE					2
#define	YK_ACTION_CANCEL					3

#define CONSTANTVALUE_READ					0x7A
#define CONSTANTVALUE_WRITE					0x7D

#define CTRL_PRO_CONSTANTVALUE				214


#define RECEIVE_SUCCESS			1
#define RECEIVE_UNFINISHED		2
#define RECEIVE_ERROR_LENGTH	3
#define RECEIVE_ERROR_CRC		4
#define RECEIVE_ERROR_FC		5
#define RECEIVE_ERROR_ROUTE		6






	typedef struct _ctrl_pro_yk_ex_ 
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
	}ctrl_pro_yk_ex;


	////��ֵ����
	//typedef struct _ctrl_pro_constantvalue_
	//{
	//	nInt32	groupNo;					//�������
	//	nUInt8	ctrlType;
	//	nUInt32	length;						//�ⲿ�Զ�������峤��
	//	char	cmdBuf[CTRL_COMMON_LEN];	//�ⲿ�Զ��������
	//}ctrl_pro_constantvalue;

	//typedef struct _ctrl_pro_constantvalue_ack_
	//{
	//	nInt32	groupNo;					//�������
	//	nUInt8	ctrlType;
	//	nUInt32	length;						//�ⲿ�Զ�������峤��
	//	char	ackBuf[CTRL_COMMON_LEN];	//�ⲿ�Զ��������
	//	nUInt8	ackState;					//����״̬
	//}ctrl_pro_constantvalue_ack;

	/*-
	 *--	�ʴ��Լ�����ṹ	
	 -*/	
	typedef struct _modbus_101_feature	
	{
		char		cmdDesc[PMC_DESCRIBE_STRING_LENGTH];		/*��������							*/
		hInt32		cmdNo;										/*����ţ�����������				*/
		hBool		isUsed;										/*�����Ƿ�ʹ��						*/
		hBool		isAck;										/*�����Ƿ���Ҫ�ظ�					*/
		hUChar		fc;											/*������							*/
		hUChar		fc2;										/*������2							*/
		hUChar		fcLength;									/*������λ����2-��λ��4-��λ��		*/
		hUChar		frmType;									/*֡���ͣ�0-yC��1-yX��2-yK��3-yT��	*/
		hUChar		addrH;										/*�Ĵ�����ַ��λ					*/
		hUChar		addrL;										/*�Ĵ�����ַ��λ					*/
		hUChar		lengthH;									/*���ݳ��ȸ�λ						*/
		hUChar		lengthL;									/*���ݳ��ȵ�λ						*/
		hInt32		routeNo;									/*����������·����					*/
		hInt32		startNo;									/*��֡ң���ң�ŵ���ʼ��ַ			*/
		hInt32		nextCmdNo;									/*��һ��ѯ��ָ������д-1��ʾ���жϣ�����ǰһ������Ѱ�ң��ڲ���ң�ص�ʱ�����ã�*/
		hUChar		checkFunc;									/*����У���㷨						*/
		ctrl_pro_yk		ykptr;
	}PMC_Feature, *PMC_Feature_ptr;

	typedef struct _modbus_101_current_send_command_
	{
		hUChar		cmdFunc;									/*����������1						*/
		hUChar		cmdFunc2;									/*����������2						*/
		hUChar		cmd[PMC_SEND_BUFFER_SIZE];					/*����								*/                                            
		hUInt16		length;										/*�����							*/
		char		cmdStr[PMC_DESCRIBE_STRING_LENGTH];			/*��������							*/
		PMC_Feature_ptr	currFeatureptr;							/*��ǰ���͵�����ָ��				*/
		hUInt32		retryCount;									/*�ط�����							*/
	}PMC_Current_Send_Command,*PMC_Current_Send_Command_ptr;

	typedef struct _modbus_101_recv_msg_
	{
		hUInt16		length;
		hUChar		msg[PMC_RECV_BUFFER_SIZE];
	}PMC_Recv_Msg,*PMC_Recv_Msg_ptr;

	struct sendpara
	{
		char 	cmac[DAC_MACADDRESS_LEN];
		int		frequency;
		int		rtu;
	};

	class MODBUS_101_EXPORT	CProto_Modbus_101 : public PDT::CProtocol
	{
	public:
		CProto_Modbus_101();
		virtual ~CProto_Modbus_101();

		//�Ƿ��
		virtual	bool	isOpen() const;
		//�򿪹�Լ
		virtual bool	open();
		//�رչ�Լ
		virtual void	close();
		//���մ������
		virtual void	mainRx();

		//���ʹ������
		virtual void	mainTx();
	protected:
	private:
		//��ʼ����ȡ���ݿ�
		bool			readFeature( DAC_ROUTE	*m_proute );										/*��ȡ��Լ������				*/
		void			addFeatureYKYT();															/*�ڹ�Լ������������������Ϣ��Ϊң�غ�ң������	*/
		void			createSTMatrix();															/*���ݹ�Լ��������״̬ת�ƾ���*/

		//״̬ת�ƾ�����������ݵ�ǰ�������͹�Լ����Ѱ����һ֡Ӧ������
		void			findNextCommand();

		//���ɱ���
		void			makeCommand( PMC_Feature_ptr p );									/*�������������������·�����	*/
		bool			make_yk_select(void* p);
		bool			make_yk_execute(void* p);
		bool			make_yk_cancel(void* p);
		bool			make_select_parameter(void* p);
		bool			make_set_parameter(void* p);
		bool			sendTime();

		bool			make_constantValue_read(ctrl_pro_constantvalue* p);
		bool			make_constantValue_write(ctrl_pro_constantvalue* p);
		bool			SetCVCmdAck(const ctrl_pro_constantvalue_ack* pAck);


		//���ͱ���
		void			setCmdinfoToBuf( );															/*�ѷ��ͱ������뵽����ͨ������	*/
		//���Ϳ��Ƶ�ʱ��������
		bool			checkCurrentCommand();											/*��鵱ǰӦ���͵����������Ϊ���������·�����*/
		bool			hasAnyManualCommand();											/*����ʱ��鵱ǰʩ�����˹�����������Ҫ����*/
		bool			hasAnyOnTimerCommand();											/*����ʱ��鵱ǰ�Ƿ��ж�ʱ������*/

		//���ͱ��ı���
		void			reportCmdExecResult( PMC_Feature_ptr p,hUChar result);														/*�������״̬���ظ�������	*/

		//���ձ���
		int				handleData( hUChar *buff, hUInt16 nLen );									/*���ձ���						*/
		void			putData( hUChar *buff, hUInt16 leng );										/*�����ݿ������������ݻ�����	*/
		
		//��������
		int				paraDevProtocol( );															/*����Э�鱨��					*/
		bool			findHeadAndAdjustMsg();														/*Ѱ�ұ���ͷ���������ģ�ɾ�����ñ���*/
		void			resolve_analog(PMC_Feature_ptr p);
		void			resolve_discrete(PMC_Feature_ptr p);
		void			resolve_control(PMC_Feature_ptr p);
		void			resolve_setvalue(PMC_Feature_ptr p);
		bool			resolve_SdevFqcy(PMC_Feature_ptr p);


		//��������
		void			clearRecvMsg(){ memset( &m_recv_msg, 0, sizeof( PMC_Recv_Msg ) );}
		PMC_Feature_ptr			findFeatureByCmdno(hInt32	cmdNo);
		PMC_Feature_ptr			findFeatureSetValue();
		PMC_Feature_ptr			findFeatureControl();
		PMC_Feature_ptr			findFeatureSetValue(int no,hUChar actionType);
		PMC_Feature_ptr			findFeatureControl(int no,hUChar actionType);
		PMC_Feature_ptr			findFeatureSetValue(int no);
		PMC_Feature_ptr			findFeatureControl(int no);
		PMC_Feature_ptr			findFeatureSyncTime();
		PMC_Feature_ptr			findFeatureConstantValue(hUChar	fc);

		int						resolve_constvalue(PMC_Feature_ptr p);
		void					resolve_SOEData(PMC_Feature_ptr p);
		void					resolve_RecordsData(PMC_Feature_ptr p);

		bool					getSOEData();
		bool					getRecordsData();
		void					resolve_analogTM(PMC_Feature_ptr p);		//����������ѹ��������
		void					resolve_paradata(PMC_Feature_ptr p);
		void					resolve_elecdata(PMC_Feature_ptr p);		//������������
		void					resolve_harmdata(PMC_Feature_ptr p);
		void					updateErrRate();
		bool					sendYcCommand(void* p);
		//bool					sendElecCommand(void* pFeature);
		bool					sendUdp();
		void					resolve_callYcCommand(PMC_Feature_ptr p);
		bool					editpara(ctrl_pro_constantvalue* pCom);
		bool					editSdevFqcy();
		bool					editRdevFqcy(int frequency);
		bool					readfile_senddev();

		hBool							m_open;
		hBool							m_islisting;												/*�ù�Լ�Ƿ����ģʽ			*/
		hBool							m_firstSend;												/*�Ƿ��һ�η���				*/
		PDT::CIntervalTimerSet			m_timeSet;													/*ʱ�䶨ʱ��					*/
		hInt32							m_rtimeout_id;												/*���ճ�ʱʱ��id				*/
		hInt32							m_scaninter_id;												/*ͨ��ɨ����					*/
		hUInt32							m_retryCount;												/*��Լ�ط�����					*/
		hUInt32							m_currPollCmdno;											/*��ǰ��ѭ�������				*/
		hUInt32							m_remoteAddr;												/*վַ							*/

		PMC_Feature						m_polling_feature[PMC_FEATURE_RECORD_MAXNUM];
		PMC_Current_Send_Command		m_current_send_cmd;
		PMC_Recv_Msg					m_recv_msg;

		ProtocolEvent					m_protocolEvent;
		hInt32							m_grpno;
		QVector<hInt32>					m_routeVector;

		unsigned int					m_ykReadPos; //

		QMap<hInt32, hInt32>			m_mapTimeout_id;

		hBool							m_bReceive;


		//SOE
		short							m_nSOENum;	//SOE���������ж�flag
		short							m_nSumSOE;	//SOE������
		QVector<ctrl_pro_data_soe>		m_nVector;

		//¼��
		int								m_nRecordsNum;
		QVector<PMC_Feature_ptr>		m_record_feature;	//¼������
		char							m_filePathName[256];

		QVector<sendpara>				m_vecPara;		//�㲥�޸���λ��Ƶ�ʺ͵�ַ

		//г��
		float							harm[6];

		hBool							m_bOk;
		PMC_Feature_ptr					m_pcmd;	

		int								m_nflag;
		int								m_nFqcy;

		//CRdbOp*							m_pRdbOp;
		DAC_SENDDEV*					m_pSdev;
		DAC_RECVDEV*					m_pRdev;
	};
}
#endif	//_PDT_DAC_MODBUS_101_H_
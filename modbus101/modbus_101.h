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
		Analog = 0 ,		//0遥调
		Discrete,			//1遥信
		Control,			//2遥控
		SetValue,			//3遥调
		SyncTime,			//4对时
		YcCommand,			//5
		ElecCommand,		//6电能数据
		HarmonicCmd,		//7谐波
		ParameterCmd,		//8参数
		ConstantValue,
		SOEData,
		RecordsData,
		RecordsData_SOE,
		RecordsData_YX,
		RecordsData_Para,
		RecordsData_Record,
		EditDFqcy,			//修改下位机频率地址
	};

	enum ManualCommandType
	{
		CTRL_PRO_CONSTANTVALUE			=	214,		//设置定值
		CTRL_PRO_YCCOMMAND				/*=	215*/,		//插入遥测命令
		CTRL_PRO_CALLSOEDATA			/*=	216*/,		//召唤SOE数据
		CTRL_PRO_CALLRECORDSDATA		/*= 217*/,		//召唤录波数据
		CTRL_PRO_CALLELEC				/*= 218*/,		//召唤电能
		CTRL_PRO_CALLHMC				/*=	219*/,		//召唤谐波
		CTRL_PRO_UDP					/*= 220*/,		//发送广播
		CTRL_PRO_EDITPRAR				/*= 221*/,		//修改上位机频段地址
		CTRL_PRO_EDITUDP				/*= 222*/,		//修改下位机频段地址
		CTRL_PRO_EDITPRAR1				/*= 223*/		//根据一键生成文件修改上位机频段地址
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
		char	groupCode[DAC_CODE_LEN];	//数据组编码
		nUInt8	ctrlType;					//控制类型，如遥控预置、执行、撤销
		nInt32	ctrlNo;						//控制号
		nInt32	ctrlOnNo;					//控合号
		nInt32	ctrlOffNo;					//控分号
		nUInt32	ctrlDuration;				//持续时间
		nUInt8	ctrlState;					//希望控制的状态
		nUInt8	ackState;					//返回状态
		char	ackReciever;				//返回信息接收者(如：SCADA_SERVER,FDC_SERVER)
		nUInt8	elapseTime;					//命令未被执行而流逝的时间
	}ctrl_pro_yk_ex;


	////定值处理
	//typedef struct _ctrl_pro_constantvalue_
	//{
	//	nInt32	groupNo;					//数据组号
	//	nUInt8	ctrlType;
	//	nUInt32	length;						//外部自定义命令缓冲长度
	//	char	cmdBuf[CTRL_COMMON_LEN];	//外部自定义命令缓冲
	//}ctrl_pro_constantvalue;

	//typedef struct _ctrl_pro_constantvalue_ack_
	//{
	//	nInt32	groupNo;					//数据组号
	//	nUInt8	ctrlType;
	//	nUInt32	length;						//外部自定义命令缓冲长度
	//	char	ackBuf[CTRL_COMMON_LEN];	//外部自定义命令缓冲
	//	nUInt8	ackState;					//返回状态
	//}ctrl_pro_constantvalue_ack;

	/*-
	 *--	问答规约特征结构	
	 -*/	
	typedef struct _modbus_101_feature	
	{
		char		cmdDesc[PMC_DESCRIBE_STRING_LENGTH];		/*命令描述							*/
		hInt32		cmdNo;										/*命令号，命令特征码				*/
		hBool		isUsed;										/*命令是否使用						*/
		hBool		isAck;										/*命令是否需要回复					*/
		hUChar		fc;											/*功能码							*/
		hUChar		fc2;										/*功能码2							*/
		hUChar		fcLength;									/*功能码位数（2-两位，4-四位）		*/
		hUChar		frmType;									/*帧类型（0-yC，1-yX，2-yK，3-yT）	*/
		hUChar		addrH;										/*寄存器地址高位					*/
		hUChar		addrL;										/*寄存器地址低位					*/
		hUChar		lengthH;									/*数据长度高位						*/
		hUChar		lengthL;									/*数据长度低位						*/
		hInt32		routeNo;									/*该数据所在路径号					*/
		hInt32		startNo;									/*该帧遥测或遥信的起始地址			*/
		hInt32		nextCmdNo;									/*下一个询问指令，如果填写-1表示不判断，按照前一个命令寻找（在插入遥控的时候有用）*/
		hUChar		checkFunc;									/*命令校验算法						*/
		ctrl_pro_yk		ykptr;
	}PMC_Feature, *PMC_Feature_ptr;

	typedef struct _modbus_101_current_send_command_
	{
		hUChar		cmdFunc;									/*命令特征码1						*/
		hUChar		cmdFunc2;									/*命令特征码2						*/
		hUChar		cmd[PMC_SEND_BUFFER_SIZE];					/*命令								*/                                            
		hUInt16		length;										/*命令长度							*/
		char		cmdStr[PMC_DESCRIBE_STRING_LENGTH];			/*命令描述							*/
		PMC_Feature_ptr	currFeatureptr;							/*当前发送的命令指针				*/
		hUInt32		retryCount;									/*重发次数							*/
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

		//是否打开
		virtual	bool	isOpen() const;
		//打开规约
		virtual bool	open();
		//关闭规约
		virtual void	close();
		//接收处理入口
		virtual void	mainRx();

		//发送处理入口
		virtual void	mainTx();
	protected:
	private:
		//初始化读取数据库
		bool			readFeature( DAC_ROUTE	*m_proute );										/*读取规约特征表				*/
		void			addFeatureYKYT();															/*在规约特征表中增加两条信息，为遥控和遥调所用	*/
		void			createSTMatrix();															/*根据规约特征表创建状态转移矩阵*/

		//状态转移矩阵操作，根据当前命令功能码和规约事项寻找下一帧应发报文
		void			findNextCommand();

		//生成报文
		void			makeCommand( PMC_Feature_ptr p );									/*根据特征表配置生成下发报文	*/
		bool			make_yk_select(void* p);
		bool			make_yk_execute(void* p);
		bool			make_yk_cancel(void* p);
		bool			make_select_parameter(void* p);
		bool			make_set_parameter(void* p);
		bool			sendTime();

		bool			make_constantValue_read(ctrl_pro_constantvalue* p);
		bool			make_constantValue_write(ctrl_pro_constantvalue* p);
		bool			SetCVCmdAck(const ctrl_pro_constantvalue_ack* pAck);


		//发送报文
		void			setCmdinfoToBuf( );															/*把发送报文送入到发送通道发送	*/
		//发送控制的时候辅助函数
		bool			checkCurrentCommand();											/*检查当前应发送的命令，当返回为真是允许下发命令*/
		bool			hasAnyManualCommand();											/*发送时检查当前施法有人工控制命令需要发送*/
		bool			hasAnyOnTimerCommand();											/*发送时检查当前是否有定时任务发送*/

		//发送报文报告
		void			reportCmdExecResult( PMC_Feature_ptr p,hUChar result);														/*将命令发送状态返回给控制者	*/

		//接收报文
		int				handleData( hUChar *buff, hUInt16 nLen );									/*接收报文						*/
		void			putData( hUChar *buff, hUInt16 leng );										/*把数据拷贝到接收数据缓冲区	*/
		
		//解析报文
		int				paraDevProtocol( );															/*解析协议报文					*/
		bool			findHeadAndAdjustMsg();														/*寻找报文头并调整报文，删除无用报文*/
		void			resolve_analog(PMC_Feature_ptr p);
		void			resolve_discrete(PMC_Feature_ptr p);
		void			resolve_control(PMC_Feature_ptr p);
		void			resolve_setvalue(PMC_Feature_ptr p);
		bool			resolve_SdevFqcy(PMC_Feature_ptr p);


		//其他操作
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
		void					resolve_analogTM(PMC_Feature_ptr p);		//解析基本电压电流数据
		void					resolve_paradata(PMC_Feature_ptr p);
		void					resolve_elecdata(PMC_Feature_ptr p);		//解析参数数据
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
		hBool							m_islisting;												/*该规约是否监听模式			*/
		hBool							m_firstSend;												/*是否第一次发送				*/
		PDT::CIntervalTimerSet			m_timeSet;													/*时间定时器					*/
		hInt32							m_rtimeout_id;												/*接收超时时间id				*/
		hInt32							m_scaninter_id;												/*通道扫描间隔					*/
		hUInt32							m_retryCount;												/*规约重发次数					*/
		hUInt32							m_currPollCmdno;											/*当前的循环命令号				*/
		hUInt32							m_remoteAddr;												/*站址							*/

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
		short							m_nSOENum;	//SOE数据请求判断flag
		short							m_nSumSOE;	//SOE总条数
		QVector<ctrl_pro_data_soe>		m_nVector;

		//录波
		int								m_nRecordsNum;
		QVector<PMC_Feature_ptr>		m_record_feature;	//录波命令
		char							m_filePathName[256];

		QVector<sendpara>				m_vecPara;		//广播修改下位机频率和地址

		//谐波
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
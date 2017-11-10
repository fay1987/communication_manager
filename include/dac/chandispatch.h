/*==============================================================================
* 文件名称 : chandispatch.h
* 模块 : 前置通道数据分流接口
* 创建时间 : 2009-10-16 15:55:23
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_BFS_DAC_CHANDISP_H_
#define	_BFS_DAC_CHANDISP_H_

#include <ace/config-all.h>
//=============================================================================
#if defined (_DAC_CHANDISP_)
#  define	CHANDISP_EXPORT	ACE_Proper_Export_Flag
#else
#  define	CHANDISP_EXPORT	ACE_Proper_Import_Flag
#endif	// _DAC_CHANDISP_
//=============================================================================
#include "utl/datatype.h"
#include "dac/dacdef.h"
#include "dacmacro.h"
#include "dac/comminf.h"
#include <ace/OS.h>
#include <ace/Task.h>
#include "dac/cyclebuffer.h"
#include <utl/intervaltimerset.h>
#include "dac/buffer.h"

namespace BFS
{
	enum DISP_LOG_TYPE
	{
		LOG_DISP_BASE			=	3000,		//base日志ID
		LOG_DISP_DATA_PUT		=	3001,		//数据传入+解析
		LOG_DISP_DATA_GET		=	3002,		//数据获取
		LOG_DISP_DATA_ERR		=	3003,		//报文错误日志
		LOG_DISP_DATA_SHOW		=	3004,		//解析过程中的报文显示
		LOG_DISP_DATA_RX_SHOW	=	3005,		//网络层RX报文显示
		LOG_DISP_DATA_TX_SHOW	=	3006		//网络层TX报文显示
		
	};

	#define	DISP_MAX_BUFFER_LEN	10240		//10k 

	//帧定义
	#define FRAM_START			0x68		//帧起始码
	#define FRAM_END			0x16		//帧结束码
	//控制码
	#define FRAM_CTRL_REG		0x21		//登录码
	#define FRAM_CTRL_REG_ACK	0xA1		//登录回校

	#define FRAM_CTRL_UNREG		0x22		//登录退出码
	#define FRAM_CTRL_UNREG_ACK	0xA2		//登录退出回校

	#define FRAM_CTRL_HEART		0x24		//心跳码
	#define FRAM_CTRL_HEART_ACK	0xA4		//心跳回校

	class CHANDISP_EXPORT CChanDispTask : public ACE_Task<ACE_NULL_SYNCH>
	{
	public:
		CChanDispTask();
		virtual ~CChanDispTask();
	public:
		//本端站址+密码
		bool	init(int localaddr,const char* pwd);
		int		start();
		void	stop();
	protected:
		//主处理过程
		int		svc();
		void	mainRx();
		bool	checkSUMC(const hUChar *buf,int len);
		nUInt8	makeSUMC(const nUInt8 *buf,int len);
		int		checkSerialNo(const hUChar* buf,int len);
		bool	putToChannel(int channel,const hUChar* buf,int len);
		bool	isLocalCtrlCode(hUInt32 ctrlCode);
		bool	checkLocalSerialNo(const hUChar* buf,int len);
		bool	procLocalHeartAck();
		bool	procLocalRegisterAck();
		//通道路径等操作+信息更新
		void	startChannel(int channel);
		void	stopChannel(int channel);
		void	stopAllChannel();
		void	txBytesChannel(int channel,int num);
		bool	addRecvOkFrmNum(int channel,int frmNum=1);
		//wfp temp add
		void	setRemoteRegisterAck(const hUChar* buf);
	public:
		int		putData(const hUChar* data,int length);
		int		getData(hUChar* data,int size);
		int		getLocalHeartData(hUChar* data,int size);
		int		getLocalRegisterData(hUChar* data,int size);
		int		getLocalUnRegisterData(hUChar* data,int size);
		bool	checkLocalHeart();
		bool	isLocalRegistered();
		bool	goClose();
		void	displayData(int logId,const char* descr,const hUChar* buf,int len);
		//wfp temp add
		int		getRemoteRegisterData(hUChar* data,int size);
	private:
		//说明： local指网关客户端与网关服务之间
		CCommInf			m_commInf;
		CCycleBuffer		m_buffer;
		CBuffer				m_rxBuffer[DAC_CHANNEL_NUM];			//通道接收缓冲区
		CBuffer				m_txBuffer[DAC_CHANNEL_NUM];			//通道发送缓冲区
		bool				m_isLocalRegisterd;						//网关客户端是否向网关服务登录成功
		int					m_localHeartAck;						//心跳回校计数
		int					m_curChannel;							//当前发送通道的序号

		CIntervalTimerSet	m_timer;
		int					m_timeId;

		int					m_localAddr;		//本端站址
		char				m_pwd[DAC_PWD_LEN];	//网关服务登录密码

		//wfp temp add at 20100105
		nUInt8				m_regAckBuffer[32];//临时增加的电压表登录反校
		bool				m_isNeedSendRegAck;//临时增加的电压表登录反校

	};

	typedef	ACE_Singleton<CChanDispTask,ACE_SYNCH_NULL_MUTEX>	CHANDISPTASK;

}

#endif	//_BFS_DAC_CHANDISP_H_

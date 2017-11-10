/*==============================================================================
* 文件名称 : protocol.h
* 模块 : 规约基类
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_PROTOCOL_H_
#define	_PDT_DAC_PROTOCOL_H_

#include "dac/comminf.h"
#include "dac/datainf.h"
#include "dac/buffer.h"
#include "dac/message.h"
#include "dac/ctrlinf.h"
#include "dac/dacctrldef.h"

#define	PROTO_ONESEC_TIMES	(1000000/DAC_PROTO_RUN_DELAY)		//规约每秒的调度次数

namespace PDT
{
		//通道数据类型，对应channel表f_datatype字段(wfp add at 20100409)
		enum PRO_CHAN_DATATYPE
		{
			DATATYPE_NORMAL		=	0,			//普通
			DATATYPE_EII		=	1,			//EII
			DATATYPE_HONGDIAN	=	2			//宏电
		};

		class DAC_EXPORT CProtocol
		{
		public:
			CProtocol();
			virtual ~CProtocol();
			//是否打开
			virtual	bool	isOpen() const = 0;
			//打开规约
			virtual bool	open() = 0;
			//关闭规约
			virtual void	close() = 0;
			//接收处理入口
			virtual void	mainRx() = 0;
			//发送处理入口
			virtual void	mainTx() = 0;

			hBool			setTimeOut();
			void			setTimeOut(hBool bTimeOut){m_bTimeOut = bTimeOut;}

			hBool			isExclusive();


		public:
			//初始化时调用
			bool			init(hInt32 channel,hInt32 route);
		protected:
			virtual	bool	readFeature();

			//one channel -- multi routes 
			/**
			* @function			若规约需要收到回应后才能下行，则使用该函数过滤下行
			* @param isSend		true:有命令正在发送；false:无
			* @simple example	mainTx()函数起始位置,if( !isSending() ) return;
			*/
			hBool			isSending() const;
			/**
			* @function			mainTx()中报告是否发送了下行命令
			* @param isSend		true:有下行命令发送；false:无下行
			*/
			void			reportCmdSend(hBool isSend = true);	
			/**
			* @function			mainRx()中报告收到下行命令的回校数据
			* @param isOk		true:数据正确；false:数据错误
			*/
			void			repotrAckRecv(hBool isOk = true);

			//接收缓冲区
			bool			get(hUChar& val);
			hUInt32			get(hUChar *buf,hUInt32 len);
			hUInt32			length()	const;		//可用字节数
			bool			back(hUInt32 num);		//向后移动当前读指针

			//发送缓冲区
			bool			put(hUChar val);
			bool			put(const hUChar *buf,hUInt32 len);
			hUInt32			remain();				//剩余空间长度

			//组合帧内容
			bool			msgCopy(const hUChar* buf,hUInt32 len);
			//从后往前删除帧内容
			bool			msgRemove(hUInt32 len);
			//清除帧内容
			void			msgClean();
			//显示组合的帧内容
			bool			msgDisplay(hUInt8 type,const char* desc); 
			/**
			* @function			解析后报文显示
			* @param type		帧类型，具体值参考FRAME_TYPE
			* @param desc		帧描述，长度限制64字节
			* @param data		帧内容，长度限制1024字节
			* @param len		帧长度
			* @retval			true:成功；false:失败
			*/
			bool			msgDisplay(hUInt8 type,const char* desc,const hUChar* data,hUInt32 len);

			//修改路径信息帧计数
			/**
			* @function			增加路径的发送帧计数
			* @param frmNum		需要增加的帧数目
			* @retval			true:成功；false:失败
			*/
			bool			addSendFrmNum(int frmNum=1);
			/**
			* @function			增加路径的接收正确帧计数
			* @param frmNum		需要增加的正确帧数目
			* @retval			true:成功；false:失败
			*/
			bool			addRecvOkFrmNum(int frmNum=1);
			/**
			* @function			增加路径的接收错误帧计数
			* @param frmNum		需要增加的错误帧数目
			* @retval			true:成功；false:失败
			*/
			bool			addRecvErrFrmNum(int frmNum=1);

		protected:	//控制接口
			/**
			* @function			获取当前路径的规约控制命令
			* @param cmdBuf		in&out缓冲区指针
			* @param len		in缓冲区长度
			* @retval			true:成功；false:失败
			*/
			bool			getCtrlCmd(char* cmdBuf,int len);
			bool			getCtrlCmdByRoute(hInt32 routeno,char* cmdBuf,int len);
			/**
			* @function			生成当前路径的规约控制反校命令
			* @param ackBuf		in缓冲区指针
			* @param len		in缓冲区内容长度
			* @retval			true:成功；false:失败
			*/
			bool			setCtrlCmdAck(const char* ackBuf,int len);
			bool			setCtrlCmdAckEx(const char* ackBuf,int len);


		private:
			//同CDacCtrl.commonCmdAck()函数
			bool			commonCmdAck(const ctrl_pro_common_ack* pCommonAck);
		protected:
			CCommInf		m_commInf;
			CDataInf		m_dataInf;
			CCtrlInf		m_ctrlInf;
			hInt32			m_route;		//路径号
			hInt32			m_channel;		//通道号

			hBool			m_bTimeOut;
			hBool			m_isExclusive;		//是否需要独占该通道，处理多条命令连续下发。

		private:
			hUChar			m_msg[DAC_MSG_DATA_LEN];
			hUInt32			m_msgLen;
			CBuffer			m_rxBuffer;
			CBuffer			m_txBuffer;
			CMessage		m_message;
			hBool			m_isSending;		//命令正在下行，等待超时
		};
}
#endif //_PDT_DAC_PROTOCOL_H_

/*==============================================================================
* 文件名称 : middlelayer.h
* 模块 : 中间层规约基类
* 创建时间 : 2010-11-09 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_BFS_DAC_MIDDLELAYER_H_
#define	_BFS_DAC_MIDDLELAYER_H_

#include "dac/bufferext.h"
#include "dac/protocol.h"
#include "dac/toolbase.h"
#include "utl/intervaltimerset.h"
#include "sys/blog.h"
#include <QtCore/QtCore>

//公共宏定义--------------begin-------
#define LOG_PRO_BASE		20000		//日志基础序号
//公共宏定义--------------end---------

//ELB协议相关宏定义-------begin-------
//标准同步头字节
#define SYNC_HEAD_BYTE1		0xeb
#define SYNC_HEAD_BYTE2		0x90

//标准同步尾字节
#define SYNC_TAIL_BYTE		0x7e

//关注的ELB命令类型
#define ELB_CMD_REG			0xe6		//注册
#define ELB_CMD_DAT			0xe3		//数据

//向ELB发送注册命令的间隔（秒）
#define ELB_REG_INTERVAL	10*60
//ELB最小帧长
#define ELB_MIN_FRM_LEN		15

//ELB协议相关宏定义-------end---------


namespace BFS
{
	class CMiddleLayer : public CProtocol
	{
	public:
		CMiddleLayer();
		virtual ~CMiddleLayer();

		//是否打开（注：派生类实现本函数的代码前先调本基类的isOpen() ）
		virtual	bool	isOpen() const;
		//打开规约（注：派生类实现本函数的代码前先调本基类的open() ）
		virtual bool	open();
		//关闭规约（注：派生类实现本函数的代码后再调本基类的close() ）
		virtual void	close();
		//接收处理入口（注：派生类实现本函数的代码前先调本基类的mainRx() ）
		virtual void	mainRx();
		//发送处理入口（注：派生类实现本函数的代码前先调本基类的mainTx() ）
		virtual void	mainTx();

	protected://中间协议公共变量与函数
		bool				m_isOpen;			//是否打开标志
		CBufferExt			m_rxDataBuf;		//实际协议数据缓冲区
		DAC_ROUTE*			m_pRoute;			//route指针
		DAC_ROUTE_INFO*		m_pRouteInfo;		//routeInfo指针 
		DAC_CHANNEL*		m_pChannel;			//channel指针
		DAC_CHANNEL_INFO*	m_pChannelInfo;		//channelinfo指针
		CIntervalTimerSet	m_timeSet;			//时间定时器集

		//是否允许发送数据
		bool	canSend() const;
		//发送数据
		bool	sendData(const hUChar *buf,hUInt32 len);
		///显示报文--支持显示超长度的报文
		void	showMsg(hUInt8 type,const char* desc,const hUChar* data,hUInt32 len);

	private://中间协议公共变量与函数
		bool				m_canSend;			//是否可以发送数据
		CBufferExt			m_rxMiddleBuf;		//中间协议数据缓冲区
			
		//查找同步头
		int		findSyncHeader(hUChar* buf,int len,int startPos,int& foundBytes);
		//大字节序转小字节序int
		void	bigEndToLittleEndInt(hUChar* buf,int len,int& retVal);
		//大字节序转小字节序float
		void	bigEndToLittleEndFloat(hUChar* buf,float& retVal);

	private://ELB协议变量与函数	
		int				m_elbTimeIDReg;			//elb注册帧定时器标识
		QString			m_elbPara;				//elb参数
		int				m_elbCom;				//elb串口号

		//解析elb参数
		bool	elbParsePara(const char* para);
		//发送elb注册帧
		void	elbTxReg();
		//发送elb数据帧
		bool	elbTxData(const hUChar* buf,int len);
		//解析elb数据
		int		elbParseData();

	private://艾萨协议变量与函数
		
	private://宏电协议变量与函数
		
	};
}

#include "middlelayer.inl"

#endif //_BFS_DAC_MIDDLELAYER_H_

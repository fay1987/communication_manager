/*==============================================================================
* 文件名称 : channeltcpcli.h
* 模块 : 电力数据采集与转发平台--平台进程(pdat.exe)--TCP客户端通道类
* 创建时间 : 2008-12-01 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef __PDAT_CHANNELTCPCLI_H__
#define __PDAT_CHANNELTCPCLI_H__

#include "channel.h"
#include <QtNetwork>
#include "dac/comminf.h"
#include "utl/intervaltimerset.h"
#include "dac/buffer.h"

//重连延时的累加间隔
#define INTV_RECONNECT_DEALY	3
#define MAX_RECONNECT_DELAY		60

class CChannelTcpCli : public CChannel
{
	Q_OBJECT
public:
	CChannelTcpCli(short channelId );
	virtual ~CChannelTcpCli(void);
	
	void	open();
	bool	isOpen();
	void	close();
	int		recv();
	int		send();
	void	remove();

	protected Q_SLOTS:
	void onConnected ();
	void onDisconnected();
	void onError( QAbstractSocket::SocketError socketError );
	void onReadyRead ();
	void onSendData();
	void onStateChanged ( QAbstractSocket::SocketState socketState );
protected:
	QTcpSocket*	m_pSocketDev;
	
private:
	bool	isAddrValid(const char* host,unsigned short port );
	void	connectError();
private:
	PDT::CCommInf			m_commInf;
	PDT::CBuffer			m_rxBuffer;
	PDT::CBuffer			m_txBuffer;

	hUInt32					m_curConnect;		//当前连接序号，用于切换连接的远端地址

	//延时连接
	hInt32					m_reConnectDelay;
	PDT::CIntervalTimerSet	m_timer;
	hInt32					m_timeId;
	bool					m_isRemoved;		//对象是否已被延时删除，防止之前的信号被调度
};


#endif //__PDAT_CHANNELTCPCLI_H__












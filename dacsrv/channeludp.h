/*==============================================================================
* 文件名称 : channeludp.h
* 模块 : 电力数据采集与转发平台--平台进程(dacsrv.exe)--UDP通道类
* 创建时间 : 2008-12-01 15:55:23
* 作者 : AAA
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef __PDAT_CHANNELUDP_H__
#define __PDAT_CHANNELUDP_H__

#include "channel.h"
#include <QtNetwork>
#include "dac/comminf.h"
#include "utl/intervaltimerset.h"
#include "dac/buffer.h"

class CChannelUdp : public CChannel
{
	Q_OBJECT
public:
	CChannelUdp(short channelId );
	virtual ~CChannelUdp(void);

	void	open();
	void	multiopen();
	bool	isOpen();
	void	close();
	int		recv();
	int		send();
	void	remove();

	protected Q_SLOTS:
		void onError( QAbstractSocket::SocketError socketError );
		void onReadyRead ();
		void onSendData();
		void onStateChanged ( QAbstractSocket::SocketState socketState );
protected:
	QUdpSocket*	m_pSocketDevRecv;
	QUdpSocket* m_pSocketDevSend;

private:
	bool	isAddrValid(const char* host,unsigned short port );
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
#ifdef	WIN32
	WSADATA wsd;
#endif
};


#endif //__PDAT_CHANNELUDP_H__












/*==============================================================================
* 文件名称 : channelstream.h
* 模块 : 电力数据采集与转发平台--平台进程(pdat.exe)--动态接受的TCP通道类
* 创建时间 : 2008-12-01 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef __DAC_CHANNELSTREAM_H__
#define __DAC_CHANNELSTREAM_H__

#include "channel.h"
#include <QtNetwork>
#include "dac/comminf.h"
#include "dac/buffer.h"

class CChannelStream : public CChannel
{
	Q_OBJECT
public:
	CChannelStream(int channelIdRef,QTcpSocket* socket);
	CChannelStream(int channelId,int channelIdRef,QTcpSocket* socket);
	virtual ~CChannelStream(void);

protected:
	bool	init();
	void	close();
	int		recv();
	int		send();
	void	remove();

	protected Q_SLOTS:
		void onDisconnected ();
		void onError ( QAbstractSocket::SocketError socketError );
		void onReadyRead ();
		void onSendData();
private:
	short			m_channelIdRef;
	QTcpSocket*		m_pSocketDev;
	PDT::CCommInf	m_commInf;
	PDT::CBuffer	m_rxBuffer;
	PDT::CBuffer	m_txBuffer;
	bool			m_isRemoved;		//对象是否已被延时删除，防止之前的信号被调度
};



#endif //__PDAT_CHANNELSTREAM_H__



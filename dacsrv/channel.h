/*==============================================================================
* 文件名称 : channel.h
* 模块 : 电力数据采集与转发平台--平台进程(pdat.exe)--通道基类
* 创建时间 : 2008-12-01 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef __PDAT_CHANNEL_H__
#define __PDAT_CHANNEL_H__

#include <QtCore>

class CChannel  : public QObject
{
	Q_OBJECT
public:
	CChannel(short channelId);
	virtual ~CChannel(void);
	enum ChannelType { Serial,TcpClient,TcpServer,TcpStream,Dynamic,Udp,Unknown};
	enum ChannelState{ Closed,Connecting,Listening,Connected,Disable};

	virtual bool	init();
	virtual void	open();
	virtual void	close();
	virtual void	remove();
	virtual int		recv();
	virtual int		send();
	ChannelState	state();
	ChannelType		type() const;
	void			setType(ChannelType type);
	int				channelId() const;

public Q_SLOTS:
	void			onSendData();
protected:
	ChannelState	m_eChannelState;
	ChannelType		m_eChannelType;
   
	short			m_channelId;
private:
};


#endif	//__PDAT_CHANNEL_H__







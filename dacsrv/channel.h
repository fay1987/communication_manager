/*==============================================================================
* �ļ����� : channel.h
* ģ�� : �������ݲɼ���ת��ƽ̨--ƽ̨����(pdat.exe)--ͨ������
* ����ʱ�� : 2008-12-01 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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







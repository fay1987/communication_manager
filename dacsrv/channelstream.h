/*==============================================================================
* �ļ����� : channelstream.h
* ģ�� : �������ݲɼ���ת��ƽ̨--ƽ̨����(pdat.exe)--��̬���ܵ�TCPͨ����
* ����ʱ�� : 2008-12-01 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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
	bool			m_isRemoved;		//�����Ƿ��ѱ���ʱɾ������ֹ֮ǰ���źű�����
};



#endif //__PDAT_CHANNELSTREAM_H__



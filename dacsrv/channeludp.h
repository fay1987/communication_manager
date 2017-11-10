/*==============================================================================
* �ļ����� : channeludp.h
* ģ�� : �������ݲɼ���ת��ƽ̨--ƽ̨����(dacsrv.exe)--UDPͨ����
* ����ʱ�� : 2008-12-01 15:55:23
* ���� : AAA
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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

	hUInt32					m_curConnect;		//��ǰ������ţ������л����ӵ�Զ�˵�ַ

	//��ʱ����
	hInt32					m_reConnectDelay;
	PDT::CIntervalTimerSet	m_timer;
	hInt32					m_timeId;
	bool					m_isRemoved;		//�����Ƿ��ѱ���ʱɾ������ֹ֮ǰ���źű�����
#ifdef	WIN32
	WSADATA wsd;
#endif
};


#endif //__PDAT_CHANNELUDP_H__












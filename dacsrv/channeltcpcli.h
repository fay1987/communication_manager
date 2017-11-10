/*==============================================================================
* �ļ����� : channeltcpcli.h
* ģ�� : �������ݲɼ���ת��ƽ̨--ƽ̨����(pdat.exe)--TCP�ͻ���ͨ����
* ����ʱ�� : 2008-12-01 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef __PDAT_CHANNELTCPCLI_H__
#define __PDAT_CHANNELTCPCLI_H__

#include "channel.h"
#include <QtNetwork>
#include "dac/comminf.h"
#include "utl/intervaltimerset.h"
#include "dac/buffer.h"

//������ʱ���ۼӼ��
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

	hUInt32					m_curConnect;		//��ǰ������ţ������л����ӵ�Զ�˵�ַ

	//��ʱ����
	hInt32					m_reConnectDelay;
	PDT::CIntervalTimerSet	m_timer;
	hInt32					m_timeId;
	bool					m_isRemoved;		//�����Ƿ��ѱ���ʱɾ������ֹ֮ǰ���źű�����
};


#endif //__PDAT_CHANNELTCPCLI_H__












/*==============================================================================
* �ļ����� : channeltcpsrv.h
* ģ�� : �������ݲɼ���ת��ƽ̨--ƽ̨����(pdat.exe)--TCP�����ͨ����
* ����ʱ�� : 2008-12-01 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef __DAC_CHANNELTCPSRV_H__
#define __DAC_CHANNELTCPSRV_H__

#include "channel.h"
#include <QtNetwork>
#include "dac/comminf.h"

class CChannelTcpSrv : public CChannel
{
	Q_OBJECT
public:
	CChannelTcpSrv(int channelId);
	virtual ~CChannelTcpSrv(void);

	void	open();
	void	close();
	void	remove();
	void	removeStream();

	protected Q_SLOTS:
		void	onNewConnection();
private:
	QTcpServer		*m_pSocketDev;
	CChannel		*m_pChannel;		//���ڱ��浱ǰ����
	PDT::CCommInf	m_commInf;
};



#endif //__DAC_CHANNELTCPSRV_H__

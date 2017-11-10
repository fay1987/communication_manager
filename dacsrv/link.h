/*==============================================================================
* �ļ����� : link.h
* ģ�� : ��·��
* ����ʱ�� : 2009-08-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_PDT_DAC_LINK_H_
#define	_PDT_DAC_LINK_H_

#include "ace/OS.h"
#include "utl/string.h"
#include "dac/dacdef.h"
#include "dac/comminf.h"
#include "dac/protocol.h"

using namespace PDT;

#define ROUTE_TIME_OUT		3		//·����ʱʱ��(����one channel - multi routes)

class CLink
{
public:
	CLink(hInt32 no);
	virtual ~CLink();

	bool		init(hInt32 no);
public:
	
	bool		isValid()	const;
	//�Ƿ��
	bool		isOpen()	const;
	//��
	bool		open();
	//�ر�
	void		close();
	//���������
	void		run();
	void	setTimeOutFlag(hBool bFlag){m_bTimeOut = bFlag;}

protected:
	bool		initProtocol(hInt32 routeOrder);
	void		destroyProtocol(hInt32 routeOrder);
	bool		check();
private:
	//������
	CCommInf		m_commInf;
	//������
	CProtocol		*m_pProtocol[DAC_ROUTE_IN_CHANNEL];	//��Լָ��
	//������
	hInt32			m_chanNo;							//ͨ����
	bool			m_isOpen;
	ACE_Time_Value  m_curRouteBegin;					//��ǰ����·������ʼʱ��
	hBool			m_bTimeOut;
};

#endif //_PDT_DAC_LINK_H_

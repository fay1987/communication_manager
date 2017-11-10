/*==============================================================================
* �ļ����� : processchecktask.h
* ģ�� : ��eisd�Ǽ�ע��
* ����ʱ�� : 2009-07-22 15:55:23
* ���� : wfp
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_PDT_DAC_CHECKTASK_H_
#define	_PDT_DAC_CHECKTASK_H_

#include <ace/Task.h>
#include <utl/intervaltimerset.h>
#include "utl/string.h"
#include "sys/procman.h"


#define PROCESS_CHECK_TIME		1		//���̼����1s

class CProcessCheckTask : public ACE_Task<ACE_NULL_SYNCH>
{
public:
	CProcessCheckTask();
	virtual ~CProcessCheckTask();
public:
	bool	init(const char* procName,const char* procDesc,int procHwd,int logId);
	int		start();
	void	stop();
	int		svc();
public:
	//���dacsrv����ʱ��ϳ�ʹ��
	void	beginInitSystem();
	void	endInitSystem();
private:
	PDT::CIntervalTimerSet	m_timer;
	int						m_timeId;
	PDT::CProcessManage		m_procMan;
	PDT::CString			m_procName;		//������
	PDT::CString			m_procDesc;		//��������
	int						m_procHwd;		//���̾��
	int						m_logId;		//��־��
	bool					m_isDebug;		//�Ƿ����ģʽ
};

#endif //_PDT_DAC_CHECKTASK_H_

#ifndef	_PDT_DAC_LINKTASKPOOL_H_
#define	_PDT_DAC_LINKTASKPOOL_H_

#include "ace/OS.h"
#include <ace/Task.h>
#include "link.h"
#include "dac/comminf.h"
#include <ace/Vector_T.h>
#include "deadheartset.h"	//wfp add at 20101129

using namespace PDT;

typedef ACE_Vector<int>		LINKNO_VECTOR;
class CLinkTaskPool : public ACE_Task<ACE_NULL_SYNCH>
{
public:
	CLinkTaskPool();
	virtual ~CLinkTaskPool();
	hInt32	start ();
	void	stop();
	hUInt32	channelNumber() const;	//��ȡ��Чͨ����Ŀ
	//���������
	hInt32	svc();
public:
	hInt32  lockMultiLinks(LINKNO_VECTOR& linknoVec,hInt32 num);
	void	unLockMultiLinks(const LINKNO_VECTOR& linknoVec);
	hInt32	lockLink();					//�õ�������·������
	void	unLockLink(hInt32 linkNo);	//������·

	void	setTimeOutFlag(hBool bFlag){m_bTimeOut = bFlag;}

private:
	CCommInf			m_commInf;
	hUInt32				m_oneTaskLinkNum;

	ACE_Thread_Mutex	m_mutex;
	hUInt32				m_curChanNo;
	struct	_link_
	{
		bool	lock;
		CLink*	pObj;
	}m_link[DAC_CHANNEL_NUM];

	//wfp add at 20101129
public:
	bool				checkDeadHeart();
private:
	int					createDeadHeart();
	void				addHeartLink(int heartid,LINKNO_VECTOR& linknoVec);
	CDeadHeartSet		m_deadHeartSet;
	hBool	m_bTimeOut;

};

#endif //_PDT_DAC_LINKTASKPOOL_H_

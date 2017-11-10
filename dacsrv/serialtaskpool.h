#ifndef	_PDT_DAC_SERIALTASK_H_
#define	_PDT_DAC_SERIALTASK_H_

#include "ace/OS.h"
#include <ace/Task.h>
#include "dac/comminf.h"
#include "serialchannel.h"

using namespace PDT;

class CSerialTaskPool : public ACE_Task<ACE_NULL_SYNCH>
{
public:
	CSerialTaskPool();
	virtual ~CSerialTaskPool();
	bool	init();
	hInt32	start();
	void	stop();
	hUInt32	serialChannelNumber() const;	//获取有效通道数目
	//主处理过程
	hInt32	svc();
public:
	hInt32	lockSerialChannel();					//得到空闲链路并锁定
	void	unLockSerialChannel(hInt32 channel);	//解锁链路
private:
	bool	isSerialChannel(hInt32 channel) const;
	void	checkObject();							//检测对象是否存在
private:
	CCommInf			m_commInf;
	ACE_Thread_Mutex	m_mutex;
	hUInt32				m_curChanNo;
	struct	_serial_channel_
	{
		bool			lock;
		CSerialChannel*	pObj;
	}m_serialChannel[DAC_CHANNEL_NUM];
};

#endif //_PDT_DAC_SERIALTASK_H_

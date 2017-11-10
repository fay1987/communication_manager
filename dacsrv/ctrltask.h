/*==============================================================================
* 文件名称 : ctrltask.h
* 模块 : 处理命令缓冲区中的命令
* 创建时间 : 2009-07-22 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_CTRLTASK_H_
#define	_PDT_DAC_CTRLTASK_H_

#include <ace/Task.h>
#include "dac/ctrlinf.h"
#include <utl/intervaltimerset.h>

using namespace PDT;

class CCtrlTask : public ACE_Task<ACE_NULL_SYNCH>
{
public:
	CCtrlTask();
	virtual ~CCtrlTask();
public:
	bool	init();
	int		start();
	void	stop();
	int		svc();
private:
	CCommInf			m_commInf;
	CCtrlInf			m_ctrlInf;
	CIntervalTimerSet	m_timer;
	int					m_timeId;


	void	procCtrlCmd();
	void	doLoadPara(hUChar* cmdBuf);
	void	doManOperate(hUChar* cmdBuf);
	void	doManStartChannel(const char* code);
	void	doManStopChannel(const char* code);
	void	doManEnableChannel(const char* code);
	void	doManDisableChannel(const char* code);
	void	doManStartRoute(const char* code);
	void	doManStopRoute(const char* code);
	void	doManEnableRoute(const char* code);
	void	doManDisableRoute(const char* code);
	void	doManResetProtocol(const char* code);
	void	doManStartGroup(const char* code);
	void	doManStopGroup(const char* code);
	void	doManEnableGroup(const char* code);
	void	doManDisableGroup(const char* code);
};

#endif //_PDT_DAC_CTRLTASK_H_

/*==============================================================================
* 文件名称 : processchecktask.h
* 模块 : 向eisd登记注册
* 创建时间 : 2009-07-22 15:55:23
* 作者 : wfp
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_CHECKTASK_H_
#define	_PDT_DAC_CHECKTASK_H_

#include <ace/Task.h>
#include <utl/intervaltimerset.h>
#include "utl/string.h"
#include "sys/procman.h"


#define PROCESS_CHECK_TIME		1		//进程检测间隔1s

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
	//针对dacsrv加载时间较长使用
	void	beginInitSystem();
	void	endInitSystem();
private:
	PDT::CIntervalTimerSet	m_timer;
	int						m_timeId;
	PDT::CProcessManage		m_procMan;
	PDT::CString			m_procName;		//进程名
	PDT::CString			m_procDesc;		//进程描述
	int						m_procHwd;		//进程句柄
	int						m_logId;		//日志号
	bool					m_isDebug;		//是否调试模式
};

#endif //_PDT_DAC_CHECKTASK_H_

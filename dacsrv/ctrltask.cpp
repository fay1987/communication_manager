/*==============================================================================
* 文件名称 : ctrltask.cpp
* 模块 : 处理由DACSRV同步过来的命令
* 创建时间 : 2009-07-22 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "ctrltask.h"
#include <ace/OS.h>
#include "sys/blog.h"
#include "dac/dacdef.h"
#include "dac/dacctrl.h"
#include "dac/loadpara.h"

CCtrlTask::CCtrlTask()
{

}

CCtrlTask::~CCtrlTask()
{

}

bool CCtrlTask::init()
{
	return true;
}

int CCtrlTask::start()
{
	logprint( LOG_DACSRV_CTRLTASK,"<CCtrlTask>:创建一个线程!" );
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

void CCtrlTask::stop()
{
	if (thr_count() > 0) 
	{
		thr_mgr()-> cancel_task(this);
		wait();
	}
}

int CCtrlTask::svc()
{
	m_timeId = m_timer.add(1000);	//1000ms
	m_timer.start();

	//ACE_Time_Value tv(0,10000);			//10ms
	ACE_Time_Value tv(0,100000);			//100ms
	while (true)
	{
		if ( thr_mgr()->testcancel( thr_mgr()->thr_self() ) )
			break;
		ACE_OS::sleep( tv );

		if ( m_timer.time_out(m_timeId) )
		{
			procCtrlCmd();
			m_ctrlInf.check();
			m_ctrlInf.checkAck();
		}
	}

	logprint(LOG_DACSRV_CTRLTASK,"threadId = %d , ctrlTask 线程退出!",ACE_Thread::self());
	return 0;
}

void	CCtrlTask::procCtrlCmd()
{
	hUChar cmdBuf[DAC_CTRL_LEN];
	hInt32 cmdLen;

	//查看是否有加载参数的命令
	cmdLen = m_ctrlInf.getParaLoad(cmdBuf,DAC_CTRL_LEN);
	if ( cmdLen > 0 )
		doLoadPara(cmdBuf);

	//查看是否有人工操作的命令
	cmdLen = m_ctrlInf.getManOperate(cmdBuf,DAC_CTRL_LEN);
	if ( cmdLen > 0 )
		doManOperate(cmdBuf);
}

void	CCtrlTask::doLoadPara(hUChar* cmdBuf)
{
	ctrl_head* pHead = (ctrl_head*)cmdBuf;
	switch( pHead->type )
	{
	case CTRL_LOADPARA_ALL:
		m_commInf.systemInfo()->loadParaFlag = LOADPARA_ALL;
		break;
	case CTRL_LOADPARA_STATION:
		m_commInf.systemInfo()->loadParaFlag |= LOADPARA_STATION;
		break;
	case CTRL_LOADPARA_DEVICE:
		m_commInf.systemInfo()->loadParaFlag |= LOADPARA_DEVICE;
		break;
	case CTRL_LOADPARA_DEVICETYPE:
		m_commInf.systemInfo()->loadParaFlag |= LOADPARA_DEVICETYPE;
		break;
	case CTRL_LOADPARA_DEVICEPARA:
		m_commInf.systemInfo()->loadParaFlag |= LOADPARA_DEVICEPARA;
		break;
	case CTRL_LOADPARA_CHANNEL:
		m_commInf.systemInfo()->loadParaFlag |= LOADPARA_CHANNEL;
		break;
	case CTRL_LOADPARA_GROUP:
		m_commInf.systemInfo()->loadParaFlag |= LOADPARA_GROUP;
		break;
	case CTRL_LOADPARA_ROUTE:
		m_commInf.systemInfo()->loadParaFlag |= LOADPARA_ROUTE;
		break;
	case CTRL_LOADPARA_PROTOCOL:
		m_commInf.systemInfo()->loadParaFlag |= LOADPARA_PROTOCOL;
		break;
	//wfp remove from loadtask to here at 20090924
	case CTRL_LOADPARA_YC:
		{
			ctrl_loadpara_data* pData = (ctrl_loadpara_data*)(cmdBuf + sizeof(ctrl_head));
			m_commInf.systemInfo()->loadParaFlag |= LOADPARA_YC;
			CLoadPara loadPara;
			loadPara.loadYc(pData->group);
			m_commInf.systemInfo()->loadParaFlag &= ~LOADPARA_YC;
			m_commInf.systemInfo()->loadParaFlag |= LOADPARA_ZFYC;
		}
		break;
	case CTRL_LOADPARA_YX:
		{
			ctrl_loadpara_data* pData = (ctrl_loadpara_data*)(cmdBuf + sizeof(ctrl_head));
			m_commInf.systemInfo()->loadParaFlag |= LOADPARA_YX;
			CLoadPara loadPara;
			loadPara.loadYx(pData->group);
			m_commInf.systemInfo()->loadParaFlag &= ~LOADPARA_YX;
			m_commInf.systemInfo()->loadParaFlag |= LOADPARA_ZFYX;
		}
		break;
	case CTRL_LOADPARA_KWH:
		{
			ctrl_loadpara_data* pData = (ctrl_loadpara_data*)(cmdBuf + sizeof(ctrl_head));
			m_commInf.systemInfo()->loadParaFlag |= LOADPARA_KWH;
			CLoadPara loadPara;
			loadPara.loadKwh(pData->group);
			m_commInf.systemInfo()->loadParaFlag &= ~LOADPARA_KWH;
			m_commInf.systemInfo()->loadParaFlag |= LOADPARA_ZFKWH;
		}
		break;
	default:
		break;
	}
	logprint(LOG_DACSRV_CTRLTASK,"<ctrltask>收到参数加载命令<%s>",CCtrlInf::describe(pHead->type));
}

void	CCtrlTask::doManOperate(hUChar* cmdBuf)
{
	ctrl_head* pHead = (ctrl_head*)cmdBuf;
	ctrl_man_operate* pData = (ctrl_man_operate*)(cmdBuf + sizeof(ctrl_head));
	switch ( pHead->type )
	{
	case CTRL_MAN_START_CHANNEL:
		doManStartChannel(pData->code);
		break;
	case CTRL_MAN_STOP_CHANNEL:
		doManStopChannel(pData->code);
		break;
	case CTRL_MAN_ENABLE_CHANNEL:
		doManEnableChannel(pData->code);
		break;
	case CTRL_MAN_DISABLE_CHANNEL:
		doManDisableChannel(pData->code);
		break;
	case CTRL_MAN_START_ROUTE:
		doManStartRoute(pData->code);
		break;
	case CTRL_MAN_STOP_ROUTE:
		doManStopRoute(pData->code);
		break;
	case CTRL_MAN_ENABLE_ROUTE:
		doManEnableRoute(pData->code);
		break;
	case CTRL_MAN_DISABLE_ROUTE:
		doManDisableRoute(pData->code);
		break;
	case CTRL_MAN_RESET_PROTOCOL:
		doManResetProtocol(pData->code);
		break;
	case CTRL_MAN_START_GROUP:
		doManStartGroup(pData->code);
		break;
	case CTRL_MAN_STOP_GROUP:
		doManStopGroup(pData->code);
		break;
	case CTRL_MAN_ENABLE_GROUP:
		doManEnableGroup(pData->code);
		break;
	case CTRL_MAN_DISABLE_GROUP:
		doManDisableGroup(pData->code);
		break;
	default:
		break;
	}
	logprint(LOG_DACSRV_CTRLTASK,"<ctrltask>收到人工操作命令<%s,name=%s>",CCtrlInf::describe(pHead->type),pData->code);
}

void	CCtrlTask::doManStartChannel(const char* code)
{
	DAC_CHANNEL_INFO* pInfo = m_commInf.channelInfo(code);
	if ( pInfo) 
		pInfo->manStop = false;
}

void	CCtrlTask::doManStopChannel(const char* code)
{
	DAC_CHANNEL_INFO* pInfo = m_commInf.channelInfo(code);
	if ( pInfo) 
		pInfo->manStop = true;
}

void	CCtrlTask::doManEnableChannel(const char* code)
{
	DAC_CHANNEL* pPara = m_commInf.channel(code);
	if ( pPara) 
		pPara->valid = true;
}

void	CCtrlTask::doManDisableChannel(const char* code)
{
	DAC_CHANNEL* pPara = m_commInf.channel(code);
	if ( pPara) 
		pPara->valid = false;
}

void	CCtrlTask::doManStartRoute(const char* code)
{
	DAC_ROUTE_INFO* pInfo = m_commInf.routeInfo(code);
	if ( pInfo) 
		pInfo->manStop = false;
}

void	CCtrlTask::doManStopRoute(const char* code)
{
	DAC_ROUTE_INFO* pInfo = m_commInf.routeInfo(code);
	if ( pInfo) 
		pInfo->manStop = true;
}

void	CCtrlTask::doManEnableRoute(const char* code)
{
	DAC_ROUTE *pPara = m_commInf.route(code);
	if ( pPara )
		pPara->valid = true;
}

void	CCtrlTask::doManDisableRoute(const char* code)
{
	DAC_ROUTE *pPara = m_commInf.route(code);
	if ( pPara )
		pPara->valid = false;
}

void	CCtrlTask::doManResetProtocol(const char* code)
{
	DAC_ROUTE_INFO* pInfo = m_commInf.routeInfo(code);
	if ( pInfo )
		pInfo->delProtocol = true;
}

void	CCtrlTask::doManStartGroup(const char* code)
{
	DAC_GROUP_INFO* pInfo = m_commInf.groupInfo(code);
	if ( pInfo )
		pInfo->manStop = false;
}

void	CCtrlTask::doManStopGroup(const char* code)
{
	DAC_GROUP_INFO* pInfo = m_commInf.groupInfo(code);
	if ( pInfo )
		pInfo->manStop = true;
}

void	CCtrlTask::doManEnableGroup(const char* code)
{
	DAC_GROUP *pPara = m_commInf.group(code);
	if ( pPara )
		pPara->valid = true;
}

void	CCtrlTask::doManDisableGroup(const char* code)
{
	DAC_GROUP *pPara = m_commInf.group(code);
	if ( pPara )
		pPara->valid = false;
}

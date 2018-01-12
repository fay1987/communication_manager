/*==============================================================================
* 文件名称 : link.cpp
* 模块 : 链路类
* 创建时间 : 2009-08-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include <ace/DLL.h>
#include "sys/sysmacro.h"
#include "dac/dacdef.h"
#include "sys/blog.h"
#include "link.h"
#include <QString>
#include <QStringList>

typedef	CProtocol *	(*FuncCreateProtocol)();
//==============================================================================
CLink::CLink(hInt32 no)
:m_chanNo(no)
{
	ACE_OS::memset(m_pProtocol,0,sizeof(m_pProtocol));
	m_isOpen = false;	
	m_bTimeOut = false;
}

CLink::~CLink()
{
	for (hUInt32 i=0;i<DAC_ROUTE_IN_CHANNEL;i++)
	{
		destroyProtocol(i);
	}
	m_isOpen = false;
}

/*
 * 函数功能：初始化规约
 * 函数参数：在channel中的路径序号
 */
bool CLink::initProtocol(hInt32 routeOrder)
{
	if ( routeOrder < 0 || routeOrder >= DAC_ROUTE_IN_CHANNEL ) return false;
	if ( m_pProtocol[routeOrder] )
		return true;

	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_chanNo);
	if( !pChannelInfo )
		return false;

	DAC_ROUTE* pRoute = m_commInf.route(pChannelInfo->routes[routeOrder]);
	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(pChannelInfo->routes[routeOrder]);
	if ( !pRouteInfo ) return false;

	DAC_PROTOCOL *pProtocol = m_commInf.protocol(pRoute->protocol);
	if(!pProtocol) return false;

	char			dllName[MAXPATHLEN+MAXNAMELEN];
	ACE_DLL			dllProtocol;

#ifdef WIN32 
	ACE_OS::sprintf(dllName,"%s\\dll\\%s.dll",ACE_OS::getenv(SYS_ENV_VAR),pProtocol->libName);
#else
	ACE_OS::sprintf(dllName,"%s/lib/lib%s.so",ACE_OS::getenv(SYS_ENV_VAR),pProtocol->libName);
#endif


	if(dllProtocol.open(dllName,ACE_DEFAULT_SHLIB_MODE,0) != 0)
	{
		logprint(LOG_DACSRV_LINK,"<LINK %d>load protocol lib<%s> faild!",m_chanNo,pProtocol->libName);
		return false;
	}
	FuncCreateProtocol	pFuncProtocol = (FuncCreateProtocol)dllProtocol.symbol("CreateProtocol");
	if(!pFuncProtocol)
		return false;

	m_pProtocol[routeOrder] = pFuncProtocol();

	pRouteInfo->delProtocol = false;

	logprint(LOG_DACSRV_LINK,"<LINK %d>load protocol lib<%s> sucess!",m_chanNo,pProtocol->libName);

	if ( m_pProtocol[routeOrder] )
		return m_pProtocol[routeOrder]->init(m_chanNo,pChannelInfo->routes[routeOrder]);
	else
		return false;

}

bool CLink::isValid() const
{
	const DAC_CHANNEL* pChannel = m_commInf.channel(m_chanNo);
	if ( pChannel == NULL ) return false;

	return true;
}

bool CLink::isOpen()	const
{
	return m_isOpen;
}

bool CLink::open()
{
	DAC_CHANNEL* pChannel = m_commInf.channel(m_chanNo);
	if ( pChannel == NULL ) return false;
	DAC_CHANNEL_INFO	*pChannelInfo = m_commInf.channelInfo(m_chanNo);
	if ( pChannelInfo == NULL )return false;

	/*/wfp add at 20110616 for 连接后10ms内即收到的报文规约无法捕获的问题
	采用方法:提前初始化规约*/
	DAC_ROUTE* pRoute = NULL;
	DAC_ROUTE_INFO* pRouteInfo = NULL;
	for (hUInt32 i=0;i<DAC_ROUTE_IN_CHANNEL;i++)
	{
		pRoute = m_commInf.route(pChannelInfo->routes[i]);
		if ( pRoute == NULL ) continue;
		pRouteInfo = m_commInf.routeInfo(pChannelInfo->routes[i]);
		if ( pRouteInfo == NULL ) continue;

		if ( !m_pProtocol[i] )  
			initProtocol(i);
	}
	////////////////////wfp add end///////////////////////////////////////

	if ( pChannel->type != CHANNEL_TYPE_VIRTUAL && pChannelInfo->state != CHANNEL_STATE_UP ) return false;

	m_isOpen = true;
	m_curRouteBegin = ACE_OS::gettimeofday();
	logprint(LOG_DACSRV_LINK,"<LINK %d>打开成功!",m_chanNo);
	return true;
}

void CLink::close()
{
	for (hUInt32 i=0;i<DAC_ROUTE_IN_CHANNEL;i++)
	{
		if ( m_pProtocol[i] && m_pProtocol[i]->isOpen() ) m_pProtocol[i]->close();
	}
	m_isOpen = false;
}

void CLink::destroyProtocol(hInt32 routeOrder)
{
	if ( routeOrder < 0 || routeOrder >= DAC_ROUTE_IN_CHANNEL ) return ;
	if ( m_pProtocol[routeOrder] )
	{
		if ( m_pProtocol[routeOrder]->isOpen() ) m_pProtocol[routeOrder]->close();
		delete m_pProtocol[routeOrder];
		m_pProtocol[routeOrder] = NULL;
	}

}

bool CLink::check()
{
	DAC_CHANNEL* pChannel = m_commInf.channel(m_chanNo);
	if ( pChannel == NULL ) return false;
	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_chanNo);
	if ( pChannelInfo == NULL ) return false;

	if ( pChannelInfo->reset & CHANNEL_TASK_USER )
	{
		close();
		pChannelInfo->reset &= ~CHANNEL_TASK_USER;
		return false;
	}

	//虚拟通道无需关心通道状态
	if ( pChannel->type != CHANNEL_TYPE_VIRTUAL && pChannelInfo->state != CHANNEL_STATE_UP ) 
	{
		close();
		return false;
	}
	
	DAC_ROUTE* pRoute = NULL;
	DAC_ROUTE_INFO* pRouteInfo = NULL;
	for (hUInt32 i=0;i<DAC_ROUTE_IN_CHANNEL;i++)
	{
		pRoute = m_commInf.route(pChannelInfo->routes[i]);
		if ( pRoute == NULL ) continue;
		pRouteInfo = m_commInf.routeInfo(pChannelInfo->routes[i]);
		if ( pRouteInfo == NULL ) continue;

		if ( pRouteInfo->delProtocol )
		{
			destroyProtocol(i);
			pRouteInfo->delProtocol = false;
		}
		
		if ( !m_pProtocol[i] ) 
			initProtocol(i);

		if ( m_pProtocol[i] && !m_pProtocol[i]->isOpen() )
		{
			m_pProtocol[i]->init(m_chanNo,pChannelInfo->routes[i]);		//wfp add at 20101115
			m_pProtocol[i]->open();
		}

	}

	return true;
}

/*
 * 函数说明：
 */
void CLink::run()
{
	if ( !check() ) return;

	DAC_CHANNEL_INFO* pChannelInfo = m_commInf.channelInfo(m_chanNo);
	DAC_CHANNEL*	pChannel = m_commInf.channel(m_chanNo);
	if ( pChannelInfo == NULL ) return;
	if ( pChannel == NULL ) return;

	DAC_ROUTE* pRoute = NULL;
	DAC_ROUTE_INFO* pRouteInfo = NULL;

	if ( pChannelInfo->routeNum <= 0 ) 
		return;

	//收处理
	//for (hUInt32 routeOrder=0;routeOrder<pChannelInfo->routeNum;routeOrder++)
	//{
	//	if ( m_pProtocol[routeOrder] == NULL || !m_pProtocol[routeOrder]->isOpen() )
	//		continue;

	//	pRouteInfo = m_commInf.routeInfo(pChannelInfo->routes[routeOrder]);
	//	if ( pRouteInfo == NULL || pRouteInfo->manStop )
	//		continue;

	//	m_pProtocol[routeOrder]->mainRx();
	//	//由规约内部设置:if (发送数据后接收完成) route[j].isOver = true;
	//}
	//for (hUInt32 routeOrder=0;routeOrder<pChannelInfo->routeNum;routeOrder++)
	{
		if ( m_pProtocol[pChannelInfo->curRouteOrder] != NULL && m_pProtocol[pChannelInfo->curRouteOrder]->isOpen())
		{
			pRouteInfo = m_commInf.routeInfo(pChannelInfo->routes[pChannelInfo->curRouteOrder]);
			if ( pRouteInfo != NULL || !pRouteInfo->manStop )
			{
				if(!pRouteInfo->switchFlag)
				{
					if ((CONFIG::instance()->timeinterval() > 0) && m_bTimeOut)
					{
						//m_pProtocol[pChannelInfo->curRouteOrder]->setTimeOut();
						//hInt32 routeno = pChannelInfo->routes[pChannelInfo->curRouteOrder];
						//CDateTime tNow = CDateTime::currentDateTime();
						//logprint(20000+ routeno, "接收： -- %02d:%02d:%02d  %03d,  接收超时",tNow.hour(), tNow.minute(), tNow.second(), tNow.microsec()/1000);

						m_pProtocol[pChannelInfo->curRouteOrder]->setTimeOut(m_bTimeOut);
						m_bTimeOut = false;
					}
					m_pProtocol[pChannelInfo->curRouteOrder]->mainRx();
				}
			}

		}
		//由规约内部设置:if (发送数据后接收完成) route[j].isOver = true;
	}

	//切换chan的当前路径(可下行命令)
	if ( pChannelInfo->routeNum > 1 )
	{
		int curRoute = pChannelInfo->routes[pChannelInfo->curRouteOrder];
		pRoute = m_commInf.route(curRoute);
		/*pRouteInfo = m_commInf.routeInfo(curRoute);*/

		//规约不正常或路径无效
		if ( m_pProtocol[pChannelInfo->curRouteOrder] == NULL 
			 || !m_pProtocol[pChannelInfo->curRouteOrder]->isOpen()
			 || pRoute == NULL 
			 || pRouteInfo == NULL )
		{
			pChannelInfo->curRouteOrder = (pChannelInfo->curRouteOrder + 1) % pChannelInfo->routeNum;
			m_curRouteBegin = ACE_OS::gettimeofday();	//新curRoute开始计时
			logprint(LOG_DACSRV_LINK,"<Link %d>规约异常或路径无效，从当前路径<%d>切换到下一个路径<%d>",
				m_chanNo,curRoute,(hInt32)pChannelInfo->routes[pChannelInfo->curRouteOrder]);

			//执行发处理，同时置switchflag = false
			pRouteInfo = m_commInf.routeInfo(pChannelInfo->routes[pChannelInfo->curRouteOrder]);
			if ( pRouteInfo && !pRouteInfo->manStop )
			{
				if( m_pProtocol[pChannelInfo->curRouteOrder] && m_pProtocol[pChannelInfo->curRouteOrder]->isOpen() )
				{
					m_pProtocol[pChannelInfo->curRouteOrder]->mainTx();
					pRouteInfo->switchFlag = false;
				}
			}
		}
		else
		{//timeout考虑其中一个设备不响应，会影响其他设备的时间.
			if ( pRouteInfo->manStop
				 ||  pRouteInfo->switchFlag 
				 || (ACE_OS::gettimeofday() - m_curRouteBegin).msec() > pChannel->switchTimeOut*1000	/*ROUTE_TIME_OUT*1000*/ )
			{
				//如果超过了通道路径切换时间，则切换
				if(( ACE_OS::gettimeofday() - m_curRouteBegin).msec() > pChannel->switchTimeOut * 1000 
				||  pRouteInfo->switchFlag)
				{
					if(CONFIG::instance()->timeinterval() > 0)
					{
						m_pProtocol[pChannelInfo->curRouteOrder]->setTimeOut();
						if (!(m_pProtocol[pChannelInfo->curRouteOrder]->isExclusive()))
						{
							pChannelInfo->curRouteOrder = (pChannelInfo->curRouteOrder + 1) % pChannelInfo->routeNum;
						}
					}
					else
					{
						pRouteInfo->switchFlag = true;
						pChannelInfo->curRouteOrder = (pChannelInfo->curRouteOrder + 1) % pChannelInfo->routeNum;
					}
					

					m_curRouteBegin = ACE_OS::gettimeofday();	//新curRoute开始计时

					CDateTime tNow = CDateTime::currentDateTime();
					logprint(20000+ curRoute, "路径manstop/need switch/timeout，从当前路径<%d>切换到下一个路径(%d)", 
						curRoute,(hInt32)pChannelInfo->routes[pChannelInfo->curRouteOrder]);

					logprint(LOG_DACSRV_LINK,"<Link %d>路径manstop/need switch/timeout，从当前路径<%d>切换到下一个路径<%d>"
						,m_chanNo,curRoute,(hInt32)pChannelInfo->routes[pChannelInfo->curRouteOrder]);

					//执行发处理，同时置switchflag = false
					pRouteInfo = m_commInf.routeInfo(pChannelInfo->routes[pChannelInfo->curRouteOrder]);
					if ( pRouteInfo && !pRouteInfo->manStop )
					{
						if( m_pProtocol[pChannelInfo->curRouteOrder] && m_pProtocol[pChannelInfo->curRouteOrder]->isOpen() )
						{
							m_pProtocol[pChannelInfo->curRouteOrder]->mainTx();
							logprint(LOG_DACSRV_LINK,"<Link %d>发送数据！<%d>",m_chanNo,curRoute);
							logprint(20000+ curRoute, "mainTx 发送数据");
							pRouteInfo->switchFlag = false;
						}
					}

				}
				//pChannelInfo->curRouteOrder = (pChannelInfo->curRouteOrder + 1) % pChannelInfo->routeNum;
				//m_curRouteBegin = ACE_OS::gettimeofday();	//新curRoute开始计时
				//logprint(LOG_DACSRV_LINK,"<Link %d>路径manstop/need switch/timeout，从当前路径<%d>切换到下一个路径<%d>"
				//						  ,m_chanNo,curRoute,(hInt32)pChannelInfo->routes[pChannelInfo->curRouteOrder]);
			}
			else
			{
				//该处应增加(发送数据后接收完成后的切换) route[j].isOver = true
				//if(pRouteInfo->isOver == true
			}
		}

		return;

	}//end if

	//发处理
	if ( m_pProtocol[pChannelInfo->curRouteOrder] == NULL || !m_pProtocol[pChannelInfo->curRouteOrder]->isOpen() )
		return;

	pRouteInfo = m_commInf.routeInfo(pChannelInfo->routes[pChannelInfo->curRouteOrder]);
	if ( pRouteInfo == NULL || pRouteInfo->manStop )
		return;

	logprint(LOG_DACSRV_LINK,"<Link %d>正在执行protocol[curRoute=%d].mainTx()",
		                      m_chanNo,(int)(pChannelInfo->routes[pChannelInfo->curRouteOrder]));

	m_pProtocol[pChannelInfo->curRouteOrder]->mainTx();

}


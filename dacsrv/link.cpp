/*==============================================================================
* �ļ����� : link.cpp
* ģ�� : ��·��
* ����ʱ�� : 2009-08-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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
 * �������ܣ���ʼ����Լ
 * ������������channel�е�·�����
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

	/*/wfp add at 20110616 for ���Ӻ�10ms�ڼ��յ��ı��Ĺ�Լ�޷����������
	���÷���:��ǰ��ʼ����Լ*/
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
	logprint(LOG_DACSRV_LINK,"<LINK %d>�򿪳ɹ�!",m_chanNo);
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

	//����ͨ���������ͨ��״̬
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
 * ����˵����
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

	//�մ���
	//for (hUInt32 routeOrder=0;routeOrder<pChannelInfo->routeNum;routeOrder++)
	//{
	//	if ( m_pProtocol[routeOrder] == NULL || !m_pProtocol[routeOrder]->isOpen() )
	//		continue;

	//	pRouteInfo = m_commInf.routeInfo(pChannelInfo->routes[routeOrder]);
	//	if ( pRouteInfo == NULL || pRouteInfo->manStop )
	//		continue;

	//	m_pProtocol[routeOrder]->mainRx();
	//	//�ɹ�Լ�ڲ�����:if (�������ݺ�������) route[j].isOver = true;
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
						//logprint(20000+ routeno, "���գ� -- %02d:%02d:%02d  %03d,  ���ճ�ʱ",tNow.hour(), tNow.minute(), tNow.second(), tNow.microsec()/1000);

						m_pProtocol[pChannelInfo->curRouteOrder]->setTimeOut(m_bTimeOut);
						m_bTimeOut = false;
					}
					m_pProtocol[pChannelInfo->curRouteOrder]->mainRx();
				}
			}

		}
		//�ɹ�Լ�ڲ�����:if (�������ݺ�������) route[j].isOver = true;
	}

	//�л�chan�ĵ�ǰ·��(����������)
	if ( pChannelInfo->routeNum > 1 )
	{
		int curRoute = pChannelInfo->routes[pChannelInfo->curRouteOrder];
		pRoute = m_commInf.route(curRoute);
		/*pRouteInfo = m_commInf.routeInfo(curRoute);*/

		//��Լ��������·����Ч
		if ( m_pProtocol[pChannelInfo->curRouteOrder] == NULL 
			 || !m_pProtocol[pChannelInfo->curRouteOrder]->isOpen()
			 || pRoute == NULL 
			 || pRouteInfo == NULL )
		{
			pChannelInfo->curRouteOrder = (pChannelInfo->curRouteOrder + 1) % pChannelInfo->routeNum;
			m_curRouteBegin = ACE_OS::gettimeofday();	//��curRoute��ʼ��ʱ
			logprint(LOG_DACSRV_LINK,"<Link %d>��Լ�쳣��·����Ч���ӵ�ǰ·��<%d>�л�����һ��·��<%d>",
				m_chanNo,curRoute,(hInt32)pChannelInfo->routes[pChannelInfo->curRouteOrder]);

			//ִ�з�����ͬʱ��switchflag = false
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
		{//timeout��������һ���豸����Ӧ����Ӱ�������豸��ʱ��.
			if ( pRouteInfo->manStop
				 ||  pRouteInfo->switchFlag 
				 || (ACE_OS::gettimeofday() - m_curRouteBegin).msec() > pChannel->switchTimeOut*1000	/*ROUTE_TIME_OUT*1000*/ )
			{
				//���������ͨ��·���л�ʱ�䣬���л�
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
					

					m_curRouteBegin = ACE_OS::gettimeofday();	//��curRoute��ʼ��ʱ

					CDateTime tNow = CDateTime::currentDateTime();
					logprint(20000+ curRoute, "·��manstop/need switch/timeout���ӵ�ǰ·��<%d>�л�����һ��·��(%d)", 
						curRoute,(hInt32)pChannelInfo->routes[pChannelInfo->curRouteOrder]);

					logprint(LOG_DACSRV_LINK,"<Link %d>·��manstop/need switch/timeout���ӵ�ǰ·��<%d>�л�����һ��·��<%d>"
						,m_chanNo,curRoute,(hInt32)pChannelInfo->routes[pChannelInfo->curRouteOrder]);

					//ִ�з�����ͬʱ��switchflag = false
					pRouteInfo = m_commInf.routeInfo(pChannelInfo->routes[pChannelInfo->curRouteOrder]);
					if ( pRouteInfo && !pRouteInfo->manStop )
					{
						if( m_pProtocol[pChannelInfo->curRouteOrder] && m_pProtocol[pChannelInfo->curRouteOrder]->isOpen() )
						{
							m_pProtocol[pChannelInfo->curRouteOrder]->mainTx();
							logprint(LOG_DACSRV_LINK,"<Link %d>�������ݣ�<%d>",m_chanNo,curRoute);
							logprint(20000+ curRoute, "mainTx ��������");
							pRouteInfo->switchFlag = false;
						}
					}

				}
				//pChannelInfo->curRouteOrder = (pChannelInfo->curRouteOrder + 1) % pChannelInfo->routeNum;
				//m_curRouteBegin = ACE_OS::gettimeofday();	//��curRoute��ʼ��ʱ
				//logprint(LOG_DACSRV_LINK,"<Link %d>·��manstop/need switch/timeout���ӵ�ǰ·��<%d>�л�����һ��·��<%d>"
				//						  ,m_chanNo,curRoute,(hInt32)pChannelInfo->routes[pChannelInfo->curRouteOrder]);
			}
			else
			{
				//�ô�Ӧ����(�������ݺ������ɺ���л�) route[j].isOver = true
				//if(pRouteInfo->isOver == true
			}
		}

		return;

	}//end if

	//������
	if ( m_pProtocol[pChannelInfo->curRouteOrder] == NULL || !m_pProtocol[pChannelInfo->curRouteOrder]->isOpen() )
		return;

	pRouteInfo = m_commInf.routeInfo(pChannelInfo->routes[pChannelInfo->curRouteOrder]);
	if ( pRouteInfo == NULL || pRouteInfo->manStop )
		return;

	logprint(LOG_DACSRV_LINK,"<Link %d>����ִ��protocol[curRoute=%d].mainTx()",
		                      m_chanNo,(int)(pChannelInfo->routes[pChannelInfo->curRouteOrder]));

	m_pProtocol[pChannelInfo->curRouteOrder]->mainTx();

}


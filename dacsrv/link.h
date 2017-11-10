/*==============================================================================
* 文件名称 : link.h
* 模块 : 链路类
* 创建时间 : 2009-08-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_LINK_H_
#define	_PDT_DAC_LINK_H_

#include "ace/OS.h"
#include "utl/string.h"
#include "dac/dacdef.h"
#include "dac/comminf.h"
#include "dac/protocol.h"

using namespace PDT;

#define ROUTE_TIME_OUT		3		//路径超时时间(用于one channel - multi routes)

class CLink
{
public:
	CLink(hInt32 no);
	virtual ~CLink();

	bool		init(hInt32 no);
public:
	
	bool		isValid()	const;
	//是否打开
	bool		isOpen()	const;
	//打开
	bool		open();
	//关闭
	void		close();
	//主处理过程
	void		run();
	void	setTimeOutFlag(hBool bFlag){m_bTimeOut = bFlag;}

protected:
	bool		initProtocol(hInt32 routeOrder);
	void		destroyProtocol(hInt32 routeOrder);
	bool		check();
private:
	//共享区
	CCommInf		m_commInf;
	//对象区
	CProtocol		*m_pProtocol[DAC_ROUTE_IN_CHANNEL];	//规约指针
	//数据区
	hInt32			m_chanNo;							//通道号
	bool			m_isOpen;
	ACE_Time_Value  m_curRouteBegin;					//当前运行路径的起始时间
	hBool			m_bTimeOut;
};

#endif //_PDT_DAC_LINK_H_

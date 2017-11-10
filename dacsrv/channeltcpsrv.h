/*==============================================================================
* 文件名称 : channeltcpsrv.h
* 模块 : 电力数据采集与转发平台--平台进程(pdat.exe)--TCP服务端通道类
* 创建时间 : 2008-12-01 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef __DAC_CHANNELTCPSRV_H__
#define __DAC_CHANNELTCPSRV_H__

#include "channel.h"
#include <QtNetwork>
#include "dac/comminf.h"

class CChannelTcpSrv : public CChannel
{
	Q_OBJECT
public:
	CChannelTcpSrv(int channelId);
	virtual ~CChannelTcpSrv(void);

	void	open();
	void	close();
	void	remove();
	void	removeStream();

	protected Q_SLOTS:
		void	onNewConnection();
private:
	QTcpServer		*m_pSocketDev;
	CChannel		*m_pChannel;		//用于保存当前连接
	PDT::CCommInf	m_commInf;
};



#endif //__DAC_CHANNELTCPSRV_H__

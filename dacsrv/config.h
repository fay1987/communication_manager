#ifndef _PDT_DAC_CONFIG_H_
#define _PDT_DAC_CONFIG_H_

#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <sys/sysmacro.h>
#include <sys/blog.h>

#define DACSRV_CONFIG_FILE			"dacsrv.ini"
#define MIN_NETSEND_CHECK_INTERVAL	20				//最小间隔(20ms)

class CConfig
{
public:
	CConfig();
	virtual ~CConfig();

	bool			read();

	hUInt32			linkThreadPoolSize() const;
	hUInt32			netThreadPoolSize() const;
	hUInt32			serialThreadPoolSize() const;
	hUInt32			qualityInvalidInterval() const;
	hUInt32			netSendCheckInterval() const;
	bool			deadCheck() const;
	//event
	bool			filterChannel() const;
	bool			filterRoute() const;
	bool			filterGroup() const;
	bool			filterBegin() const;

private:
	hUInt32			m_linkThreadPoolSize;		//链路处理线程池大小
	hUInt32			m_netThreadPoolSize;		//网络线程池大小
	hUInt32			m_serialThreadPoolSize;		//串口线程池大小
	hUInt32			m_qualityInvalidInterval;	//实时数据质量码无效间隔(s)
	hUInt32			m_netSendCheckInterval;		//网络数据发送检测间隔
	bool			m_deadCheck;				//规约处理线程dead检测
	//event
	bool			m_filterChannel;			//过滤通道事项
	bool			m_filterRoute;				//过滤路径事项
	bool			m_filterGroup;				//过滤数据组事项
	bool			m_filterBegin;				//过滤通道&数据组&路径第一次启动事项
};

typedef	ACE_Singleton<CConfig,ACE_SYNCH_MUTEX>	CONFIG;

#endif	//_PDT_DAC_CONFIG_H_

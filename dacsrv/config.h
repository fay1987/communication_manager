#ifndef _PDT_DAC_CONFIG_H_
#define _PDT_DAC_CONFIG_H_

#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <sys/sysmacro.h>
#include <sys/blog.h>

#define DACSRV_CONFIG_FILE			"dacsrv.ini"
#define MIN_NETSEND_CHECK_INTERVAL	20				//��С���(20ms)

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
	hUInt32			m_linkThreadPoolSize;		//��·�����̳߳ش�С
	hUInt32			m_netThreadPoolSize;		//�����̳߳ش�С
	hUInt32			m_serialThreadPoolSize;		//�����̳߳ش�С
	hUInt32			m_qualityInvalidInterval;	//ʵʱ������������Ч���(s)
	hUInt32			m_netSendCheckInterval;		//�������ݷ��ͼ����
	bool			m_deadCheck;				//��Լ�����߳�dead���
	//event
	bool			m_filterChannel;			//����ͨ������
	bool			m_filterRoute;				//����·������
	bool			m_filterGroup;				//��������������
	bool			m_filterBegin;				//����ͨ��&������&·����һ����������
};

typedef	ACE_Singleton<CConfig,ACE_SYNCH_MUTEX>	CONFIG;

#endif	//_PDT_DAC_CONFIG_H_

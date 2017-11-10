/*==============================================================================
* �ļ����� : intervaltimerset.h
* ģ�� : ��ʱ����
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : wlq
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 2007��7��25��
* �������� : ACE_Time_Value::msec()����unsigned����,���,��ʱ���ǰ,���޷����ظ���.
==============================================================================*/
#ifndef _PDT_UTL_INTERVAL_TIMER_H_
#define _PDT_UTL_INTERVAL_TIMER_H_

#include "ace/Time_Value.h"
#include "utl/utl_export.h"

#define MAX_INTERVAL_NUM	32		//���֧�ֵļ������

typedef struct _timer_info_ 
{
	bool			valid;			//��Ч��־
	bool			isStart;		//�Ƿ�������
	int				interval;		//��ʱ��ʱ����;
	ACE_Time_Value	tvBegin;		//��ʼ�����ʱ
}TIMER_INFO;

namespace PDT
{
	class UTL_EXPORT CIntervalTimerSet
	{
	public:
		CIntervalTimerSet();
		virtual ~CIntervalTimerSet();

		/**
		* @function			����һ����ʱ��
		* @param interval	��ʱ���(ms)
		* @retval			timeId:���ڼ�¼��ǰ��ʱ��
		*/
		int				add(int interval);	
		//ɾ����ʱ��
		void			remove(int timerId);			
		void			removeAll();
		//������ʱ������������Ч�Ķ�ʱ��
		void			start();
		//����ָ����ʱ��
		void			start(int timerId);
		//ֹͣ��ʱ����������Ч�Ķ�ʱ��
		void			stop();
		//ָֹͣ����ʱ��
		void			stop(int timerId);
		//��ʱ����ʱ�ж�
		bool			time_out(int timerId);					
	private:
		int				validPos();
		TIMER_INFO		m_timerInfos[MAX_INTERVAL_NUM];
	};
}

#endif //_PDT_UTL_INTERVAL_TIMER_H_

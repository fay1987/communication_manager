/*==============================================================================
* 文件名称 : intervaltimerset.h
* 模块 : 定时器集
* 创建时间 : 2008-04-28 15:55:23
* 作者 : wlq
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 2007年7月25日
* 遗留问题 : ACE_Time_Value::msec()返回unsigned类型,因而,若时间改前,则无法返回负数.
==============================================================================*/
#ifndef _PDT_UTL_INTERVAL_TIMER_H_
#define _PDT_UTL_INTERVAL_TIMER_H_

#include "ace/Time_Value.h"
#include "utl/utl_export.h"

#define MAX_INTERVAL_NUM	32		//最多支持的间隔个数

typedef struct _timer_info_ 
{
	bool			valid;			//有效标志
	bool			isStart;		//是否已启动
	int				interval;		//定时的时间间隔;
	ACE_Time_Value	tvBegin;		//起始间隔计时
}TIMER_INFO;

namespace PDT
{
	class UTL_EXPORT CIntervalTimerSet
	{
	public:
		CIntervalTimerSet();
		virtual ~CIntervalTimerSet();

		/**
		* @function			增加一个定时器
		* @param interval	定时间隔(ms)
		* @retval			timeId:用于记录当前定时器
		*/
		int				add(int interval);	
		//删除定时器
		void			remove(int timerId);			
		void			removeAll();
		//启动定时器集中所有有效的定时器
		void			start();
		//启动指定定时器
		void			start(int timerId);
		//停止定时器集所有有效的定时器
		void			stop();
		//停止指定定时器
		void			stop(int timerId);
		//定时器到时判断
		bool			time_out(int timerId);					
	private:
		int				validPos();
		TIMER_INFO		m_timerInfos[MAX_INTERVAL_NUM];
	};
}

#endif //_PDT_UTL_INTERVAL_TIMER_H_

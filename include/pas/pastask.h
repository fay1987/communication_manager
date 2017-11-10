/*==============================================================================
* 文件名称 : pastask.h
* 模块 : 高级应用任务基类
* 创建时间 : 2011-11-25 15:55:23
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_BFS_PAS_TASK_H_
#define	_BFS_PAS_TASK_H_

#include "utl/datatype.h"
#include "ace/Task.h"

namespace BFS
{
		class CPasTask : public ACE_Task<ACE_NULL_SYNCH>
		{
		public:
			CPasTask(){}
			virtual ~CPasTask(){}
			//是否打开
			virtual	bool	init() = 0;
			//打开规约
			virtual int		start() = 0;
			//关闭规约
			virtual void	stop() = 0;
		};
}
#endif //_BFS_PAS_TASK_H_

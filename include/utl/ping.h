/*==============================================================================
* 文件名称 : nethelper.h
* 模块 : 关系库操作接口(多数据库支持)
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 2008年08月08日
==============================================================================*/
#ifndef _PDT_DATABASE_PING_H_
#define _PDT_DATABASE_PING_H_

#include "utl/utl_export.h"

namespace PDT
{
	class UTL_EXPORT CPing
	{
	public:
		CPing(void);
		virtual ~CPing(void);
		
		static bool	ping(const char* ip,unsigned short port,bool connectFlag=false,int timeout=1000);
	private:
	};
}


#endif //_PDT_DATABASE_PING_H_

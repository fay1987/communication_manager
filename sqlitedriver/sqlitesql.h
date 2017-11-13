/*==============================================================================
* 文件名称 : sqlitesql.h
* 模块 : SQLITE SQL语句构造器
* 创建时间 : 2011-03-26 15:55:23
* 作者 : wlq
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DATABASE_SQLITESQL_H_
#define	_PDT_DATABASE_SQLITESQL_H_

#include "rdb/sql.h"
#include "rdb/driver_export.h"

namespace	PDT
{
	class DRIVER_EXPORT  CSqliteSQL : public CSql
	{
	public:
	public:
		CSqliteSQL();
		virtual	~CSqliteSQL();
	public:
		//释放对象
		void	destroy();
	private:
		virtual CString valueToString(const CVariant& value) const;
	};
};
//---------------------------------------------------------------------------------------

#endif //_PDT_DATABASE_SQLITESQL_H_

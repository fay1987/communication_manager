/*==============================================================================
* 文件名称 : sqlitedriver.h
* 模块 : SQLITE 数据库驱动
* 创建时间 : 2011-03-28 15:55:23
* 作者 : wlq
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef _PDT_DATABASE_SQLITEDRIVER_H_
#define _PDT_DATABASE_SQLITEDRIVER_H_

#include "rdb/dbdriver.h"
#include <ace/config-all.h>
#include "rdb/driver_export.h"
#include "sqlite/sqlite3.h"

namespace PDT
{

	class DRIVER_EXPORT CSqliteDriver : public CDbDriver
	{
	public:
		CSqliteDriver();
		virtual	~CSqliteDriver();
	public:
		//连接数据库
		bool open( const CString & db,
			const CString & srvname,
			const CString & user     = "",
			const CString & password = "",
			const CString & host     = "", 
			int       port     = -1 );
		//关闭数据库连接
		void close();
		//-------------------------------------------------------------------------
		//开始事务
		bool beginTransaction();
		bool commitTransaction();
		bool rollbackTransaction();
		//-------------------------------------------------------------------------
		//表名列表（返回个数）
		int tableNames( CStringArray & o_nameList,
			DB::TableType  i_tableType = DB::TT_AllTables );
		//表信息
		bool tableInfo( const char       * i_tableName,
			CTableInfo & o_tableInfo );
		//SQL信息
		bool	sqlInfo(const char *sql,CTableInfo &tableInfo);
		//执行SQL语句
		int exec( const char * i_sql );
		int exec( const char * i_sql, CDataset & o_dataset ,size_t step=PDT_DB_SELECT_STEP);
		//批量提交（true--全部提交成功,否则返回false）
		bool exec(CDataset & dataset);
		//-------------------------------------------------------------------------
		//返回当前数据库默认的NULL的字符串
		CString nullText() const { return "NULL"; }

		//判断是否已经连接到数据库
		bool	isOpen();
		bool	isOpenError();
		bool    testDB();

	protected:

	private:
		sqlite3*		m_pDb;

		CVariant::Type	type(int type);
		CVariant::Type	type(const char* typeName);
		void			setTableInfo(CTableInfo& tableInfo,sqlite3_stmt *stmt);
		void			setFieldValue(CField& field,const CFieldInfo& fieldInfo,sqlite3_stmt *stmt,int colIndex);
		bool			insertDataset(CDataset& ds);
		bool			updateDataset(CDataset& ds);
		bool			deleteDataset(CDataset& ds);
		bool			handleDataset(CDataset& ds);
	};
};
#endif //_PDT_DATABASE_SQLITEDRIVER_H_

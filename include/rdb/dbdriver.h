/*==============================================================================
* 文件名称 : dbdriver.h
* 模块 : 数据库驱动基类
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 2007年7月25日
==============================================================================*/
#ifndef	_PDT_DATABASE_DBDRIVER_H_
#define	_PDT_DATABASE_DBDRIVER_H_

#include "rdb/db_export.h"
#include "rdb/dbdef.h"
#include "rdb/dberror.h"
#include "rdb/tableinfo.h"
#include "rdb/dataset.h"
#include "ace/SString.h"

namespace PDT
{
	struct ConnectionPool 
	{
		int		initConnCount;
		int		maxConnCount;
		int		stepConnCount;
	};

	class	DB_EXPORT CDbDriver
	{
		friend class CDatabase;
	public:
		CDbDriver();
		virtual	~CDbDriver();
	public:
		enum DriverFeatrue{ Transaction,ConnectionPool,BLOB,NamedPlaceholders, PositionalPlaceholders };

		//获取驱动支持的功能，应在具体的driver中用静态函数实现
		//virtual	bool hasDriverProperty( DriverFeatrue featrue);
		//设置驱动某种功能的属性，如：连接池的起始连接数，最大连接数，递增连接数，应在具体的driver中用静态函数实现
		//virtual bool setDriverProperty( DriverFeatrue featrue,void* buf);

		//连接数据库，纯虚函数，派生类必须实现
		virtual bool open ( const CString & db , const CString& srvname,const CString & user = "", const CString & password = "", const CString & host = "", int port = -1 ) = 0;
		//关闭数据库连接，纯虚函数，派生类必须实现
		virtual void close () = 0;
		//-------------------------------------------------------------------------
		//开始事务
		virtual bool beginTransaction();
		virtual bool commitTransaction();
		virtual bool rollbackTransaction();
		//-------------------------------------------------------------------------
		//表名列表（返回个数）
		virtual int		tableNames(CStringArray &nameList,DB::TableType tableType=DB::TT_AllTables) = 0;
		//表信息
		virtual bool	tableInfo(const char *tableName,CTableInfo &tableInfo) = 0;
		virtual bool	sqlInfo(const char *sql,CTableInfo &tableInfo) = 0;
		//-------------------------------------------------------------------------
		//错误信息
		virtual	const	CDbError	&lastError()	const;
		//-------------------------------------------------------------------------
		//执行SQL语句
		virtual int		exec(const char *sql) = 0;
		virtual int		exec(const char *sql,CDataset &dataset,size_t step=PDT_DB_SELECT_STEP) = 0;
		//提交数据集
		virtual bool    exec(CDataset & dataset);
		//-------------------------------------------------------------------------
		//返回当前数据库默认的NULL的字符串
		virtual CString nullText() const;
	protected:
		virtual void setOpen ( bool o );
		virtual void setOpenError ( bool e );
		virtual void setLastError ( const CDbError & e );
	public:
		//连接数据库
		bool	open ( const CString & db, const CString& srvname,const CString & user, const CString & password, const CString & host, int port, const CString & connOpts );
		//判断是否已经连接到数据库
		bool	isOpen();
		bool	isOpenError();
	private:
		int			m_iState;
		CDbError	m_error;
	};
};

#endif //_PDT_DATABASE_DBDRIVER_H_

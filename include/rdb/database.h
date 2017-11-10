/*==============================================================================
* 文件名称 : database.h
* 模块 : 数据库管理
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : v1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DATABASE_DATABASE_H_
#define	_PDT_DATABASE_DATABASE_H_

#include "rdb/dbdef.h"
#include "rdb/dberror.h"
#include "rdb/sql.h"
#include "rdb/tableinfo.h"
#include "rdb/dbdriver.h"
#include "rdb/dataset.h"



class CDatabasePrivate;

namespace	PDT
{
	class	DB_EXPORT CDatabase
	{
	public:
		virtual	~CDatabase();
	public:
		//打开数据库
		bool	open();
		bool	open(const char *user,const char *pwd);
		//是否已经打开
		bool	isOpen()	const;
		//关闭数据库
		void	close();
		//执行SQL语句
		int		exec(const char *sql);
		int		exec(const char *sql,CDataset &dataset,size_t step=PDT_DB_SELECT_STEP);
		//执行SQL对象（参数及返回值的涵义如上）
		int		exec(const CSql &dbSQL);
		int		exec(const CSql &dbSQL,CDataset &dataset,size_t step=PDT_DB_SELECT_STEP);
		//提交数据集（小于0时认为操作失败）
		int		exec(CDataset &dataset);
		//----------------------------------------------------------------
		//表名列表（返回个数）
		int		tableNames(CStringArray &nameList,DB::TableType tableType=DB::TT_AllTables);
		//表信息
		bool	tableInfo(const char *tableName,CTableInfo &tableInfo);
		//----------------------------------------------------------------
		//错误信息
		const	CDbError	&lastError()	const;
		//----------------------------------------------------------------
		//开始事务
		bool	transaction();
		//提交事务
		bool	commit();
		//回滚事务
		bool	rollback();
		//----------------------------------------------------------------
		//设置服务名
		virtual void	setServerName(const char* srvName);
		//设置数据库名
		virtual	void	setDatabaseName(const char *dbName);
		//设置用户名
		virtual	void	setUserName(const char *user);
		//设置密码
		virtual	void	setPassword(const char *pwd);
		//设置主机名
		virtual	void	setHostName(const char *host);
		//设置端口
		virtual	void	setPort(int port);
		//设置连接选项
		void			setConnectOptions(const char *options=0);
		//获取服务名
		const char *	serverName() const;
		//获取数据库名
		const char *	databaseName()	const;
		//获取用户名
		const char *	userName()	const;
		//获取密码
		const char *	password()	const;
		//获取主机名
		const char *	hostName()	const;
		//获取端口
		int				port()	const;
		//获取驱动
		const CString& 	driverName()	const;
		//构造一个CSql对象，在不需要CSql对象时，调用destroySQL进行销毁
		CSql*			createSQL();

		static const	char*	m_pDefaultConnectStr;
	public:
		//----------------------------------------------------------------
		//获取驱动是否支持某种功能
		static	bool		hasDriverProperty( const char* driverName,CDbDriver::DriverFeatrue featrue);
		//设置驱动某种功能的属性，如：连接池的起始连接数，最大连接数，递增连接数
		static	bool		setDriverProperty( const char* driverName,CDbDriver::DriverFeatrue featrue,void* buf);
		//添加一个数据库到全局（instanceName全局唯一，自己定义）
		static	CDatabase *	addDatabase(const char *driverName,const char *instanceName=m_pDefaultConnectStr);
		static	CDatabase *	addDatabase(CDbDriver *driver,const char *instanceName=m_pDefaultConnectStr);
		//获取数据库对象
		static	CDatabase *	database(const char *instanceName=m_pDefaultConnectStr, bool open = true);
		//删除数据库对象
		static	void		removeDatabase(const char *instanceName= 0 );
		static	void		removeDatabase(CDatabase *db);
		//判断连接名是否存在
		static	bool		isExist(const char *instanceName= m_pDefaultConnectStr);
		//返回实例名列表
		static	int			instances(CStringArray &instanceNameList);
		//----------------------------------------------------------------
	protected:
		CDatabase( const CString& type, const CString& name);
		CDatabase( CDbDriver* driver);
	private:
		void 	init( const CString& type, const CString& name );
		CDatabasePrivate* d;
	};
}

#endif //_PDT_DATABASE_DATABASE_H_

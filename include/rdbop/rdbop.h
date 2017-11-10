/*==============================================================================
* 文件名称 : rdbop.h
* 模块 : 关系库操作接口（多数据库支持）
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 2008年08月08日
==============================================================================*/
#ifndef _PDT_DATABASE_RDBOP_H_
#define _PDT_DATABASE_RDBOP_H_

#include <ace/config-all.h>

//=============================================================================
#if defined (_PDT_RDBOP_)
#  define	RDBOP_EXPORT	ACE_Proper_Export_Flag
#else
#  define	RDBOP_EXPORT	ACE_Proper_Import_Flag
#endif	// _PDT_RDBOP_
//=============================================================================

#include "rdb/rdb"

class CRdbOpPrivate;

namespace PDT
{
	class RDBOP_EXPORT CRdbOp
	{
	public:
		enum ConnectMode { Platform, Direct, PingDirect ,TcpDirect};

		CRdbOp(ConnectMode connMode = Platform , bool useConnPool = false ,int timeout = 1000 );//1000ms
		~CRdbOp();
	public:
		//判断是否在连接状态 (yhh20090827)
		bool	isOpen() const;
		//----------------------------------------------------------------
		//执行不需要返回结果的SQL语句，如（insert,update,delete）
		int		exec(const char *sql);
		//执行需要返回结果集的SQL语句，如（select）
		int		exec(const char *sql,CDataset &dataset);
		//执行不需要返回结果的SQL对象，如（insert,update,delete）
		int		exec(const CSql &dbSQL);
		//执行需要返回结果集的SQL对象，如（select）
		int		exec(const CSql &dbSQL,CDataset &dataset);
		//提交数据集（小于0时认为操作失败）
		int		exec(CDataset &dataset);

		//----------------------------------------------------------------
		//表名列表
		int		tableNames(PDT::CStringArray &nameList,DB::TableType tableType=DB::TT_AllTables);
		//表信息
		bool	tableInfo(const char *tableName,CTableInfo &tableInfo);

		//----------------------------------------------------------------
		//开始事务
		bool	transaction();
		//提交事务
		bool	commit();
		//回滚事务
		bool	rollback();

		//----------------------------------------------------------------
		//构造一个CSql对象，在不需要CSql对象时，调用CSql的destroy()进行销毁
		CSql*	createSQL();

	protected:
		CRdbOpPrivate*	m_pRdbOpPrivate;
	private:
	};
}
#endif	//_PDT_DATABASE_RDBOP_H_




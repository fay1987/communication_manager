/*==============================================================================
* 文件名称 : field.h
* 模块 : 数据记录
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 2007年7月25日
==============================================================================*/
#ifndef	_PDT_DATABASE_DBERROR_H_
#define	_PDT_DATABASE_DBERROR_H_

#include "rdb/dbdef.h"
#include "rdb/db_export.h"
#include "utl/string.h"

//---------------------------------------------------------------------------------------
namespace PDT
{
	class	DB_EXPORT CDbError
	{
	public:
		enum Type
		{
			None,
			Connection,
			Statement,
			Transaction,
			Unknown
		};

		CDbError(  const CString& driverText = "",const CString& databaseText = "",	int type = CDbError::None,int number = -1 );
		CDbError( const CDbError& other );
		CDbError& operator=( const CDbError& other );
		virtual ~CDbError();

		CString		driverText() const;
		void			setDriverText( const CString& driverText );
		CString		databaseText() const;
		void			setDatabaseText( const CString& databaseText );
		int				type() const;
		void			setType( int type );
		int				number() const;
		void			setNumber( int number );
		CString 	text() const;

	private:
		CString	m_strDriverError;
		CString	m_strDatabaseError;
		int			m_iErrorType;
		int			m_iErrorNumber;
	};
};
//---------------------------------------------------------------------------------------

#endif //_PDT_DATABASE_DBERROR_H_

#include "sqlitedriver.h"
#include "sys/blog.h"
#include <QString>

#define	 LOG_SQLITE_BASE		1120	//根据日志分配表，SQLITE基础类型
#define  SIZE_STEP_INCREASE		100		//记录增加步调
using namespace PDT;

CSqliteDriver::CSqliteDriver()
{
	m_pDb = NULL;
}

CSqliteDriver::~CSqliteDriver()
{
	close();
}

//连接数据库
bool CSqliteDriver::open( const CString & db,
		  const CString & srvname,
		  const CString & user,
		  const CString & password,
		  const CString & host, 
		  int       port)
{
	if ( m_pDb != NULL )
		return true;

	int ret = -1;
	
	try
	{
		CString dbFile = ACE_OS::getenv(SYS_ENV_VAR) + CString("/dbscripts/") + db + CString(".db");
		ret = sqlite3_open(dbFile.c_str(),&m_pDb);
		if ( ret == SQLITE_OK )
		{
			logprint(LOG_SQLITE_BASE,"open <%s> sucess!",dbFile.c_str());
			setOpen(true);
			setOpenError(false);
			return true;
		}
		else
		{
			logprint(LOG_SQLITE_BASE,"open <%s> error!",dbFile.c_str());
			setLastError( CDbError( "open database error", "",CDbError::Connection,ret) );
			setOpen(false);
			setOpenError(true);
			return false;
		}
	}
	catch(...)
	{
		setLastError( CDbError( "catch open database error", "",CDbError::Connection,ret) );
		setOpen(false);
		setOpenError(true);
		return false;
	}

	return false;
}

//关闭数据库连接
void CSqliteDriver::close()
{
	try
	{
		if ( m_pDb != NULL) sqlite3_close(m_pDb);
		m_pDb = NULL;
	}
	catch(...)
	{
		
	}
}

//-------------------------------------------------------------------------
//开始事务
bool CSqliteDriver::beginTransaction()
{
	return true;
}

bool CSqliteDriver::commitTransaction()
{
	return true;
}

bool CSqliteDriver::rollbackTransaction()
{
	return true;
}

//-------------------------------------------------------------------------
//表名列表（返回个数）
int CSqliteDriver::tableNames( CStringArray & o_nameList,
								 DB::TableType  i_tableType /*= DB::TT_AllTables*/ )
{
	if ( !isOpen() ) return -1;

	CString sql;
	switch( i_tableType )
	{
	case DB::TT_Tables:
		sql = "select name from sqlite_master where type='table'";
		break;
	case DB::TT_SystemTables:
		break;
	case DB::TT_Views:
		sql = "select name from sqlite_master where type='view'";
		break;
	case DB::TT_AllTables:
		sql = "select name from sqlite_master where type='table'or type = 'view'";
		break;
	default:
		{
			setLastError( CDbError("DB::TableType i_tableType error in tableNames", "", CDbError::Connection));
			return -1;;
		}
	}

	int rowCounter = 0;
	CDataset l_dataset;
	rowCounter = this->exec (sql.c_str(), l_dataset);
	if ( rowCounter <= 0 ) return -1;

	for(int i = 0; i < (int)l_dataset.rowSize(); i++ )
	{
		for(int j = 0; j < (int)l_dataset.colSize(); j++ )
		{
			o_nameList.append (l_dataset[i][j].value().toString());
		}
	}
	return rowCounter;
}

//表信息
bool CSqliteDriver::tableInfo( const char       * i_tableName,
								 CTableInfo & o_tableInfo )
{
	if( !isOpen() )
		return false;
	const char* ptablename = ACE_OS::strstr(i_tableName, ".");//若包含模式.表名
	if (ptablename)
		ptablename++;//跳过 . 
	else
		ptablename = i_tableName;

	o_tableInfo.setTableName(i_tableName);

	CString sql;
	sql = CString("PRAGMA table_info(") + CString(i_tableName) + CString(")");

	int rowCounter = 0;
	CDataset l_dataset;
	rowCounter = this->exec (sql.c_str(), l_dataset);
	if ( rowCounter == -1 ) return false;

	CFieldInfos& fieldInfos = o_tableInfo.fieldInfos();
	size_t fieldCnt = rowCounter;

	fieldInfos.resize( fieldCnt,CFieldInfo::null );

	for(int i = 0; i < (int)l_dataset.rowSize(); i++ )
	{
		CFieldInfo& fi = fieldInfos[i];

		fi.setName( l_dataset[i][1].value().toString() );

		CVariant::Type tt =  type(l_dataset[i][2].value().toString().c_str());
		

		fi.setType( tt );		//字符串信息
		fi.setSize(  l_dataset[i][3].value().toInt32() );			//无法获取，可从type字符串间接获取
		fi.setPrimaryKey(  l_dataset[i][5].value().toInt8() );
		///
		//logprint(9999,"name:%s type:%s size:%d primary:%d",fi.name().c_str(),l_dataset[i][2].value().toString().c_str(),fi.size(),fi.isPrimaryKey());
		///
	}
	o_tableInfo.initFieldIndexs();
	return true;
}

//SQL信息
bool CSqliteDriver::sqlInfo(const char *sql,CTableInfo &tableInfo)
{
	return false;
}

//执行SQL语句
int CSqliteDriver::exec( const char * i_sql )
{	
	logprint(LOG_SQLITE_BASE,"exec <%s>",i_sql);
	if (!i_sql  )
		return 0;

	QString str_s = i_sql;
	CString str = str_s.toUtf8().data();

	if ( !isOpen() )
		return 0;

	int ret = -1;
	ret = sqlite3_exec(m_pDb,str.c_str(),NULL,NULL,NULL);
	if ( ret == SQLITE_OK )
		return 1;
	else 
		setLastError( CDbError(sqlite3_errmsg(m_pDb), "", CDbError::Statement));

	return 0;
}

int CSqliteDriver::exec( const char * i_sql, CDataset & o_dataset ,size_t step /*=PDT_DB_SELECT_STEP*/ )
{
	logprint(LOG_SQLITE_BASE,"exec <%s>",i_sql);	
	if ( !i_sql )
		return -1;

	QString str_s = i_sql;
	CString str = str_s.toUtf8().data();

	 sqlite3_stmt *stmt = NULL;
	 int ret = -1;
	 ret = sqlite3_prepare(m_pDb,str.c_str(),-1,&stmt,0);
	 if ( ret != SQLITE_OK ) 
	 {
		 logprint(LOG_SQLITE_BASE,"sqlite3_prepare error<%s>",sqlite3_errmsg(m_pDb));
		 return -1;
	 }
	 
	 ret = sqlite3_step(stmt);
	 if ( ret == SQLITE_DONE ) return 0;
	 else if ( ret != SQLITE_ROW ) 
	 {
		 logprint(LOG_SQLITE_BASE,"sqlite3_step error<%s>",sqlite3_errmsg(m_pDb));
		 return -1;
	 }
	 int ncols = sqlite3_column_count(stmt);
	 setTableInfo(o_dataset.tableInfo(),stmt);
	 
	 int type = -1;
	 CFieldInfos& fieldInfos = o_dataset.tableInfo().fieldInfos();
	 int stepSize = SIZE_STEP_INCREASE;		//wfp add
	 o_dataset.reSize( stepSize,ncols );
	 int nrows = 0;
	 while ( ret == SQLITE_ROW ) 
	 {
		 if ( nrows >= stepSize )
		 {
			 stepSize *= 2;
			 o_dataset.reSize(stepSize,ncols);
		 }

		 for (int i=0;i<ncols;i++)
		 {
			CField& field = o_dataset[nrows][i];
			setFieldValue(field,fieldInfos[i],stmt,i);
		 }//end for

		 ++nrows;
		 ret = sqlite3_step(stmt);
	 }//end while

	 o_dataset.reSize(nrows,ncols);
	 sqlite3_finalize(stmt);
	 return nrows;
}

//批量提交（true--全部提交成功,否则返回false）
bool CSqliteDriver::exec(CDataset & dataset)
{
	logprint(LOG_SQLITE_BASE,"批量exec");

	return true;
}

//-------------------------------------------------------------------------
//判断是否已经连接到数据库
bool	CSqliteDriver::isOpen()
{
	return (m_pDb != NULL);
}

CVariant::Type CSqliteDriver::type(int type)
{
	if ( type == SQLITE_NULL )
		return CVariant::Null;
	else if ( type == SQLITE_INTEGER )
		return CVariant::Int32;
	else if ( type == SQLITE_FLOAT )
		return CVariant::Float;
	else if ( type == SQLITE_TEXT )	//wfp add
		return CVariant::String;
	else if ( type == SQLITE_BLOB )
		return CVariant::ByteArray;
	else
		return CVariant::Null;
}

CVariant::Type CSqliteDriver::type(const char* typeName)
{
	if ( typeName == NULL ) return CVariant::Null;

	CString typeStr = typeName;
	if ( typeStr.caseFind("char") >= 0 )
	{
		return CVariant::String;
	}
	else if ( typeStr.caseFind("int")>= 0 )
		return CVariant::Int32;
	else if ( typeStr.caseFind("double")>= 0 )
		return CVariant::Double;
	else if ( typeStr.caseFind("float")>= 0 )
		return CVariant::Float;
	else if ( typeStr.caseFind("text") >= 0)
		return CVariant::String;
	else if ( typeStr.caseFind("datetime")>= 0 )
		return CVariant::DateTime;
	else if ( typeStr.caseFind("date") >= 0)
		return CVariant::Date;
	else if ( typeStr.caseFind("time") >= 0)
		return CVariant::Time;
	else if ( typeStr.caseFind("bolb") >= 0)
		return CVariant::ByteArray;
	else
		return CVariant::Null;
}

void CSqliteDriver::setTableInfo(CTableInfo& tableInfo,sqlite3_stmt *stmt)
{
	if ( stmt == NULL ) return;
	QString str_s;
	CString strTableName;

#ifdef WIN32
	str_s = QString::fromUtf8((const char *)sqlite3_column_table_name(stmt,0));
	strTableName = str_s.toLocal8Bit().data();
#else
	str_s = QString((const char *)sqlite3_column_table_name(stmt,0));
	strTableName = str_s.toStdString().c_str();
#endif



	tableInfo.setTableName( strTableName );

	CFieldInfos& fieldInfos = tableInfo.fieldInfos();
	size_t fieldCnt = sqlite3_column_count(stmt);

	fieldInfos.resize( fieldCnt,CFieldInfo::null );

	for (hUInt32 i=0;i<fieldCnt;i++ )
	{
		CFieldInfo& fi = fieldInfos[i];

		

		CString strFieldName;

#ifdef WIN32
		str_s = QString::fromUtf8((const char *)sqlite3_column_name(stmt,i));
		strFieldName = str_s.toLocal8Bit().data();
#else
		str_s = QString((const char *)sqlite3_column_name(stmt,i));
		strFieldName = str_s.toStdString().c_str();
#endif

		fi.setName( strFieldName );
		

		char const *sType = NULL;
		char const *sColl = NULL;

		int bNotNull;
		int bPrimaryKey;
		int bAutoinc;

		sqlite3_table_column_metadata(m_pDb, NULL, strTableName.c_str(),
			strFieldName.c_str(), &sType, &sColl, &bNotNull, &bPrimaryKey, &bAutoinc );
		
		fi.setType( type(sType) );
		fi.setPrimaryKey(bPrimaryKey);
		fi.setRequired(bNotNull);



		fi.setSize( 1 );	//wfp_test
		//fi.setPrimaryKey( field.primary_key() );
		//CString str = (const char*)sqlite3_column_decltype(stmt,i);
		//logprint(9999,"%d__type=%d__%s",i,sqlite3_column_type(stmt,i),str.c_str());	//wfp_test
	}

	tableInfo.initFieldIndexs(); //wfp20091231
}

void CSqliteDriver::setFieldValue(CField& field,const CFieldInfo& fieldInfo,sqlite3_stmt *stmt,int colIndex)
{
	if ( stmt == NULL ) return;

//	sqlite3_column_type(stmt,colIndex);


	switch ( fieldInfo.type() )
	{
	case CVariant::Int8:
		field.value( sqlite3_column_int(stmt,colIndex) );
		break;
	case CVariant::UInt8:
		field.value( sqlite3_column_int(stmt,colIndex) );
		break;
	case CVariant::Int16:
		field.value( sqlite3_column_int(stmt,colIndex) );
		break;
	case CVariant::UInt16:
		field.value( sqlite3_column_int(stmt,colIndex) );
		break;
	case CVariant::Int32:
		field.value( sqlite3_column_int(stmt,colIndex) );
		break;
	case CVariant::UInt32:
		field.value( sqlite3_column_int(stmt,colIndex) );
		break;
	case CVariant::Int64:
		field.value( sqlite3_column_int64(stmt,colIndex) );
		break;
	case CVariant::UInt64:
		field.value( sqlite3_column_int64(stmt,colIndex) );
		break;
	case CVariant::Float:
		field.value( sqlite3_column_double(stmt,colIndex) );
		break;
	case CVariant::Double:
		field.value( sqlite3_column_double(stmt,colIndex) );
		break;
	case CVariant::Date:
		{
			CString dateString = (const char*)sqlite3_column_text16(stmt,colIndex);
			dateString += " 00:00:00";
			CDateTime dt;
			dt.fromString( dateString,DT_FMT_DEF );
			field.value( dt );
		}
		break;
	case CVariant::Time:
		{
			CString timeString = CString("1970-01-01 ") + CString((const char*)sqlite3_column_text16(stmt,colIndex));
			CDateTime dt;
			dt.fromString( timeString,DT_FMT_DEF );
			field.value( dt );
		}
		break;
	case CVariant::DateTime:
		{
			CDateTime dt;
			dt.fromString( CString((const char*)sqlite3_column_text16(stmt,colIndex)),DT_FMT_DEF );
			field.value( dt );
		}
		break;
	case CVariant::Null:
	case CVariant::String:
		{
			//CString str = ACE_Wide_To_Ascii::convert( (const wchar_t *)sqlite3_column_text16(stmt,colIndex) );	//wfp_test
			//field.value(ACE_Wide_To_Ascii::convert( (const wchar_t *)sqlite3_column_text16(stmt,colIndex) ));	

#ifdef WIN32
			QString str_s =QString::fromUtf8((const char *)sqlite3_column_text(stmt,colIndex));
			CString str = str_s.toLocal8Bit().data();
#else
			QString str_s = QString((const char *)sqlite3_column_text(stmt,colIndex));
			CString str = str_s.toStdString().c_str();
#endif
			

			//CString str = (const char *)sqlite3_column_text(stmt,colIndex);
			field.value(str);	
		}
		break;
	default:
		field.value( CVariant::null );
		break;
	}
}

bool CSqliteDriver::insertDataset(CDataset& ds)
{
	return false;
}

bool CSqliteDriver::updateDataset(CDataset& ds)
{
	return false;
}

bool CSqliteDriver::deleteDataset(CDataset& ds)
{
	return false;
}

bool CSqliteDriver::handleDataset(CDataset& ds)
{
	return false;
}


//****实现标准化的返回驱动对象的函数***
extern "C" DRIVER_EXPORT PDT::CSqliteDriver * CreateDriver()
{
	return ( new CSqliteDriver() );
}

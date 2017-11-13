/*==============================================================================
* �ļ����� : sqlitedriver.h
* ģ�� : SQLITE ���ݿ�����
* ����ʱ�� : 2011-03-28 15:55:23
* ���� : wlq
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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
		//�������ݿ�
		bool open( const CString & db,
			const CString & srvname,
			const CString & user     = "",
			const CString & password = "",
			const CString & host     = "", 
			int       port     = -1 );
		//�ر����ݿ�����
		void close();
		//-------------------------------------------------------------------------
		//��ʼ����
		bool beginTransaction();
		bool commitTransaction();
		bool rollbackTransaction();
		//-------------------------------------------------------------------------
		//�����б����ظ�����
		int tableNames( CStringArray & o_nameList,
			DB::TableType  i_tableType = DB::TT_AllTables );
		//����Ϣ
		bool tableInfo( const char       * i_tableName,
			CTableInfo & o_tableInfo );
		//SQL��Ϣ
		bool	sqlInfo(const char *sql,CTableInfo &tableInfo);
		//ִ��SQL���
		int exec( const char * i_sql );
		int exec( const char * i_sql, CDataset & o_dataset ,size_t step=PDT_DB_SELECT_STEP);
		//�����ύ��true--ȫ���ύ�ɹ�,���򷵻�false��
		bool exec(CDataset & dataset);
		//-------------------------------------------------------------------------
		//���ص�ǰ���ݿ�Ĭ�ϵ�NULL���ַ���
		CString nullText() const { return "NULL"; }

		//�ж��Ƿ��Ѿ����ӵ����ݿ�
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

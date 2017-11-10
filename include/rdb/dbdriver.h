/*==============================================================================
* �ļ����� : dbdriver.h
* ģ�� : ���ݿ���������
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 2007��7��25��
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

		//��ȡ����֧�ֵĹ��ܣ�Ӧ�ھ����driver���þ�̬����ʵ��
		//virtual	bool hasDriverProperty( DriverFeatrue featrue);
		//��������ĳ�ֹ��ܵ����ԣ��磺���ӳص���ʼ�������������������������������Ӧ�ھ����driver���þ�̬����ʵ��
		//virtual bool setDriverProperty( DriverFeatrue featrue,void* buf);

		//�������ݿ⣬���麯�������������ʵ��
		virtual bool open ( const CString & db , const CString& srvname,const CString & user = "", const CString & password = "", const CString & host = "", int port = -1 ) = 0;
		//�ر����ݿ����ӣ����麯�������������ʵ��
		virtual void close () = 0;
		//-------------------------------------------------------------------------
		//��ʼ����
		virtual bool beginTransaction();
		virtual bool commitTransaction();
		virtual bool rollbackTransaction();
		//-------------------------------------------------------------------------
		//�����б����ظ�����
		virtual int		tableNames(CStringArray &nameList,DB::TableType tableType=DB::TT_AllTables) = 0;
		//����Ϣ
		virtual bool	tableInfo(const char *tableName,CTableInfo &tableInfo) = 0;
		virtual bool	sqlInfo(const char *sql,CTableInfo &tableInfo) = 0;
		//-------------------------------------------------------------------------
		//������Ϣ
		virtual	const	CDbError	&lastError()	const;
		//-------------------------------------------------------------------------
		//ִ��SQL���
		virtual int		exec(const char *sql) = 0;
		virtual int		exec(const char *sql,CDataset &dataset,size_t step=PDT_DB_SELECT_STEP) = 0;
		//�ύ���ݼ�
		virtual bool    exec(CDataset & dataset);
		//-------------------------------------------------------------------------
		//���ص�ǰ���ݿ�Ĭ�ϵ�NULL���ַ���
		virtual CString nullText() const;
	protected:
		virtual void setOpen ( bool o );
		virtual void setOpenError ( bool e );
		virtual void setLastError ( const CDbError & e );
	public:
		//�������ݿ�
		bool	open ( const CString & db, const CString& srvname,const CString & user, const CString & password, const CString & host, int port, const CString & connOpts );
		//�ж��Ƿ��Ѿ����ӵ����ݿ�
		bool	isOpen();
		bool	isOpenError();
	private:
		int			m_iState;
		CDbError	m_error;
	};
};

#endif //_PDT_DATABASE_DBDRIVER_H_

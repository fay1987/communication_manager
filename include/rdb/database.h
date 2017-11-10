/*==============================================================================
* �ļ����� : database.h
* ģ�� : ���ݿ����
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : v1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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
		//�����ݿ�
		bool	open();
		bool	open(const char *user,const char *pwd);
		//�Ƿ��Ѿ���
		bool	isOpen()	const;
		//�ر����ݿ�
		void	close();
		//ִ��SQL���
		int		exec(const char *sql);
		int		exec(const char *sql,CDataset &dataset,size_t step=PDT_DB_SELECT_STEP);
		//ִ��SQL���󣨲���������ֵ�ĺ������ϣ�
		int		exec(const CSql &dbSQL);
		int		exec(const CSql &dbSQL,CDataset &dataset,size_t step=PDT_DB_SELECT_STEP);
		//�ύ���ݼ���С��0ʱ��Ϊ����ʧ�ܣ�
		int		exec(CDataset &dataset);
		//----------------------------------------------------------------
		//�����б����ظ�����
		int		tableNames(CStringArray &nameList,DB::TableType tableType=DB::TT_AllTables);
		//����Ϣ
		bool	tableInfo(const char *tableName,CTableInfo &tableInfo);
		//----------------------------------------------------------------
		//������Ϣ
		const	CDbError	&lastError()	const;
		//----------------------------------------------------------------
		//��ʼ����
		bool	transaction();
		//�ύ����
		bool	commit();
		//�ع�����
		bool	rollback();
		//----------------------------------------------------------------
		//���÷�����
		virtual void	setServerName(const char* srvName);
		//�������ݿ���
		virtual	void	setDatabaseName(const char *dbName);
		//�����û���
		virtual	void	setUserName(const char *user);
		//��������
		virtual	void	setPassword(const char *pwd);
		//����������
		virtual	void	setHostName(const char *host);
		//���ö˿�
		virtual	void	setPort(int port);
		//��������ѡ��
		void			setConnectOptions(const char *options=0);
		//��ȡ������
		const char *	serverName() const;
		//��ȡ���ݿ���
		const char *	databaseName()	const;
		//��ȡ�û���
		const char *	userName()	const;
		//��ȡ����
		const char *	password()	const;
		//��ȡ������
		const char *	hostName()	const;
		//��ȡ�˿�
		int				port()	const;
		//��ȡ����
		const CString& 	driverName()	const;
		//����һ��CSql�����ڲ���ҪCSql����ʱ������destroySQL��������
		CSql*			createSQL();

		static const	char*	m_pDefaultConnectStr;
	public:
		//----------------------------------------------------------------
		//��ȡ�����Ƿ�֧��ĳ�ֹ���
		static	bool		hasDriverProperty( const char* driverName,CDbDriver::DriverFeatrue featrue);
		//��������ĳ�ֹ��ܵ����ԣ��磺���ӳص���ʼ�����������������������������
		static	bool		setDriverProperty( const char* driverName,CDbDriver::DriverFeatrue featrue,void* buf);
		//���һ�����ݿ⵽ȫ�֣�instanceNameȫ��Ψһ���Լ����壩
		static	CDatabase *	addDatabase(const char *driverName,const char *instanceName=m_pDefaultConnectStr);
		static	CDatabase *	addDatabase(CDbDriver *driver,const char *instanceName=m_pDefaultConnectStr);
		//��ȡ���ݿ����
		static	CDatabase *	database(const char *instanceName=m_pDefaultConnectStr, bool open = true);
		//ɾ�����ݿ����
		static	void		removeDatabase(const char *instanceName= 0 );
		static	void		removeDatabase(CDatabase *db);
		//�ж��������Ƿ����
		static	bool		isExist(const char *instanceName= m_pDefaultConnectStr);
		//����ʵ�����б�
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

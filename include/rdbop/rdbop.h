/*==============================================================================
* �ļ����� : rdbop.h
* ģ�� : ��ϵ������ӿڣ������ݿ�֧�֣�
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 2008��08��08��
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
		//�ж��Ƿ�������״̬ (yhh20090827)
		bool	isOpen() const;
		//----------------------------------------------------------------
		//ִ�в���Ҫ���ؽ����SQL��䣬�磨insert,update,delete��
		int		exec(const char *sql);
		//ִ����Ҫ���ؽ������SQL��䣬�磨select��
		int		exec(const char *sql,CDataset &dataset);
		//ִ�в���Ҫ���ؽ����SQL�����磨insert,update,delete��
		int		exec(const CSql &dbSQL);
		//ִ����Ҫ���ؽ������SQL�����磨select��
		int		exec(const CSql &dbSQL,CDataset &dataset);
		//�ύ���ݼ���С��0ʱ��Ϊ����ʧ�ܣ�
		int		exec(CDataset &dataset);

		//----------------------------------------------------------------
		//�����б�
		int		tableNames(PDT::CStringArray &nameList,DB::TableType tableType=DB::TT_AllTables);
		//����Ϣ
		bool	tableInfo(const char *tableName,CTableInfo &tableInfo);

		//----------------------------------------------------------------
		//��ʼ����
		bool	transaction();
		//�ύ����
		bool	commit();
		//�ع�����
		bool	rollback();

		//----------------------------------------------------------------
		//����һ��CSql�����ڲ���ҪCSql����ʱ������CSql��destroy()��������
		CSql*	createSQL();

	protected:
		CRdbOpPrivate*	m_pRdbOpPrivate;
	private:
	};
}
#endif	//_PDT_DATABASE_RDBOP_H_




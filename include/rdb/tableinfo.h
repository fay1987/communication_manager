/*==============================================================================
* �ļ����� : tableinfo.h
* ģ�� : ���ݿ��Ԫ��Ϣ
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : v1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_PDT_DATABASE_TABLEINFO_H_
#define	_PDT_DATABASE_TABLEINFO_H_

#include "rdb/db_export.h"
#include "rdb/fieldinfo.h"
#include "utl/stringarray.h"
#include "ace/Map_Manager.h"	//yhh20091110
#include "ace/Synch.h"			//yhh20091110

namespace PDT
{
	class	DB_EXPORT	CTableInfo
	{
	public:
		CTableInfo();
		CTableInfo(const CFieldInfos& fieldInfos);
		virtual	~CTableInfo();

		static CTableInfo	null;
	public:
		//������Ϣ
		const	CString		&tableName()	const;
		void				setTableName(const CString &tableName);
		//��������CFieldInfo��ACE_Vector
		const CFieldInfos&	fieldInfos() const;
		CFieldInfos&		fieldInfos();
		//����ָ����CFieldInfo����
		const CFieldInfo&			fieldInfo(int idx) const;
		CFieldInfo&					fieldInfo(int idx);
		const CFieldInfo&			fieldInfo(const CString& fieldName) const;
		CFieldInfo&					fieldInfo(const CString& fieldName);
		const CFieldInfo&			operator[](int idx) const;
		CFieldInfo&					operator[](int idx);
		//����ָ�����ֵ�CFieldInfo�����λ��
		int							fieldIndex(const CString& fieldName) const;	

		//yhh20091110---------------------------------------------------------begin
		//�������캯��
		CTableInfo(const CTableInfo& other){*this = other;}				
		CTableInfo&		operator=(const CTableInfo& other);
		void			initFieldIndexs();
		//--------------------------------------------------------------------end
	protected:
		CString			m_tableName;
		CFieldInfos		m_fieldInfos;
		//yhh20091110---------------------------------------------------------begin
		typedef ACE_Map_Manager<CString,int,ACE_SYNCH_RW_MUTEX>		FieldIndexs;
		FieldIndexs		m_fieldIndexs;
		bool			m_hasInitIndexs;
		//--------------------------------------------------------------------end
	};	
};

#endif //_PDT_DATABASE_TABLEINFO_H_

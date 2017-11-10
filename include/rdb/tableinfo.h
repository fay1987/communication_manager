/*==============================================================================
* 文件名称 : tableinfo.h
* 模块 : 数据库表元信息
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : v1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
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
		//表名信息
		const	CString		&tableName()	const;
		void				setTableName(const CString &tableName);
		//返回所有CFieldInfo的ACE_Vector
		const CFieldInfos&	fieldInfos() const;
		CFieldInfos&		fieldInfos();
		//返回指定的CFieldInfo对象
		const CFieldInfo&			fieldInfo(int idx) const;
		CFieldInfo&					fieldInfo(int idx);
		const CFieldInfo&			fieldInfo(const CString& fieldName) const;
		CFieldInfo&					fieldInfo(const CString& fieldName);
		const CFieldInfo&			operator[](int idx) const;
		CFieldInfo&					operator[](int idx);
		//返回指定名字的CFieldInfo对象的位置
		int							fieldIndex(const CString& fieldName) const;	

		//yhh20091110---------------------------------------------------------begin
		//拷贝构造函数
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

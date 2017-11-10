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
#ifndef	_PDT_DATABASE_RECORD_H_
#define	_PDT_DATABASE_RECORD_H_

#include "rdb/db_export.h"
#include "rdb/dbdef.h"
#include "field.h"
//---------------------------------------------------------------------------------------
namespace PDT
{
	//数据记录
	class	DB_EXPORT	CRecord
	{
	public:
		//数据标志
		enum	DataFlag
		{
			Null = 0,		//空
			Normal,			//正常数据（Normal状态不按此返回）
			Insert,			//需要insert to 数据库
			Delete		    //需要delete from 数据库
		};
	public:
		CRecord();
		CRecord(size_t size);
		CRecord(const CRecord &record);
		virtual	~CRecord();
	public:
		//获取大小
		size_t				size()	const;
		//重新分配大小
		void				resize(size_t size);
		//返回记录状态
		//DB::RecordFlag		dataFlag()	const;		//不返回“Normal、Update”状态
		bool				isNull()	const;
		bool				isNormal()	const;		//非“Insert、Delete、Update”时为Normal
		bool				isInsert()	const;
		bool				isDelete()	const;
		bool				isUpdate()	const;		//有一个及一个以上数据域为Update时为Update
		//设置字段状态
		void				setFlagNone();
		void				setFlagNormal();
		void				setFlagInsert();
		void				setFlagDelete();
		//获取可修改字段
		CField				&field(size_t col);
		CField				&operator[](size_t col);
		CFields				&fields();
		//获取只读字段
		const	CField		&field(size_t col) const;
		const	CField		&operator[](size_t col)	const;
		const	CFields		&fields()	const;
	public:
		CRecord				&operator=(const CRecord &record);
		bool				operator==(const CRecord &record)	const;
		bool				operator!=(const CRecord &record)	const;
	public:
		static	CRecord		null;			//空对象
	private:
		CFields				m_fields;
		DataFlag			m_dataFlag;
	};
	//数据记录数组
	typedef	ACE_Vector<CRecord>	CRecords;
}

#endif

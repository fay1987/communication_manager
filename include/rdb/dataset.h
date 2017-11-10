/*==============================================================================
* 文件名称 : field.h
* 模块 : 数据集
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 2007年7月25日
==============================================================================*/
#ifndef	_PDT_DATABASE_DATASET_H_
#define	_PDT_DATABASE_DATASET_H_

#include "rdb/db_export.h"
#include "rdb/record.h"
#include "rdb/tableinfo.h"
//---------------------------------------------------------------------------------------
namespace PDT
{
	class	DB_EXPORT	CDataset
	{
	public:
		CDataset();
		CDataset(size_t rowSize,size_t colSize);
		CDataset(const CDataset &dataset);
		virtual	~CDataset();
	public:
		//字段个数
		size_t				colSize()	const;
		//记录个数
		size_t				rowSize()	const;
		//设置大小
		void				reSize(size_t rowSize,size_t colSize);
		//可写表信息
		CTableInfo			&tableInfo();
		//只读表信息
		const	CTableInfo	&tableInfo()	const;
		//可写字段信息
		CFieldInfo			&fieldInfo(size_t col);
		CFieldInfo			&fieldInfo(const CString &fieldName);
		CFieldInfos			&fieldInfos();
		//只读字段信息
		const	CFieldInfo	&fieldInfo(size_t col)	const;
		const	CFieldInfo	&fieldInfo(const CString &fieldName)	const;
		const	CFieldInfos	&fieldInfos()	const;
		//返回数据集是否具有某种状态
		bool				hasNull() const;
		bool				hasNormal() const;
		bool				hasInsert() const;
		bool				hasDelete() const;
		bool				hasUpdate() const;
		//返回数据集中所有记录都具有某种状态
		bool				isAllNull() const;
		bool				isAllNormal() const;
		bool				isAllInsert() const;
		bool				isAllDelete() const;
		bool				isAllUpdate() const;
		//设置数据集状态
		void				setFlagNormal();
		void				setFlagInsert();
		void				setFlagDelete();
		void				setFlagUpdate();
		//获取可写记录
		CRecord				&record(size_t row);
		CRecord				&operator[](size_t row);
		CRecords			&records();
		//获取只读记录
		const	CRecord		&record(size_t row) const;
		const	CRecord		&operator[](size_t row) const;
		const	CRecords	&records()	const;
		//获取可写数据域
		CField				&field(size_t row,size_t col);
		CField				&field(size_t row,const CString &fieldName);
		//获取只读数据域
		const	CField		&field(size_t row,size_t col)	const;
		const	CField		&field(size_t row,const CString &fieldName)	const;
		//增加记录（rowNum：增加的行数，colNum：增加的列数）
		void				addRecords(size_t rowNum=1,size_t colNum=0);
	public:
		CDataset			&operator=(const CDataset &dataset);
		bool				operator==(const CDataset &dataset)	const;
		bool				operator!=(const CDataset &dataset)	const;
	private:
		CRecords			m_records;
		CTableInfo			m_tableInfo;
	};
}
//---------------------------------------------------------------------------------------

#endif

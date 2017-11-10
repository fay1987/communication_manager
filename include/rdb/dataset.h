/*==============================================================================
* �ļ����� : field.h
* ģ�� : ���ݼ�
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 2007��7��25��
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
		//�ֶθ���
		size_t				colSize()	const;
		//��¼����
		size_t				rowSize()	const;
		//���ô�С
		void				reSize(size_t rowSize,size_t colSize);
		//��д����Ϣ
		CTableInfo			&tableInfo();
		//ֻ������Ϣ
		const	CTableInfo	&tableInfo()	const;
		//��д�ֶ���Ϣ
		CFieldInfo			&fieldInfo(size_t col);
		CFieldInfo			&fieldInfo(const CString &fieldName);
		CFieldInfos			&fieldInfos();
		//ֻ���ֶ���Ϣ
		const	CFieldInfo	&fieldInfo(size_t col)	const;
		const	CFieldInfo	&fieldInfo(const CString &fieldName)	const;
		const	CFieldInfos	&fieldInfos()	const;
		//�������ݼ��Ƿ����ĳ��״̬
		bool				hasNull() const;
		bool				hasNormal() const;
		bool				hasInsert() const;
		bool				hasDelete() const;
		bool				hasUpdate() const;
		//�������ݼ������м�¼������ĳ��״̬
		bool				isAllNull() const;
		bool				isAllNormal() const;
		bool				isAllInsert() const;
		bool				isAllDelete() const;
		bool				isAllUpdate() const;
		//�������ݼ�״̬
		void				setFlagNormal();
		void				setFlagInsert();
		void				setFlagDelete();
		void				setFlagUpdate();
		//��ȡ��д��¼
		CRecord				&record(size_t row);
		CRecord				&operator[](size_t row);
		CRecords			&records();
		//��ȡֻ����¼
		const	CRecord		&record(size_t row) const;
		const	CRecord		&operator[](size_t row) const;
		const	CRecords	&records()	const;
		//��ȡ��д������
		CField				&field(size_t row,size_t col);
		CField				&field(size_t row,const CString &fieldName);
		//��ȡֻ��������
		const	CField		&field(size_t row,size_t col)	const;
		const	CField		&field(size_t row,const CString &fieldName)	const;
		//���Ӽ�¼��rowNum�����ӵ�������colNum�����ӵ�������
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

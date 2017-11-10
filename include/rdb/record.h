/*==============================================================================
* �ļ����� : field.h
* ģ�� : ���ݼ�¼
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : ����ƽ
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 2007��7��25��
==============================================================================*/
#ifndef	_PDT_DATABASE_RECORD_H_
#define	_PDT_DATABASE_RECORD_H_

#include "rdb/db_export.h"
#include "rdb/dbdef.h"
#include "field.h"
//---------------------------------------------------------------------------------------
namespace PDT
{
	//���ݼ�¼
	class	DB_EXPORT	CRecord
	{
	public:
		//���ݱ�־
		enum	DataFlag
		{
			Null = 0,		//��
			Normal,			//�������ݣ�Normal״̬�����˷��أ�
			Insert,			//��Ҫinsert to ���ݿ�
			Delete		    //��Ҫdelete from ���ݿ�
		};
	public:
		CRecord();
		CRecord(size_t size);
		CRecord(const CRecord &record);
		virtual	~CRecord();
	public:
		//��ȡ��С
		size_t				size()	const;
		//���·����С
		void				resize(size_t size);
		//���ؼ�¼״̬
		//DB::RecordFlag		dataFlag()	const;		//�����ء�Normal��Update��״̬
		bool				isNull()	const;
		bool				isNormal()	const;		//�ǡ�Insert��Delete��Update��ʱΪNormal
		bool				isInsert()	const;
		bool				isDelete()	const;
		bool				isUpdate()	const;		//��һ����һ������������ΪUpdateʱΪUpdate
		//�����ֶ�״̬
		void				setFlagNone();
		void				setFlagNormal();
		void				setFlagInsert();
		void				setFlagDelete();
		//��ȡ���޸��ֶ�
		CField				&field(size_t col);
		CField				&operator[](size_t col);
		CFields				&fields();
		//��ȡֻ���ֶ�
		const	CField		&field(size_t col) const;
		const	CField		&operator[](size_t col)	const;
		const	CFields		&fields()	const;
	public:
		CRecord				&operator=(const CRecord &record);
		bool				operator==(const CRecord &record)	const;
		bool				operator!=(const CRecord &record)	const;
	public:
		static	CRecord		null;			//�ն���
	private:
		CFields				m_fields;
		DataFlag			m_dataFlag;
	};
	//���ݼ�¼����
	typedef	ACE_Vector<CRecord>	CRecords;
}

#endif

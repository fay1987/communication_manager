/*==============================================================================
* �ļ����� : field.h
* ģ�� : ������
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : ����ƽ
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 2007��7��25��
==============================================================================*/
#ifndef	_PDT_DATABASE_FIELD_H_
#define	_PDT_DATABASE_FIELD_H_

#include <ace/Vector_T.h>
#include "rdb/db_export.h"
#include "rdb/dbdef.h"
#include "utl/variant.h"
#include "utl/utl.h"
//---------------------------------------------------------------------------------------
namespace PDT
{
	//������
	class	DB_EXPORT	CField
	{
	public:
		//���ݱ�־
		enum	DataFlag
		{
			Null = 0,		//��
			Normal,			//��������
			Update,		    //��Ҫupdate to ���ݿ�
		};
	public:
		CField();
		CField(const CField &field);
		virtual	~CField();
	public:
		//�����ֶ�״̬
		bool				isNull()	const;
		bool				isNormal() const;
		bool				isUpdate() const;
		//�����ֶ�״̬
		void				setFlagNone();
		void				setFlagNormal();
		void				setFlagUpdate();
		//ȡֵ
		CVariant			&value() ;
		const CVariant		&value() const;
		//��ֵ
		const CVariant		&value(const CVariant &value);
	public:
		CField				&operator=(const CField &field);
		bool				operator==(const CField &field)	const;
		bool				operator!=(const CField &field)	const;
	public:
		static	CField		null;			//�ն���
	private:
		CVariant			m_value;		//ֵ
		DataFlag			m_dataFlag;		//��־
	};
	//����������
	typedef	ACE_Vector<CField>	CFields;
}

#endif

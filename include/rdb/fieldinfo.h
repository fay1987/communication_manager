/*==============================================================================
* �ļ����� : fieldinfo.h
* ģ�� : ���ݿ���ֶ�Ԫ��Ϣ
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : v1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� :
==============================================================================*/
#ifndef	_PDT_DATABASE_FIELDINFO_H_
#define	_PDT_DATABASE_FIELDINFO_H_

#include "rdb/db_export.h"
#include "utl/variant.h"
#include "ace/String_Base.h"
#include "ace/Vector_T.h"

namespace PDT
{
	class	DB_EXPORT	CFieldInfo
	{
	public:
		//���캯��
		CFieldInfo();
		CFieldInfo( const CFieldInfo & other );
		virtual	~CFieldInfo();
		CFieldInfo &	operator=( const CFieldInfo & other );
		bool			operator==( const CFieldInfo & f ) const;

		static	CFieldInfo		null;
	public:
		//�ֶ�����
		const CString&	name() const;
		void			setName(const CString& name);
		//��������
		CVariant::Type	type() const;
		void			setType(CVariant::Type type);
		//���ݾ���
		int				precision() const;
		void			setPrecision(int precision);
		//С����λ��
		int				scale() const;
		void			setScale(int scale);
		//�����ֽڳ���(���ݿ��еĴ洢����)
		int				size() const;
		void			setSize(int size);
		//Ĭ��ֵ
		CVariant		defaultValue() const;
		void			setDefaultValue(const CVariant& defaultValue);
		//����
		bool			isPrimaryKey()	const;
		void			setPrimaryKey(bool primaryKey);
		//������ַ������ͣ��Ƿ�����ض�β���ո�
		bool			isTrim() const;
		void			setTrim ( bool trim );
		//�Ƿ�����Ϊ��(NULL)
		bool			isRequired() const;
		void			setRequired(bool required);
		//�Ƿ��Ǽ����ֶ�
		bool			isCalculated() const;
		void			setCalculated( bool calc );
	protected:
		CString			m_strName;
		CVariant::Type	m_type;	
		int				m_iPrecision;
		int				m_iScale;
		int				m_iSize;
		CVariant		m_defValue;
		bool			m_bPrimaryKey;
		bool			m_bTrim;
		bool			m_iRequired;
		bool			m_bCalculated;
	};	

	typedef ACE_Vector<CFieldInfo>		CFieldInfos;
};

#endif //_PDT_DATABASE_FIELDINFO_H_

/*==============================================================================
* 文件名称 : fieldinfo.h
* 模块 : 数据库表字段元信息
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : v1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 :
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
		//构造函数
		CFieldInfo();
		CFieldInfo( const CFieldInfo & other );
		virtual	~CFieldInfo();
		CFieldInfo &	operator=( const CFieldInfo & other );
		bool			operator==( const CFieldInfo & f ) const;

		static	CFieldInfo		null;
	public:
		//字段名称
		const CString&	name() const;
		void			setName(const CString& name);
		//数据类型
		CVariant::Type	type() const;
		void			setType(CVariant::Type type);
		//数据精度
		int				precision() const;
		void			setPrecision(int precision);
		//小数点位数
		int				scale() const;
		void			setScale(int scale);
		//数据字节长度(数据库中的存储长度)
		int				size() const;
		void			setSize(int size);
		//默认值
		CVariant		defaultValue() const;
		void			setDefaultValue(const CVariant& defaultValue);
		//主键
		bool			isPrimaryKey()	const;
		void			setPrimaryKey(bool primaryKey);
		//如果是字符串类型，是否允许截断尾部空格
		bool			isTrim() const;
		void			setTrim ( bool trim );
		//是否允许为空(NULL)
		bool			isRequired() const;
		void			setRequired(bool required);
		//是否是计算字段
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

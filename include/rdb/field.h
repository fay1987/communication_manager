/*==============================================================================
* 文件名称 : field.h
* 模块 : 数据域
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 2007年7月25日
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
	//数据域
	class	DB_EXPORT	CField
	{
	public:
		//数据标志
		enum	DataFlag
		{
			Null = 0,		//空
			Normal,			//正常数据
			Update,		    //需要update to 数据库
		};
	public:
		CField();
		CField(const CField &field);
		virtual	~CField();
	public:
		//返回字段状态
		bool				isNull()	const;
		bool				isNormal() const;
		bool				isUpdate() const;
		//设置字段状态
		void				setFlagNone();
		void				setFlagNormal();
		void				setFlagUpdate();
		//取值
		CVariant			&value() ;
		const CVariant		&value() const;
		//设值
		const CVariant		&value(const CVariant &value);
	public:
		CField				&operator=(const CField &field);
		bool				operator==(const CField &field)	const;
		bool				operator!=(const CField &field)	const;
	public:
		static	CField		null;			//空对象
	private:
		CVariant			m_value;		//值
		DataFlag			m_dataFlag;		//标志
	};
	//数据域数组
	typedef	ACE_Vector<CField>	CFields;
}

#endif

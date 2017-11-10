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
#ifndef	_PDT_DATABASE_DBSQL_H_
#define	_PDT_DATABASE_DBSQL_H_

#include "utl/variant.h"
#include "utl/string.h"
#include "ace/Vector_T.h"
#include "rdb/db_export.h"
//---------------------------------------------------------------------------------------
namespace	PDT
{
	typedef struct _column_info
	{
		CString		colName;  //字段名
		CVariant	val;      //值
		bool		useflag;  //标识该字段是否被使用
	}ColInfo;

	typedef struct _where_info
	{
		CString		name;	  //字段名称或"("或")"或"AND"
		int			cmp;      //比较操作符
		CVariant	val;      //值
		const	CVariant	*pval;    //指针用于绑定
		bool		opflag;   //本结构是否是操作符的标识
		bool		bindflag; //是否绑定标识

		_where_info()
		{
			val = CVariant::null;
			pval = NULL;
			opflag = false;
			bindflag = false;
		}
	}WhereInfo;
	
	//order by no,name(可能会出现多个字段的order by)
	typedef struct _order_info
	{
		CString		name;
		CString		sort;  //asc,dsc,空
		bool		useflag;
	}OrderInfo;

	typedef ACE_Vector<ColInfo>		VEC_COLINFO;
	typedef ACE_Vector<WhereInfo>	VEC_WHEREINFO;
	typedef ACE_Vector<OrderInfo>	VEC_ORDERINFO;

	class DB_EXPORT CSql
	{
	public:
		//操作类型
		enum	Operate
		{
			OP_None = 0,
			OP_Select = 1,
			OP_Insert = 2,
			OP_Update = 3,
			OP_Delete = 4,
			OP_CALL	= 5,			//yhh20090827
			OP_EXEC = 6				//yhh20090827
		};
		enum	Compare
		{
			CP_Equal = 0,		//等于
			CP_NotEqual,		//不等于
			CP_Greater,			//大于
			CP_GreaterEqual,	//大于等于
			CP_Less,			//小于
			CP_LessEqual		//小于等于
		};
		enum	Logic
		{
			LG_Not = 0,			//非
			LG_And,				//与
			LG_Or				//或
		};
		enum	Sort
		{
			S_Defult = 0,		//默认
			S_Asc,				//升序
			S_Desc				//降序
		};

	public:
		CSql();
		virtual	~CSql();
	public:
		//释放对象
		virtual	void	destroy();
		//清空所有内容
		virtual void	clear();
		//设置操作类型（insert、update、delete、select）
		virtual bool	setOperate(Operate op);
		//设置表名
		virtual bool	setTableName(const char *tableName);
		//----------------------------------------------------------
		//SELECT DISTINCT
		virtual void	selectDistinct(bool dist=true);
		//增加SELECT字段
		virtual bool	selectField(const char *fieldName);
		//删除SELECT字段
		virtual bool	deSelectField(const char *fieldName);
		//增加所有SELECT字段
		virtual bool	selectAllField();
		//删除所有SELECT字段
		virtual bool	deSelectAllField();
		//----------------------------------------------------------
		//增加UPDATE字段
		virtual bool	updateField(const char *fieldName,const CVariant &value);
		//删除UPDATE字段
		virtual bool	deUpdateField(const char *fieldName);
		//----------------------------------------------------------
		//增加INSERT字段
		virtual bool	insertField(const char *fieldName,const CVariant &value);
		//删除INSERT字段
		virtual bool	deInsertField(const char *fieldName);
		//----------------------------------------------------------
		//增加WHERE字段
		virtual bool	whereField(const char *fieldName,const CVariant &value,Compare cmp=CP_Equal,bool bindFlag=false);
		//增加WHERE左括号
		virtual bool	whereLeftBracket();
		//增加WHERE右括号
		virtual bool	whereRightBracket();
		//增加WHERE逻辑
		virtual bool	whereLogic(Logic logic=LG_And);
		//删除WHERE语
		virtual bool	whereClear();
		//----------------------------------------------------------
		//增加OEDER字段
		virtual bool	orderField(const char *fieldName,Sort sort=S_Defult);  //asc&desc升降序
		//删除OEDER字段
		virtual bool	deOrderField(const char *fieldName);
		//----------------------------------------------------------
		//增加GROUP字段
		virtual bool	groupField(const char *fieldName);
		//删除GROUP字段
		virtual bool	deGroupField(const char *fieldName);
		//----------------------------------------------------------
		virtual CString sql() const;
		virtual const CString &tableName() const;
		virtual CSql::Operate operateType(const CString &sql) const;  
		virtual CSql::Operate operateType() const;
		virtual CString operateTypeName(const CString &sql) const;
		virtual CString operateTypeName() const;

	protected: //以下函数请按照sql语法差异重载
		virtual CString makeSelectSql() const; //组select整句
		virtual CString makeInsertSql() const; //组insert整句
		virtual CString makeUpdateSql() const; //组update整句
		virtual CString makeDeleteSql() const; //组delete整句
		virtual CString makeWhereSql()  const; //组where 子句
		virtual CString makeOrderSql()  const; //组order by 子句

		virtual CString valueToString(const CVariant& value) const;
	protected:
		void init();		  //清空存入的子句
		bool addField(const char* fieldName,const CVariant& value=CVariant::null);
		bool removeField(const char* fieldName);
	protected:
		VEC_COLINFO m_vecColumn; //字段信息
		VEC_WHEREINFO m_vecWhere;//where信息
		VEC_ORDERINFO m_vecOrder;//用于order by 的字段信息
		CString		m_tableName; //表名
		Operate		m_opType;    //操作类型
		CString		m_group;//用于定义group by "field"

		bool		m_distinctFlag;  //
		bool		m_selectAllFlag; //全选标志.
		
	};
};
//---------------------------------------------------------------------------------------

#endif

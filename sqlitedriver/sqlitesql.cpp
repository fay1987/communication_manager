/*==============================================================================
* 文件名称 : sqlitesql.cpp
* 模块 : SQLITE SQL 语句构造
* 创建时间 : 2011-03-25 15:55:23
* 作者 : wlq
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : XXXX年XX月XX日
==============================================================================*/

#include "sqlitesql.h"

using namespace PDT;

CSqliteSQL::CSqliteSQL()
{
}

CSqliteSQL::~CSqliteSQL()
{
}

void	CSqliteSQL::destroy()
{
	delete	this;
}

/*
* 功	能：将值转换为stirng
* 输入参数：value-值
* 输出参数：无
* 返	回：true-成功;false-失败.
* 说　　明：若值类型为CDateTime,则统一用ISODate格式输出，数据库可以解析多种格式．
*/
CString CSqliteSQL::valueToString(const CVariant& value) const
{
	CVariant::Type type = value.type();
	CString str;

	if (type==CVariant::String)
	{
		str =  "'" +value.toString() +"'";
	}
	else if (type == CVariant::DateTime)
	{
		str =  "to_date('" + value.toDateTime().toString(DT_FMT_DEF) + "','yyyy-mm-dd hh24:mi:ss')";
	}
	else
	{//值类型
		str =  value.toString();
	}

	return str;
}

extern "C" DRIVER_EXPORT PDT::CSql * CreateSQL()
{
	return ( new CSqliteSQL() );
} 


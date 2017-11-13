/*==============================================================================
* �ļ����� : sqlitesql.cpp
* ģ�� : SQLITE SQL ��乹��
* ����ʱ�� : 2011-03-25 15:55:23
* ���� : wlq
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : XXXX��XX��XX��
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
* ��	�ܣ���ֵת��Ϊstirng
* ���������value-ֵ
* �����������
* ��	�أ�true-�ɹ�;false-ʧ��.
* ˵����������ֵ����ΪCDateTime,��ͳһ��ISODate��ʽ��������ݿ���Խ������ָ�ʽ��
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
	{//ֵ����
		str =  value.toString();
	}

	return str;
}

extern "C" DRIVER_EXPORT PDT::CSql * CreateSQL()
{
	return ( new CSqliteSQL() );
} 


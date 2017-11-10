/*==============================================================================
* �ļ����� : fdcconfig.h
* ģ�� : ϵͳ������Ϣ��ȡ
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#include "fdc/fdcconfig.h"
#include "rdbop.h"

using namespace BFS;

CFdcConfig::CFdcConfig()
{

}

CFdcConfig::~CFdcConfig()
{

}

hInt32 CFdcConfig::intValue(const char* name)
{
	if ( !name )
		return -1;

	CRdbOp rdbop(CRdbOp::PingDirect);
	CSql* pSql = rdbop.createSQL();
	if ( !pSql )
	{
		printf( "CFdcConfig::intValue() : rdbop.createSQL() failed\n" );
		return -1;
	}
	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("sys_config");
	pSql->selectAllField();
	pSql->whereField("name",name);

	int ret;
	CDataset ds;
	ret = rdbop.exec( *pSql,ds);

	if ( ret <= 0 )
	{
		printf( "CFdcConfig::intValue() : rdbop.exec( *pSql,ds) failed\n" );
		pSql->destroy();
		return -1;
	}

	pSql->destroy();

	return ds.field( 0,"intval").value().toInt32();
}

hFloat CFdcConfig::floatValue(const char* name)
{
	if ( !name )
		return -1;

	CRdbOp rdbop(CRdbOp::PingDirect);
	CSql* pSql = rdbop.createSQL();
	if ( !pSql )
	{
		printf( "CFdcConfig::floatValue() : rdbop.createSQL() failed\n" );
		return -1;
	}
	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("sys_config");
	pSql->selectAllField();
	pSql->whereField("name",name);

	int ret;
	CDataset ds;
	ret = rdbop.exec( *pSql,ds);

	if ( ret <= 0 )
	{
		printf( "CFdcConfig::floatValue() : rdbop.exec( *pSql,ds) failed\n" );
		pSql->destroy();
		return -1;
	}

	pSql->destroy();

	return ds.field( 0,"floatval").value().toFloat();
}

CString CFdcConfig::stringValue(const char* name)
{
	if ( !name )
		return "";

	CRdbOp rdbop(CRdbOp::PingDirect);
	CSql* pSql = rdbop.createSQL();
	if ( !pSql )
	{
		printf( "CFdcConfig::stringValue() : rdbop.createSQL() failed\n" );
		return "";
	}
	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("sys_config");
	pSql->selectAllField();
	pSql->whereField("name",name);

	int ret;
	CDataset ds;
	ret = rdbop.exec( *pSql,ds);

	if ( ret <= 0 )
	{
		printf( "CFdcConfig::stringValue() : rdbop.exec( *pSql,ds) failed\n" );
		pSql->destroy();
		return "";
	}

	pSql->destroy();

	return ds.field( 0,"strval").value().toString();
}

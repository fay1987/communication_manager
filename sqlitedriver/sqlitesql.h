/*==============================================================================
* �ļ����� : sqlitesql.h
* ģ�� : SQLITE SQL��乹����
* ����ʱ�� : 2011-03-26 15:55:23
* ���� : wlq
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_PDT_DATABASE_SQLITESQL_H_
#define	_PDT_DATABASE_SQLITESQL_H_

#include "rdb/sql.h"
#include "rdb/driver_export.h"

namespace	PDT
{
	class DRIVER_EXPORT  CSqliteSQL : public CSql
	{
	public:
	public:
		CSqliteSQL();
		virtual	~CSqliteSQL();
	public:
		//�ͷŶ���
		void	destroy();
	private:
		virtual CString valueToString(const CVariant& value) const;
	};
};
//---------------------------------------------------------------------------------------

#endif //_PDT_DATABASE_SQLITESQL_H_

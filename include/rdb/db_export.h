#ifndef	_PDT_DATABASE_DB_EXPORT_H_
#define	_PDT_DATABASE_DB_EXPORT_H_

#include <ace/config-all.h>

//=============================================================================
#if defined (_PDT_RDB_)
#  define	DB_EXPORT	ACE_Proper_Export_Flag
#else
#  define	DB_EXPORT	ACE_Proper_Import_Flag
#endif	// _DBAPI_
//=============================================================================

#endif	// _PDT_DATABASE_DB_EXPORT_H_

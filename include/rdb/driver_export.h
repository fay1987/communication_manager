#ifndef	_ECON_DATABASE_DBDRIVER_EXPORT_H_
#define	_ECON_DATABASE_DBDRIVER_EXPORT_H_

#include <ace/config-all.h>

//=============================================================================
#if defined (_DRIVER_DLL_)
#  define	DRIVER_EXPORT	ACE_Proper_Export_Flag
#else
#  define	DRIVER_EXPORT	ACE_Proper_Import_Flag
#endif	// _DRIVER_DLL_
//=============================================================================

#endif	// _ECON_DATABASE_DBDRIVER_EXPORT_H_

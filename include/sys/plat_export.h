#ifndef	_PDT_PLAT_EXPORT_H_
#define	_PDT_PLAT_EXPORT_H_

#include <ace/config-all.h>

//=============================================================================
#if defined (_PDT_PLAT_)
#  define	PLAT_EXPORT	ACE_Proper_Export_Flag
#else
#  define	PLAT_EXPORT	ACE_Proper_Import_Flag
#endif	// _PDT_PLAT_
//=============================================================================

#endif	// _PDT_PLAT_EXPORT_H_

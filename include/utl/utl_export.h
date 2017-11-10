#ifndef	_PD_UTL_UTL_EXPORT_H_
#define	_PD_UTL_UTL_EXPORT_H_

#include <ace/config-all.h>

//=============================================================================
#if defined (_PD_UTL_)
#  define	UTL_EXPORT	ACE_Proper_Export_Flag
#else
#  define	UTL_EXPORT	ACE_Proper_Import_Flag
#endif	// _PD_UTL_
//=============================================================================

#endif	// _PD_UTL_UTL_EXPORT_H_

#ifndef	_PDT_PAS_EXPORT_H_
#define	_PDT_PAS_EXPORT_H_

#include <ace/config-all.h>

//=============================================================================
#if defined (_PAS_)
#  define	PAS_EXPORT	ACE_Proper_Export_Flag
#else
#  define	PAS_EXPORT	ACE_Proper_Import_Flag
#endif	// _DAC_
//=============================================================================

#endif	// _PDT_PAS_EXPORT_H_

#ifndef	_PDT_DAC_EXPORT_H_
#define	_PDT_DAC_EXPORT_H_

#include <ace/config-all.h>

//=============================================================================
#if defined (_DAC_)
#  define	DAC_EXPORT	ACE_Proper_Export_Flag
#else
#  define	DAC_EXPORT	ACE_Proper_Import_Flag
#endif	// _DAC_
//=============================================================================

#endif	// _PDT__DAC_EXPORT_H_

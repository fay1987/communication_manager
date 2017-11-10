#ifndef _ALGORITHM_EXP_H_
#define _ALGORITHM_EXP_H_

#include <ace/config-all.h>
//=============================================================================
#if defined (_EXPCALC_)
#  define	ALGORITHM_EXPORT	ACE_Proper_Export_Flag
#else
#  define	ALGORITHM_EXPORT	ACE_Proper_Import_Flag
#endif	// _EXPCALC_
//=============================================================================

#endif
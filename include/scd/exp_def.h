#ifndef _EXP_CALC_DEF_H_
#define _EXP_CALC_DEF_H_

#include <ace/config-all.h>
//=============================================================================
#if defined (_EXPCALC_)
#  define	EXPCALC_EXPORT	ACE_Proper_Export_Flag
#else
#  define	EXPCALC_EXPORT	ACE_Proper_Import_Flag
#endif	// _EXPCALC_
//=============================================================================

#endif

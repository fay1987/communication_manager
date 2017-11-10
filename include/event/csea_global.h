#ifndef CSEA_GLOBAL_H
#define CSEA_GLOBAL_H

#include <ace/config-all.h>
//=============================================================================
#if defined (CSEA_LIB)
#  define	CSEA_EXPORT	ACE_Proper_Export_Flag
#else
#  define	CSEA_EXPORT	ACE_Proper_Import_Flag
#endif	// CSEA_LIB
//=============================================================================

#endif // CSEA_GLOBAL_H

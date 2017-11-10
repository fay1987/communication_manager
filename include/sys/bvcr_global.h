#ifndef BVCR_GLOBAL_H
#define BVCR_GLOBAL_H

#include <Qt/qglobal.h>

#ifdef BVCR_LIB
# define BVCR_EXPORT Q_DECL_EXPORT
#else
# define BVCR_EXPORT Q_DECL_IMPORT
#endif

#endif // BVCR_GLOBAL_H

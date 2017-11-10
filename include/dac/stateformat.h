#ifndef _STATE_FROMAT_H_
#define _STATE_FROMAT_H_

#include "dac/dacdef.h"
#include "utl/string.h"
#include "dac/dac_export.h"

namespace PDT
{
	/*===================状态的格式字符串类=========================*/
	class DAC_EXPORT CStateFormat
	{
	public:
		static CString channelStateDesc(hInt32 state);
		static CString groupStateDesc(hInt32 state);
		static CString routeStateDesc(hInt32 state);
		static CString mainFlagDesc(hBool flag);
		static CString channelTypeDesc(hUInt8 type);
	};
}


#endif	//_STATE_FROMAT_H_



#ifndef GATE_PROTOCOL_H
#define GATE_PROTOCOL_H

#include "utl/datatype.h"

class GProtocol
{
public:
	GProtocol(){};
	virtual ~GProtocol(){};

	virtual void	setPackage(hUChar *data, int len) = 0;
	virtual int		getClientNo() = 0;
	virtual int		sendOK(hUChar *data) = 0;
	virtual int		sendERR(hUChar *data) = 0;
};


#endif

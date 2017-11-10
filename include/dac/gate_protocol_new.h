#ifndef GATE_PROTOCOL_H
#define GATE_PROTOCOL_H

#include "utl/datatype.h"

class GProtocol
{
public:
	enum frm_tpye
	{
		FRM_DATA = 0,
		FRM_REGISTER,
		FRM_HEART
	};

public:
	virtual frm_tpye set_data(char *data, int len) = 0;
	virtual int get_remote_addr() = 0;
	virtual bool transfer_all_data() { return false; }
};


#endif

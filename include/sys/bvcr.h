#ifndef BVCR_H
#define BVCR_H

#include "bvcr_global.h"

#define LICENSE_KEY		18920965

class BVCR_EXPORT bvcr
{
public:
	bvcr();
	~bvcr();

	int		GetLicenseCode(long key,int valid_day,unsigned long hostid,char *license_code);
	bool	VerifyLicense();

	bool		CreateLicenseFile(long key,int valid_day,unsigned long hostid);

private:

};

#endif // BVCR_H

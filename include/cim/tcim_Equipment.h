/***********************************************************************
 * Module:  tcim_Equipment.h
 * Author:  think
 * Modified: 2016Äê11ÔÂ28ÈÕ 12:07:48
 * Purpose: Declaration of the class tcim_Equipment
 ***********************************************************************/

#if !defined(__CR8000_tcim_Equipment_h)
#define __CR8000_tcim_Equipment_h

#include <tcim_IdentifiedObject.h>

class tcim_Equipment : public tcim_IdentifiedObject
{
public:
   short aggregat(void);
   short normallyinservice(void);
   std::string stationcode(void);
   std::string groupcode(void);
   std::string paratabname(void);
   int no(void);

   short f_aggregat;
   short f_normallyinservice;
   std::string f_stationcode;
   std::string f_groupcode;
   std::string f_paratabname;
   int f_no;

protected:
private:

};

#endif
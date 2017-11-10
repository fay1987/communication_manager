/***********************************************************************
 * Module:  tcim_Station.h
 * Author:  think
 * Modified: 2016Äê11ÔÂ28ÈÕ 12:06:10
 * Purpose: Declaration of the class tcim_Station
 ***********************************************************************/

#if !defined(__CR8000_tcim_Station_h)
#define __CR8000_tcim_Station_h

#include <tcim_IdentifiedObject.h>

class tcim_Station : public tcim_IdentifiedObject
{
public:
   int no(void);
   short level(void);
   std::string addr(void);
   std::string area(void);
   std::string memo(void);
   short isdelete(void);
   float locationx(void);
   float locationy(void);

   int f_no;
   short f_level;
   std::string f_addr;
   std::string f_area;
   std::string f_memo;
   short f_delete;
   float f_locationx;
   float f_locationy;

protected:
private:

};

#endif
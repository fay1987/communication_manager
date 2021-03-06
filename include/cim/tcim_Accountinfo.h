/***********************************************************************
 * Module:  tcim_Accountinfo.h
 * Author:  think
 * Modified: 2016��11��28�� 12:12:29
 * Purpose: Declaration of the class tcim_Accountinfo
 ***********************************************************************/

#if !defined(__CR8000_tcim_Accountinfo_h)
#define __CR8000_tcim_Accountinfo_h

class tcim_Accountinfo
{
public:
   std::string paracode(void);
   std::string devcode(void);
   std::string paraname(void);
   std::string value(void);
   short valtype(void);
   std::string valunit(void);
   short useflag(void);
   short isshow(void);
   int showorder(void);
   int no(void);

   std::string f_paracode;
   std::string f_devcode;
   std::string f_paraname;
   std::string f_value;
   short f_valtype;
   std::string f_valunit;
   short f_useflag;
   short f_isshow;
   int f_showorder;
   int f_no;

protected:
private:

};

#endif
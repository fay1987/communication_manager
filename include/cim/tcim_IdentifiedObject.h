/***********************************************************************
 * Module:  tcim_IdentifiedObject.h
 * Author:  think
 * Modified: 2016年11月28日 11:49:56
 * Purpose: Declaration of the class tcim_IdentifiedObject
 * Comment: 统一资源码
 ***********************************************************************/

#if !defined(__CR8000_tcim_IdentifiedObject_h)
#define __CR8000_tcim_IdentifiedObject_h

class tcim_IdentifiedObject
{
public:
   std::string mrid(void);
   std::string name(void);
   std::string aliasname(void);
   std::string description(void);

   std::string f_mrid;
   std::string f_name;
   std::string f_aliasname;
   std::string f_description;

protected:
private:

};

#endif
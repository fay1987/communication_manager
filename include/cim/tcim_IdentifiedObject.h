/***********************************************************************
 * Module:  tcim_IdentifiedObject.h
 * Author:  think
 * Modified: 2016��11��28�� 11:49:56
 * Purpose: Declaration of the class tcim_IdentifiedObject
 * Comment: ͳһ��Դ��
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
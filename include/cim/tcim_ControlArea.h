/***********************************************************************
 * Module:  tcim_ControlArea.h
 * Author:  think
 * Modified: 2016��11��28�� 11:54:42
 * Purpose: Declaration of the class tcim_ControlArea
 * Comment: ��������
 ***********************************************************************/

#if !defined(__CR8000_tcim_ControlArea_h)
#define __CR8000_tcim_ControlArea_h

#include <tcim_IdentifiedObject.h>

class tcim_ControlArea : public tcim_IdentifiedObject
{
public:
   std::string uparea(void);
   bool isroot(void);

   std::string f_uparea;
   bool f_isroot;

protected:
private:

};

#endif
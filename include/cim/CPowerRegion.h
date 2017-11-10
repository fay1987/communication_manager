/***********************************************************************
 * Module:  CPowerRegion.h
 * Author:  think
 * Modified: 2016Äê11ÔÂ28ÈÕ 11:57:25
 * Purpose: Declaration of the class CPowerRegion
 ***********************************************************************/

#if !defined(__CR8000_CPowerRegion_h)
#define __CR8000_CPowerRegion_h

#include <tcim_ControlArea.h>

class CPowerRegion : public tcim_ControlArea
{
public:
   float RInstallCapacity(void);
   int RNumber(void);
   float RDayPower(void);
   float RTotPower(void);
   float RCO2(void);
   float RPowerIncome(void);
   float RTotPR(void);
   float RDayPR(void);
   float RInstallCR(void);
   float RPowerIR(void);
   float RNumberR(void);
   float RCO2R(void);
   void* RPowerGraph(void);
   void* RIncomGraph(void);

   float _RInstallCapacity;
   int _RNumber;
   float _RDayPower;
   float _RTotPower;
   float _RCO2;
   float _RPowerIncome;
   float _RTotPR;
   float _RDayPR;
   float _RInstallCR;
   float _RPowerIR;
   float _RNumberR;
   float _RCO2R;
   long _RPowerGraph;
   long _RIncomGraph;

protected:
private:

};

#endif
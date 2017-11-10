/***********************************************************************
 * Module:  CPowerStation.h
 * Author:  think
 * Modified: 2016Äê11ÔÂ28ÈÕ 12:03:39
 * Purpose: Declaration of the class CPowerStation
 ***********************************************************************/

#if !defined(__CR8000_CPowerStation_h)
#define __CR8000_CPowerStation_h

#include <tcim_Station.h>

class CPowerStation : public tcim_Station
{
public:
   int ymd(void);
   float SInstallCapacity(void);
   float SDayPower(void);
   float STotPower(void);
   float PowerIncome(void);
   float STotPR(void);
   float SPowerIR(void);
   float SInstallCR(void);
   float SDayPR(void);
   void* DPowerBarGraph(void);
   void* WPowerBarGraph(void);
   void* MPowerBarGraph(void);
   void* YearPowerGtraph(void);
   void* SPowerGraph(void);
   void* SIncomGraph(void);
   int RunGraph(void);
   void* REGraph(void);
   void* ACInfor(void);
   void* DCInfor(void);

   int _ymd;
   float _SInstallCapacity;
   float _SDayPower;
   float _STotPower;
   float _PowerIncome;
   float _STotPR;
   float _SPowerIR;
   float _SInstallCR;
   float _SDayPR;
   long _DPowerBarGraph;
   long _WPowerBarGraph;
   long _MPowerBarGraph;
   long _YearPowerGraph;
   long _SPowerGraph;
   long _SIncomGraph;
   int _RunState;
   long _REGraph;
   long _ACInfor;
   long _DCInfor;

protected:
private:

};

#endif
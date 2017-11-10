#ifndef _DAC_EVENT_H_	
#define _DAC_EVENT_H_

#include <ace/config-all.h>
//=============================================================================
#if defined (_DAC_EVENT_)
#  define	DACEVENT_EXPORT	ACE_Proper_Export_Flag
#else
#  define	DACEVENT_EXPORT	ACE_Proper_Import_Flag
#endif	// _DAC_EVENT_
//=============================================================================
#include "dac/comminf.h"
#include "event/csea.h"

namespace PDT
{
	class DACEVENT_EXPORT CDacEvent
	{
	public:
		CDacEvent();
		virtual ~CDacEvent();
		int			channelChange(hInt32 channelNo,hUInt8 oldState,hUInt8 newState);	//ͨ��״̬�仯
		int			groupChange(hInt32 groupNo,hUInt8 oldState,hUInt8 newState);		//������״̬�仯
		int			groupChange(hInt32 groupNo,short routeNo);							//��������·���仯
		int			routeChange(hInt32 routeNo,hUInt8 oldState,hUInt8 newState);		//·��״̬�仯
		int			routeChange(hInt32 routeNo,hUInt8 mainFlag);						//·������ʶ�仯
	private:
		CCommInf		m_commInf;	
		CSysEvent		m_event;
	};
}


#endif //_DAC_EVENT_H_

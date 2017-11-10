#ifndef _EVT_ANALYZE_H_
#define _EVT_ANALYZE_H_

#include "event/csea.h"
#include "event/type.h"
#include <map>
#include <vector>
#include "utl/string.h"
#include "dac/comminf.h"
#include "dac/datainf.h"

#include <ace/config-all.h>
//=============================================================================
#if defined (_EVT_ANALYZE_)
#  define	EVT_ANALYZE_EXPORT	ACE_Proper_Export_Flag
#else
#  define	EVT_ANALYZE_EXPORT	ACE_Proper_Import_Flag
#endif	// _EVT_ANALYZE_
//=============================================================================

namespace PDT
{
	#define LOG_EVT_ANALYZE		10000
	#define TABLE_EVT_STATE		"tdac_event_state" 

	typedef struct _evt_state_descr_ 
	{
		int				sort;
		int				type;
		int				state;
		PDT::CString	descr;
	}EVT_STATE_DESCR;

	typedef std::vector<EVT_STATE_DESCR>		EVT_STATE_VECTOR;

	typedef std::map<hInt32,EVT_STATE_VECTOR>	EVT_STATE_MAP;		//key is type 

	class EVT_ANALYZE_EXPORT CEvtAnalyze
	{
	public:
		CEvtAnalyze();
		virtual ~CEvtAnalyze();

		bool			init();
		bool			analyze(PDT::nSysEvent&	data);
		bool			stateDescr(int type,int state,PDT::CString& descr);
	protected:
		void			addState(const EVT_STATE_DESCR&  data);
	private:
		bool			m_isInited;
		EVT_STATE_MAP	m_stateMap;
		CCommInf		m_commInf;
		CDataInf		m_dataInf;
	};

}

#endif

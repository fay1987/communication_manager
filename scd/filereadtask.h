#ifndef _PDT_SCD_FILEREAD_TASK_H_
#define _PDT_SCD_FILEREAD_TASK_H_

#include "utl/intervaltimerset.h"
#include "ace/Task.h"
#include "ace/Thread_Manager.h"
#include "utl/string.h"

class CFileReadTask : public ACE_Task<ACE_MT_SYNCH>
{
public:
	CFileReadTask();
	virtual ~CFileReadTask();
public:
	bool		init();
	int			start();
	void		stop();
	int			svc();
protected:
	void		readFiles();
	void		procHisDataFile(const char* fname);
	bool		putData(const char* databuf,int size);
private:
	PDT::CIntervalTimerSet	m_timer;
	int						m_timeId;
	PDT::CString			m_dirName;				
};

typedef ACE_Singleton<CFileReadTask, ACE_Null_Mutex> TASK_FILEREAD;

#endif	//_PDT_SCD_FILEREAD_TASK_H_

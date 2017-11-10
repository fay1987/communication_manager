#ifndef _PDT_SCD_CONFIG_H_
#define _PDT_SCD_CONFIG_H_

#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <sys/sysmacro.h>
#include "utl/datatype.h"

#define SCD_CONFIG_FILE	"scd.ini"

class CConfig
{
public:
	CConfig();
	virtual ~CConfig();

	bool			read();
	bool			isSaveWater() const;		//是否处理阀值
	bool			hasBattery() const;			//是否需要处理蓄电池
	bool			microNet() const;			//是否微网运行模式(蓄电池专用)
	hUInt32			saveMode() const;			//数据库存盘模式;1--不处理_bak表；2--处理_bak表
	bool			isExecProcedure() const;	//是否执行统计存储过程
	bool			isSaveErrFile() const;		//是否将存库失败的数据存到errfile目录
	bool			isYcInSgz() const;


	bool			isRunTask_dbsave() const;	
	bool			isRunTask_hisdata() const;	
	bool			isRunTask_fileread() const;	
	bool			isRunTask_calc() const;	
	bool			isRunTask_limit() const;	


private:
	bool			m_isSaveWater;
	bool			m_hasBattery;
	bool			m_microNet;
	hUInt32			m_saveMode;
	bool			m_isExecProcedure;
	bool			m_isSaveErrFile;
	bool			m_isYcInSgz;

	bool			m_isRunTask_dbsave;
	bool			m_isRunTask_hisdata;
	bool			m_isRunTask_fileread;
	bool			m_isRunTask_calc;
	bool			m_isRunTask_limit;


};

typedef	ACE_Singleton<CConfig,ACE_Null_Mutex>	CONFIG;

#endif	//_PDT_SCD_CONFIG_H_

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
	bool			isSaveWater() const;		//�Ƿ���ֵ
	bool			hasBattery() const;			//�Ƿ���Ҫ��������
	bool			microNet() const;			//�Ƿ�΢������ģʽ(����ר��)
	hUInt32			saveMode() const;			//���ݿ����ģʽ;1--������_bak��2--����_bak��
	bool			isExecProcedure() const;	//�Ƿ�ִ��ͳ�ƴ洢����
	bool			isSaveErrFile() const;		//�Ƿ񽫴��ʧ�ܵ����ݴ浽errfileĿ¼
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

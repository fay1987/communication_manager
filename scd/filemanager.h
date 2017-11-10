#ifndef _PDT_SCD_FILE_MANAGER_H_
#define _PDT_SCD_FILE_MANAGER_H_

#include "utl/string.h"
#include <ace/OS.h>
#include "scdmacro.h"

class CFileManager
{
public:
	CFileManager();
	virtual ~CFileManager();

	/**
	* @function			��ʼ��
	* @param dir		�ļ�������,��"hisdata"
	* @param prefix		�ļ���ǰ׺,��"err"
	* @param maxRecords �ļ��ɴ洢������¼��
	* @retval			true:�ɹ�
	*/
	bool			init(const char* dir,const char* prefix,int maxRecords=SCD_MAX_RECORDS_INFILE);

	bool			read();
	bool			save(const char* pData,int length);
protected:
	bool			openNewFile();
	bool			closeFile();
	bool			rename();
private:
	bool			m_isOpen;
	PDT::CString	m_dirName;
	PDT::CString	m_prefix;
	PDT::CString	m_tmpFName;		//��ʱ�ļ���
	hUInt32			m_maxRecords;
	hUInt32			m_curRecords;
	FILE*			m_pFile;
};

#endif	//_PDT_SCD_FILE_MANAGER_H_

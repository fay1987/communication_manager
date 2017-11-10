/*==============================================================================
* �ļ����� : directory.h
* ģ�� : Ŀ¼����
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : ����ƽ
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 2007��7��25��
==============================================================================*/
#ifndef	_PDT_UTL_DIRECTORY_H_
#define	_PDT_UTL_DIRECTORY_H_

#include "utl/utl_export.h"
#include "utl/string.h"
#include "ace/OS.h"
#include "ace/Log_Msg.h"

namespace PDT
{
	class UTL_EXPORT CDir
	{
	public:
		CDir();
		virtual ~CDir();
		virtual	int		getDir(const CString& parent) const;
	public:
		static	void	makeDir(const CString& path);
		static	bool	isDirectory(const CString&  path);
		static	bool	isRelationDir(const CString&  path);
		static	CString	convertLocalDir(const CString&  path);
		static	CString	convertAbsDir(const CString&  path);
	protected:
		virtual int	 getOneDir(const CString& parent, const CString& dirName = "") const;
		virtual int  handleOneFile(const CString& pathFileName, const CString& name) const;
		//�����ļ�������������Ĭ�ϲ�����
		virtual bool byPass(const CString& pathFileName) const;
	};
	class UTL_EXPORT CDirectory
	{
	public:
		CDirectory(const CString &path=CString::null);
		virtual	~CDirectory();
	public:
		//·��
		const	CString	&path()	const;
		void			setPath(const CString &path);
		//����
		bool			isExist()	const;
		//��Ծ���
		bool			isRelative()	const;
		CString			absPath()	const;
		//����·��
		void			mkdir(const CString &path=CString::null);
	public:
		//����·��
		static	const	CString	&workPath();
		static	void	setWorkPath(const CString &path);
	private:
		CString	m_path;
	private:
		static	CString	m_workPath;		//����·��
	};
}

#endif



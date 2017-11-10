/*==============================================================================
* 文件名称 : directory.h
* 模块 : 目录处理
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 2007年7月25日
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
		//忽略文件的条件；这里默认不忽略
		virtual bool byPass(const CString& pathFileName) const;
	};
	class UTL_EXPORT CDirectory
	{
	public:
		CDirectory(const CString &path=CString::null);
		virtual	~CDirectory();
	public:
		//路径
		const	CString	&path()	const;
		void			setPath(const CString &path);
		//存在
		bool			isExist()	const;
		//相对绝对
		bool			isRelative()	const;
		CString			absPath()	const;
		//创建路径
		void			mkdir(const CString &path=CString::null);
	public:
		//工作路径
		static	const	CString	&workPath();
		static	void	setWorkPath(const CString &path);
	private:
		CString	m_path;
	private:
		static	CString	m_workPath;		//工作路径
	};
}

#endif



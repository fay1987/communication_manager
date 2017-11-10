/*===================================================================
*	模块名称：	
*	创建日期:	2013年11月26日 15:03
*	文 件 名: 	E:\pdt3000\include\sys\blog.h
*	文件路径:	E:\pdt3000\include\sys
*	模块名称:	blog
*	文件类型:	h
	
*	作    者:	王林青
*	版    本:	V2.0
	
*	模块说明:	

*	修改版本：	V2.1
*	修改作者：	王林青
*	修改日期：	26/11/2013
*	修改摘要：

====================================================================*/
#ifndef	_BF_LOG_H_
#define	_BF_LOG_H_

#include "sys/plat_export.h"
#include "utl/datetime.h"
#include "sys/sysmacro.h"
#include "utl/sharememorybase.h"


//系统日志信息
#define BLOG_SHM_KEY		2010031701			//LOG共享内存标识
#define BLOG_TYPE_NUM		50000
#define BLOG_MAX_NUM		4096
#define BLOG_LEN			1024				//单日志内容长度
#define BLOG_HEAD_LEN		96
#define BLOG_FILE_LEN		5242880				//5M

//#define BLOG_SHM_DIR		"dat/map/sys"		//共享内存相对目录
//#define	BLOG_SHM_FILE		"blog.dat"			//共享内存映射文件

typedef struct
{
	unsigned char	lock;						//分锁--conflict for multithread loging
	unsigned short	type;						//日志类型id
	PDT::CDateTime	dateTime;					//产生时间
	char			procName[SYS_PROCNAME_LEN];	//产生日志的进程名
	char			msg[BLOG_LEN];
}BLOG_DATA;

typedef struct
{
	unsigned char	lock;						//总锁
	unsigned char	flock;						//文件锁-wfp add for printerr
	int				writePos;
	unsigned char	flag[BLOG_TYPE_NUM];		//记录logd是否登记该类型
	BLOG_DATA		data[BLOG_MAX_NUM];
}BLOG_MEM;

namespace PDT
{
	class PLAT_EXPORT CLogInfo : public CShareMemoryBase
	{
	public:
		CLogInfo();
		virtual	~CLogInfo();

		BLOG_MEM		*logMemPtr()	{return	m_ptr;}

		void			setDisplay(int type);
		void			clearDisplay(int type);
		void			setDisplayAll();
		void			clearDisplayAll();

		char			*readLog(int &type,char *loghead=NULL,BLOG_DATA **data=NULL,int flag=FALSE);  
		void			writeLog(int type,const char *format,...);
		void			writeError(int type,const char *format,...);  //写错误级别的日志
		void			displayBuffer(int type,const void *buf,int len);

	private:
		int 			lockWrite();
		void 			unLockWrite(int pos);

	private:
		static	BLOG_MEM		*m_ptr;
		static	char			m_procName[SYS_PROCNAME_LEN];
		int						m_readPos;
	public:
		//远端使用
		int				writePos() const;
		bool			readLog(int readPos,int& type,char *data=NULL,char *procNmae=NULL); 
	};
}//end namespace

#if	(defined(WIN32) && !defined(_PDT_PLAT_))
__declspec(dllimport)	PDT::CLogInfo	g_logInfo;
#elif	defined(__unix)
extern	PDT::CLogInfo	g_logInfo;
#endif

#define	logprint		(g_logInfo.writeLog)
#define errprint		(g_logInfo.writeError)  //wfp add

#endif

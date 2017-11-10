/*===================================================================
*	ģ�����ƣ�	
*	��������:	2013��11��26�� 15:03
*	�� �� ��: 	E:\pdt3000\include\sys\blog.h
*	�ļ�·��:	E:\pdt3000\include\sys
*	ģ������:	blog
*	�ļ�����:	h
	
*	��    ��:	������
*	��    ��:	V2.0
	
*	ģ��˵��:	

*	�޸İ汾��	V2.1
*	�޸����ߣ�	������
*	�޸����ڣ�	26/11/2013
*	�޸�ժҪ��

====================================================================*/
#ifndef	_BF_LOG_H_
#define	_BF_LOG_H_

#include "sys/plat_export.h"
#include "utl/datetime.h"
#include "sys/sysmacro.h"
#include "utl/sharememorybase.h"


//ϵͳ��־��Ϣ
#define BLOG_SHM_KEY		2010031701			//LOG�����ڴ��ʶ
#define BLOG_TYPE_NUM		50000
#define BLOG_MAX_NUM		4096
#define BLOG_LEN			1024				//����־���ݳ���
#define BLOG_HEAD_LEN		96
#define BLOG_FILE_LEN		5242880				//5M

//#define BLOG_SHM_DIR		"dat/map/sys"		//�����ڴ����Ŀ¼
//#define	BLOG_SHM_FILE		"blog.dat"			//�����ڴ�ӳ���ļ�

typedef struct
{
	unsigned char	lock;						//����--conflict for multithread loging
	unsigned short	type;						//��־����id
	PDT::CDateTime	dateTime;					//����ʱ��
	char			procName[SYS_PROCNAME_LEN];	//������־�Ľ�����
	char			msg[BLOG_LEN];
}BLOG_DATA;

typedef struct
{
	unsigned char	lock;						//����
	unsigned char	flock;						//�ļ���-wfp add for printerr
	int				writePos;
	unsigned char	flag[BLOG_TYPE_NUM];		//��¼logd�Ƿ�ǼǸ�����
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
		void			writeError(int type,const char *format,...);  //д���󼶱����־
		void			displayBuffer(int type,const void *buf,int len);

	private:
		int 			lockWrite();
		void 			unLockWrite(int pos);

	private:
		static	BLOG_MEM		*m_ptr;
		static	char			m_procName[SYS_PROCNAME_LEN];
		int						m_readPos;
	public:
		//Զ��ʹ��
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

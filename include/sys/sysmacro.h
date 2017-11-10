/*==============================================================================
* 文件名称 : sysmacro.h
* 文件功能 ：系统各种宏定义
* 模块 : 数据采集
* 创建时间 : 2009-06-08
* 作者 : www
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	PDT_SYS_MACRO_H_
#define	PDT_SYS_MACRO_H_

//wfp add at 20091216
#ifndef	TRUE
#define TRUE	1
#endif
#ifndef	FALSE
#define FALSE	0
#endif

#define SYS_PROCNAME_LEN	16						//进程名长度
#define SYS_PROCPARA_LEN	32						//进程执行参数长度
#define SYS_DESC_LEN		64						//描述长度 

#define PATH_LEN			256						//路径长度
#define FILE_NAME_LEN		32						//文件名长度
#define FILE_PATH_LEN		PATH_LEN+FILE_NAME_LEN	//文件名加路径总长度

#define SYS_THREAD_STACK_SIZE	(1*1024*1024)		//1M

#define SYS_ENV_VAR			"PDT_ROOT"				//系统环境变量


#endif //PDT_SYS_MACRO_H_

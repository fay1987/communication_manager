/*==============================================================================
* 文件名称 : cpuid.h
* 模块 : CPU序列号
* 创建时间 : 2015-06-28 15:55:23
* 作者 : wlq
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 2015年6月28日
* 遗留问题 : 
==============================================================================*/
#ifndef _PDT_UTL_CPU_ID_H_
#define _PDT_UTL_CPU_ID_H_

#include <Qt/qglobal.h>

#ifdef _PD_LIC_
# define LIC_EXPORT Q_DECL_EXPORT
#else
# define LIC_EXPORT Q_DECL_IMPORT
#endif

#include <QString>

namespace PDT
{
#define LOG_LIC_BASE				10

	enum	CheckResult
	{
		CR_OK			= 0,		/*授权成功*/
		CR_INVALID		= 1,		/*授权非法*/
		CR_EXPIRED		= 2,		/*授权过期*/
		CR_SUSPICIOUS	= 3,		/*授权可疑*/
		CR_NOLICENSE				/*缺失授权*/
	};

	class CCpuId
	{
		typedef unsigned long       DWORD;
		typedef int                 BOOL;
#define MAX_PATH	260
	public:	
		static CCpuId *Instance();					// Get instance pointer
		bool cpu_id();	// Get CPU Serial Number
		bool disk_id();
		QString strseriol() const;
	private:
		CCpuId();			// constructor
		virtual ~CCpuId();	// d-constructor

		DWORD m_eax;		// value eax after cpuid
		DWORD m_ebx;		// value ebx after cpuid
		DWORD m_ecx;		// value ecx after cpuid
		DWORD m_edx;		// value edx after cpuid
		DWORD m_serino;
		static CCpuId * m_pInstance;		// Instance pointer
	};

	class LIC_EXPORT CLicense
	{

	public:
		//QString generate_lic() const;
		int		check_valid(const QString & lic);
	};
}

#endif //_PDT_UTL_CPU_ID_H_

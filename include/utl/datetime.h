/*==============================================================================
* 文件名称 : datetime.h
* 模块 : 日期时间
* 创建时间 : 2008-04-28 15:55:23
* 作者 : aaa
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 2007年7月25日
==============================================================================*/
#ifndef	_PDT_UTL_DATETIME_H_
#define	_PDT_UTL_DATETIME_H_

#include <ace/Time_Value.h>
#include <ace/Date_Time.h>
#include "utl/utl_export.h"
#include "utl/string.h"

#define	DT_FMT_DEF		"yyyy-MM-dd hh:mm:ss"	//默认
#define	DT_FMT_MS		"yyyy-MM-dd hh:mm:ss zzz"	//MS
#define	DT_FMT_ISO		"yyyy-MM-ddThh:mm:ss"	//ISO


namespace PDT
{
	class UTL_EXPORT CDateTime : public ACE_Date_Time
	{
	public:
		CDateTime();
		CDateTime(const CDateTime &datetime);
		CDateTime(const ACE_Time_Value &timevalue);
		CDateTime(const nDateTime& datetime);
		CDateTime(long year,long month,long day,long hour=0,long minute=0,long second=0,long microsec=0);
		~CDateTime();
	public:
		const	CDateTime	&operator=(const CDateTime &dt);
		bool				operator==(const CDateTime &dt)	const;
		bool				operator!=(const CDateTime &dt)	const;
		bool				operator<(const CDateTime &dt)	const;
		bool				operator>(const CDateTime &dt)	const;
		bool				operator<=(const CDateTime &dt)	const;
		bool				operator>=(const CDateTime &dt)	const;
	public:
		inline	long		weekday()	const;

		time_t				toTimeT()	const;
		ACE_Time_Value		toTimeValue()	const;
		void				toNDateTime(nDateTime& dt)	const;
	public:
		CString toString(const CString &format) const;
		static	CDateTime fromString(const CString &dtString,const CString &df);
		static  long daysInMonth(long y,long m);
		static  long daysInYear(long y);
		static	CDateTime currentDateTime();	//yhh20090715
		static  void currentDateTime(nDateTime& dt);
	};
}
#endif

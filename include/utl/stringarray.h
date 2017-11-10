/*==============================================================================
* 文件名称 : stringarraiy.h
* 模块 : 字符串数组类
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : v1.1
* 修改摘要 : 
* 修改者 :  尹宏昊
* 修改日期 : 2008年5月6日
==============================================================================*/
#ifndef _PDT_UTL_STRINGARRAY_H_
#define _PDT_UTL_STRINGARRAY_H_

#include <ace/Vector_T.h>
#include "utl/utl_export.h"
#include "utl/string.h"
//============================================================================
namespace PDT
{
	class	UTL_EXPORT	CStringArray	:	public	ACE_Vector<CString>
	{
	public:
		CStringArray(size_t size = 0);
		CStringArray(const CStringArray &stringArray);
	public:
		const	CStringArray	&operator=(const CStringArray &stringArray);
	public:
		const	CStringArray	&append(const CString &str);
	};
}
//============================================================================
#endif


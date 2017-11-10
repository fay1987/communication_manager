/*==============================================================================
* �ļ����� : stringarraiy.h
* ģ�� : �ַ���������
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : v1.1
* �޸�ժҪ : 
* �޸��� :  �����
* �޸����� : 2008��5��6��
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


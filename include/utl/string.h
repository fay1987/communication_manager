/*==============================================================================
* �ļ����� : string.h
* ģ�� : �ַ�����
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : v1.1
* �޸�ժҪ : trim��trimLeft��trimRight�����ж�ȥ���ұߵĵ�ָ���ַ��Ĵ������(pRightָ��Ӧ�ݼ�)
* �޸��� :  �����
* �޸����� : 2008��5��6��
==============================================================================*/
#ifndef _PDT_UTL_STRING_H_
#define _PDT_UTL_STRING_H_

#include <ace/SString.h>
#include "utl/utl_export.h"
#include "utl/datatype.h"

#if defined(WIN32)
#define	strtoint64				_strtoi64
#define	strtouint64				_strtoui64
#define	int64tostr				_i64toa
#define	uint64tostr				_ui64toa
#else
#define	strtoint64				strtoll
#define	strtouint64				strtoull
UTL_EXPORT	const	char	*int64tostr(hInt64 value,char *str,int base=10);
UTL_EXPORT	const	char	*uint64tostr(hUInt64 value,char *str,int base=10);
#endif

namespace PDT
{
	class	UTL_EXPORT	CString : public ACE_TString
	{
	public:
		CString(ACE_Allocator *the_allocator = 0);
		CString(const hChar *s,ACE_Allocator *the_allocator = 0,bool release = true);
		CString(const hChar *s,size_type len,ACE_Allocator *the_allocator = 0,bool release = true);
		CString(const ACE_TString &s);
		CString(const CString &s);
		CString(hChar c, ACE_Allocator *the_allocator = 0);
		CString(size_type len,hChar c = 0,ACE_Allocator *the_allocator = 0);
		virtual	~CString ();
	public:
		//����ֵ
		hChar		toChar() const;
		hUChar		toUChar() const;
		hInt8		toInt8() const;
		hUInt8		toUInt8() const;
		hInt16		toInt16() const;
		hUInt16		toUInt16() const;
		hInt32		toInt32() const;
		hUInt32		toUInt32() const;
		hInt64		toInt64(int base=10) const;
		hUInt64		toUInt64(int base=10) const;
		hFloat		toFloat() const;
		hDouble		toDouble() const;
		//����ֵ
		const	CString	&from(hChar value);
		const	CString	&from(hUInt8 value,int base=10);
		const	CString	&from(hInt16 value,int base=10);
		const	CString	&from(hUInt16 value,int base=10);
		const	CString	&from(hInt32 value,int base=10);
		const	CString	&from(hUInt32 value,int base=10);
		const	CString	&from(hInt64 value,int base=10);
		const	CString	&from(hUInt64 value,int base=10);
		const	CString	&from(hFloat value,int precision=8);
		const	CString	&from(hDouble value,int precision=8);
		//�ض�
		const	CString	&trim(hChar c=' ');
		const	CString	&trimLeft(hChar c=' ');
		const	CString	&trimRight(hChar c=' ');
		//���
		CString			toUpper()	const;
		CString			toLower()	const;
		//ת��
		const	CString	&asUpper();
		const	CString	&asLower();
		//�Ƚ�
		int				iCompare(const CString &str)	const;
		//�滻
		CString			&replace(const CString &src,const CString &dec,size_type pos=0);
		//����
		int				caseFind(const CString &str,int pos=0)	const;		//�����ִ�Сд
	public:
		static	const	CString	null;
	};

}
//============================================================================

#endif

/*==============================================================================
* �ļ����� : variant.h
* ģ�� : ͨ�ñ���
* ����ʱ�� : 2013-11-25 15:55:23
* ���� : 
* �汾 : v1.0

* �޸ĺ�汾 : v1.1
* �޸�ժҪ : 
* �޸��� :  wlq
* �޸����� : 2013��11��25��
==============================================================================*/
#ifndef	_PDT_DATABASE_VARIANT_H_
#define	_PDT_DATABASE_VARIANT_H_

#include <ace/Array_Base.h>
#include "utl/utl_export.h"
#include "utl/bytearray.h"
#include "utl/datatype.h"
#include "utl/string.h"
#include "utl/datetime.h"

namespace PDT
{
	class UTL_EXPORT CVariant
	{
	public:
		enum	Type
		{
			Null = 0,
			Invalid,
			Int8,
			UInt8,
			Int16,
			UInt16,
			Int32,
			UInt32,
			Int64,
			UInt64,
			Float,
			Double,
			String,
			DateTime,
			Date,
			Time,
			ByteArray
		};
	public:
		CVariant();
		virtual	~CVariant();
	public:
		CVariant(const CVariant &value);
		CVariant(hInt8 value);
		CVariant(hUInt8 value);
		CVariant(hInt16 value);
		CVariant(hUInt16 value);
		CVariant(hInt32 value);
		CVariant(hUInt32 value);
		CVariant(hInt64 value);
		CVariant(hUInt64 value);
		CVariant(hFloat value);
		CVariant(hDouble value);
		CVariant(const char *value);
		CVariant(const CString &value);
		CVariant(const CDateTime &value);
		CVariant(const CByteArray &value);
	public:
		//��ֵ
		const CVariant	&operator=(const CVariant &var);		
		const CVariant  &operator=(hInt8 value);
		const CVariant  &operator=(hUInt8 value);
		const CVariant  &operator=(hInt16 value);
		const CVariant	&operator=(hUInt16 value); 
		const CVariant	&operator=(hInt32 value);
		const CVariant	&operator=(hUInt32 value);
		const CVariant	&operator=(hInt64 value);
		const CVariant	&operator=(hUInt64 value);
		const CVariant	&operator=(hFloat value); 
		const CVariant	&operator=(hDouble value);
		const CVariant	&operator=(const char* value);
		const CVariant	&operator=(const CString &value); 
		const CVariant	&operator=(const CDateTime &value); 
		const CVariant	&operator=(const CByteArray &value); 
		//����
		bool			operator==(const CVariant &var)	const;
		bool			operator==(hInt8 value) const;
		bool			operator==(hUInt8 value) const;
		bool			operator==(hInt16 value) const;
		bool			operator==(hUInt16 value) const; 
		bool			operator==(hInt32 value) const;
		bool			operator==(hUInt32 value) const;
		bool			operator==(hInt64 value) const;
		bool			operator==(hUInt64 value) const;
		bool			operator==(hFloat value) const; 
		bool			operator==(hDouble value) const;
		bool			operator==(const char* value) const;
		bool			operator==(const CString &value) const; 
		bool			operator==(const CDateTime &value) const; 
		bool			operator==(const CByteArray &value) const; 
		//������
		bool			operator!=(const CVariant &value)	const;
		bool			operator!=(hInt8 value) const;
		bool			operator!=(hUInt8 value) const;
		bool			operator!=(hInt16 value) const;
		bool			operator!=(hUInt16 value) const; 
		bool			operator!=(hInt32 value) const;
		bool			operator!=(hUInt32 value) const;
		bool			operator!=(hInt64 value) const;
		bool			operator!=(hUInt64 value) const;
		bool			operator!=(hFloat value) const; 
		bool			operator!=(hDouble value) const;
		bool			operator!=(const char* value) const;
		bool			operator!=(const CString &value) const; 
		bool			operator!=(const CDateTime &value) const; 
		bool			operator!=(const CByteArray &value) const; 
		//ת�����Ͳ��������ת���������ͣ�
		hChar				toChar() const;
		hInt8				toInt8() const;
		hUInt8				toUInt8() const;
		hInt16				toInt16() const;
		hUInt16				toUInt16() const;
		hInt32				toInt32() const;
		hUInt32				toUInt32() const;
		hInt64				toInt64() const;
		hUInt64				toUInt64() const;
		hFloat				toFloat() const;
		hDouble				toDouble() const;
		CString				toString(int baseOrPrecision=10) const;		//���ƣ����Σ������ȣ����㣩
		CDateTime			toDateTime(const CString df=DT_FMT_DEF)	const;
		CByteArray			toByteArray() const;
		//ת�����Ͳ������ת���������ͣ�
		const	hInt8		&asInt8();
		const	hUInt8		&asUInt8();
		const	hInt16		&asInt16();
		const	hUInt16		&asUInt16();
		const	hInt32		&asInt32();
		const	hUInt32		&asUInt32();
		const	hInt64		&asInt64();
		const	hUInt64		&asUInt64();
		const	hFloat		&asFloat();
		const	hDouble		&asDouble();
		const	CString		&asString(int baseOrPrecision=10);			//���ƣ����Σ������ȣ����㣩
		const	CDateTime	&asDateTime(const CString df=DT_FMT_DEF);
		const	CByteArray	&asByteArray();
		//���ض������ã�ֱ�ӷ��أ����ж϶���ָ���Ƿ���ڣ�
		hInt8				&int8Value();
		hUInt8				&uint8Value();
		hInt16				&int16Value();
		hUInt16				&uint16Value();
		hInt32				&int32Value();
		hUInt32				&uint32Value();
		hInt64				&int64Value();
		hUInt64				&uint64Value();
		hFloat				&floatValue();
		hDouble				&doubleValue();
		CString				&stringValue();
		CDateTime			&dateTimeValue();
		CByteArray			&byteArrayValue();
		const	hChar		&charValue()	const;
		const	hUChar		&ucharValue()	const;
		const	hInt8		&int8Value()	const;
		const	hUInt8		&uint8Value()	const;
		const	hInt16		&int16Value()	const;
		const	hUInt16		&uint16Value()	const;
		const	hInt32		&int32Value()	const;
		const	hUInt32		&uint32Value()	const;
		const	hInt64		&int64Value()	const;
		const	hUInt64		&uint64Value()	const;
		const	hFloat		&floatValue()	const;
		const	hDouble		&doubleValue()	const;
		const	CString		&stringValue()	const;
		const	CDateTime	&dateTimeValue()	const;
		const	CByteArray	&byteArrayValue()	const;
	public:
		bool				isNull()	const;
		bool				isValid()	const;
		void				setInvalid();
		Type				type()	const;
		const char *		typeName()	const;
		bool				canCast(Type type)	const;
		bool				cast(Type type);
	public:
		static	Type			nameToType(const char *name);
		static	const char *	typeToName(Type type);
	public:
		static CVariant			null;  //�ն���
	private:
		void				init();
		void				clear();

		void				setType(Type type);
	private:
		Type			m_type;
		union
		{
			hInt8		int8Value;
			hUInt8		uint8Value;
			hInt16		int16Value;
			hUInt16		uint16Value;
			hInt32		int32Value;
			hUInt32		uint32Value;
			hInt64		int64Value;
			hUInt64		uint64Value;
			hFloat		floatValue;
			hDouble		doubleValue;
			void*		ptr;
		}m_value;
	};
};
//---------------------------------------------------------------------------------------

#endif

/*==============================================================================
* �ļ����� : datatype.h
* ģ�� : �������ͣ����������磩
* ����ʱ�� : 2009-07-28 15:55:23
* ���� : �����
* �汾 : v1.0
* ˵�� : ����Ĭ��ʹ��little endbian(ͬwindows&linux)��hInt32--����ϵͳ����;nInt32--��ϵͳĬ������(little),�������紫������

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 2009��7��25��
==============================================================================*/
#ifndef	_PDT_UTL_DATATYPE_H_
#define	_PDT_UTL_DATATYPE_H_

#include <ace/Basic_Types.h>
//--------------------------------������������-------------------------------------------------------
typedef	ACE_Byte		hBool;
typedef	ACE_TCHAR		hChar;
typedef	ACE_Byte		hUChar;
typedef	ACE_INT8		hInt8;
typedef	ACE_UINT8		hUInt8;
typedef	ACE_INT16		hInt16;
typedef	ACE_UINT16		hUInt16;
typedef	ACE_INT32		hInt32;
typedef	ACE_UINT32		hUInt32;
typedef	ACE_INT64		hInt64;
typedef	ACE_UINT64		hUInt64;
typedef	float			hFloat;
typedef	double			hDouble;
//--------------------------------������������-------------------------------------------------------
//�ֽ���
#define	SWAP_BYTE_2(X)	((((X)&0x00FF)<<8)|(((X)&0xFF00)>>8))
#define	SWAP_BYTE_4(X)	((SWAP_BYTE_2((X)&0xFFFF)<<16)|SWAP_BYTE_2(((X)>>16)&0xFFFF))
#define	SWAP_BYTE_8(X)	((SWAP_BYTE_4((X)&0xFFFFFFFF)<<32)|SWAP_BYTE_4(((X)>>32)&0xFFFFFFFF))

#if defined(ACE_BIG_ENDIAN)		//����0x0123
#define	HL_BYTE_2(X)	SWAP_BYTE_2(X)
#define	HL_BYTE_4(X)	SWAP_BYTE_4(X)
#define	HL_BYTE_8(X)	SWAP_BYTE_8(X)
#define	LH_BYTE_2(X)	X
#define	LH_BYTE_4(X)	X
#define	LH_BYTE_8(X)	X
#define	HOST_TO_NET_BYTE_2(X)	SWAP_BYTE_2(X)
#define	HOST_TO_NET_BYTE_4(X)	SWAP_BYTE_4(X)
#define	HOST_TO_NET_BYTE_8(X)	SWAP_BYTE_8(X)
#define	NET_TO_HOST_BYTE_2(X)	SWAP_BYTE_2(X)
#define	NET_TO_HOST_BYTE_4(X)	SWAP_BYTE_4(X)
#define	NET_TO_HOST_BYTE_8(X)	SWAP_BYTE_8(X)
#else
#define	HL_BYTE_2(X)	X
#define	HL_BYTE_4(X)	X
#define	HL_BYTE_8(X)	X
#define	LH_BYTE_2(X)	SWAP_BYTE_2(X)
#define	LH_BYTE_4(X)	SWAP_BYTE_4(X)
#define	LH_BYTE_8(X)	SWAP_BYTE_8(X)
#define	HOST_TO_NET_BYTE_2(X)	X
#define	HOST_TO_NET_BYTE_4(X)	X
#define	HOST_TO_NET_BYTE_8(X)	X
#define	NET_TO_HOST_BYTE_2(X)	X
#define	NET_TO_HOST_BYTE_4(X)	X
#define	NET_TO_HOST_BYTE_8(X)	X
#endif

//���ò�����
# define PDT_EVEN(NUM) (((NUM) & 1) == 0)					//ż��У��
# define PDT_ODD(NUM) (((NUM) & 1) == 1)					//����У��
# define PDT_BIT_ENABLED(VAL, BIT) (((VAL) & (BIT)) != 0)	//�ж�ĳλ�Ƿ���Ч
# define PDT_BIT_DISABLED(VAL, BIT) (((VAL) & (BIT)) == 0)	//�ж�ĳλ�Ƿ���Ч
# define PDT_SET_BITS(VAL, BITS) (VAL |= (BITS))			//����ĳλ
# define PDT_CLR_BITS(VAL, BITS) (VAL &= ~(BITS))			//���ĳλ

//�жϸ����������Ƿ�Ϊ��
#define	PDT_ISZERO(VAL)			((VAL<0.000001) && (VAL>-0.000001))

#ifndef MAX
#define MAX(X,Y)		(((X)>(Y))?(X):(Y))
#endif
#ifndef MIN
#define MIN(X,Y)		(((X)<(Y))?(X):(Y))
#endif

#ifdef	__unix
#define MAKEWORD(a, b)	((unsigned short)(((unsigned char)((unsigned short)(a) & 0xFF)) | ((unsigned short)((unsigned char)((unsigned short)(b) & 0xFF))) << 8))
#define MAKELONG(a, b)	((unsigned int)(((unsigned short)((unsigned int)(a) & 0xFFFF)) | ((unsigned int)((unsigned short)((unsigned int)(b) & 0xFFFF))) << 16))
#define LOBYTE(w)		((unsigned char)(w))
#define HIBYTE(w)		((unsigned char)((unsigned short)(w) >> 8))
#define LOWORD(l)		((unsigned short)(l))
#define HIWORD(l)		((unsigned short)((unsigned int)(l) >> 16)) 
#endif

#define MakeWord(a,b)	((unsigned short)((unsigned char)(a) + (unsigned char)(b) * 256))
#define MakeLong(a,b)	((unsigned int)((unsigned short)(a) + (unsigned short)(b) * 65536))
#define LoByte(w)		((unsigned char)((unsigned short)(w) % 256))
#define HiByte(w)		((unsigned char)((unsigned short)(w) / 256))
#define LoWord(l)		((unsigned short)((unsigned int)(l) % 65536))
#define HiWord(l)		((unsigned short)((unsigned int)(l) / 65536))

//����������������ģ��
template<class T,int size>
class	nIntegerDataType
{
public:
	nIntegerDataType()																				{m_value = 0;}
	nIntegerDataType(const nIntegerDataType<T,size> &val)											{setValue(val);}
	nIntegerDataType(const T &val)																	{setValue(val);}
	//���������
	inline	operator							T()	const											{return	hostValue();}
	inline	const	nIntegerDataType<T,size>	&operator=(const nIntegerDataType<T,size> &val)		{return	setValue(val);}
	inline	const	nIntegerDataType<T,size>	&operator=(const T &val)							{return	setValue(val);}
	inline	const	nIntegerDataType<T,size>	&operator+=(const nIntegerDataType<T,size> &val)	{return	setValue(hostValue()+val.hostValue());}
	inline	const	nIntegerDataType<T,size>	&operator+=(const T &val)							{return	setValue(hostValue()+val);}
	inline	const	nIntegerDataType<T,size>	&operator-=(const nIntegerDataType<T,size> &val)	{return	setValue(hostValue()-val.hostValue());}
	inline	const	nIntegerDataType<T,size>	&operator-=(const T &val)							{return	setValue(hostValue()-val);}
	inline	const	nIntegerDataType<T,size>	&operator*=(const nIntegerDataType<T,size> &val)	{return	setValue(hostValue()*val.hostValue());}
	inline	const	nIntegerDataType<T,size>	&operator*=(const T &val)							{return	setValue(hostValue()*val);}
	inline	const	nIntegerDataType<T,size>	&operator/=(const nIntegerDataType<T,size> &val)	{return	setValue(hostValue()/val.hostValue());}
	inline	const	nIntegerDataType<T,size>	&operator/=(const T &val)							{return	setValue(hostValue()/val);}
	inline	const	nIntegerDataType<T,size>	&operator++()										{return	setValue(hostValue()+1);}
	inline	const	nIntegerDataType<T,size>	&operator--()										{return	setValue(hostValue()-1);}
	inline	nIntegerDataType<T,size>			operator++(int)										{nIntegerDataType<T,size>	temp(*this);	++*this;	return	temp;}
	inline	nIntegerDataType<T,size>			operator--(int)										{nIntegerDataType<T,size>	temp(*this);	--*this;	return	temp;}
	//��ȡ
	inline	const	T							&netValue()	const									{return	m_value;}
	inline	T									hostValue()	const
	{
		switch(size)
		{
		case	2:
			return	NET_TO_HOST_BYTE_2(m_value);
		case	4:
			return	NET_TO_HOST_BYTE_4(m_value);
		case	8:
			return	NET_TO_HOST_BYTE_8(m_value);
		}
		return	0;
	}
	//д��
	inline	const	nIntegerDataType<T,size>	&setValue(const nIntegerDataType<T,size> &val)		{m_value = val.netValue();	return	*this;}
	inline	const	nIntegerDataType<T,size>	&setValue(const T &val)
	{
		switch(size)
		{
		case	2:
			m_value = HOST_TO_NET_BYTE_2(val);
			break;
		case	4:
			m_value = HOST_TO_NET_BYTE_4(val);
			break;
		case	8:
			m_value = HOST_TO_NET_BYTE_8(val);
			break;
		default:
			m_value = 0;
			break;
		}
		return	*this;
	}
private:
	T	m_value;
};

//8λ�ֽ�
typedef	struct
{
#if defined(ACE_BIG_ENDIAN)
	unsigned char b7:1;
	unsigned char b6:1;
	unsigned char b5:1;
	unsigned char b4:1;
	unsigned char b3:1;
	unsigned char b2:1;
	unsigned char b1:1;
	unsigned char b0:1;
#else
	unsigned char b0:1;
	unsigned char b1:1;
	unsigned char b2:1;
	unsigned char b3:1;
	unsigned char b4:1;
	unsigned char b5:1;
	unsigned char b6:1;
	unsigned char b7:1;
#endif
}nBit8;

//��������
typedef	hUInt8								nBool;
typedef	hChar								nChar;
typedef	hUChar								nUChar;
typedef	hInt8								nInt8;
typedef	hUInt8								nUInt8;
typedef	nIntegerDataType<hInt16,2>			nInt16;
typedef	nIntegerDataType<hUInt16,2>			nUInt16;
typedef	nIntegerDataType<hInt32,4>			nInt32;
typedef	nIntegerDataType<hUInt32,4>			nUInt32;
typedef	nIntegerDataType<hInt32,8>			nInt64;
typedef	nIntegerDataType<hUInt32,8>			nUInt64;
//32λ�����ȸ�����
class	nFloat
{
	typedef	hFloat	T;
public:
	nFloat()													{m_value.value = 0;}
	nFloat(const nFloat &val)									{setValue(val);}
	nFloat(const T &val)										{setValue(val);}
	//���������
	inline	operator			T()	const						{return	hostValue();}
	inline	const	nFloat	&operator=(const nFloat &val)		{return	setValue(val);}
	inline	const	nFloat	&operator=(const T &val)			{return	setValue(val);}
	inline	const	nFloat	&operator+=(const nFloat &val)		{return	setValue(hostValue()+val.hostValue());}
	inline	const	nFloat	&operator+=(const T &val)			{return	setValue(hostValue()+val);}
	inline	const	nFloat	&operator-=(const nFloat &val)		{return	setValue(hostValue()-val.hostValue());}
	inline	const	nFloat	&operator-=(const T &val)			{return	setValue(hostValue()-val);}
	inline	const	nFloat	&operator*=(const nFloat &val)		{return	setValue(hostValue()*val.hostValue());}
	inline	const	nFloat	&operator*=(const T &val)			{return	setValue(hostValue()*val);}
	inline	const	nFloat	&operator/=(const nFloat &val)		{return	setValue(hostValue()/val.hostValue());}
	inline	const	nFloat	&operator/=(const T &val)			{return	setValue(hostValue()/val);}
	inline	const	nFloat	&operator++()						{return	setValue(hostValue()+1);}
	inline	const	nFloat	&operator--()						{return	setValue(hostValue()-1);}
	inline	nFloat			operator++(int)						{nFloat	temp(*this);	++*this;	return	temp;}
	inline	nFloat			operator--(int)						{nFloat	temp(*this);	--*this;	return	temp;}
	//��ȡ(��������ֵ--ʹ��ϵͳ�ֽ���(little)�����������ֽ���(big))
	inline	const	T		&netValue()	const					{return	m_value.value;}
	inline	T				hostValue()	const
	{
#if defined(ACE_BIG_ENDIAN)
		_t_byte_	temp;
		temp.byte.b0 = m_value.byte.b3;
		temp.byte.b1 = m_value.byte.b2;
		temp.byte.b2 = m_value.byte.b1;
		temp.byte.b3 = m_value.byte.b0;
		return	temp.value;
#else
		return	m_value.value;
#endif
	}
	//д��
	inline	const	nFloat	&setValue(const nFloat &val)		{m_value.value = val.netValue();	return	*this;}
	inline	const	nFloat	&setValue(const T &val)
	{
#if defined(ACE_BIG_ENDIAN)
		_t_byte_	temp;
		temp.value = val;
		m_value.byte.b0 = temp.byte.b3;
		m_value.byte.b1 = temp.byte.b2;
		m_value.byte.b2 = temp.byte.b1;
		m_value.byte.b3 = temp.byte.b0;
#else
		m_value.value = val;
#endif
		return	*this;
	}
private:
	union	_t_byte_
	{
		T	value;
		struct
		{
			hUChar	b0;
			hUChar	b1;
			hUChar	b2;
			hUChar	b3;
		}byte;
	};
	_t_byte_	m_value;
};
//64λ˫���ȸ�����
class	nDouble
{
	typedef	hDouble	T;
public:
	nDouble()													{m_value.value = 0;}
	nDouble(const nDouble &val)									{setValue(val);}
	nDouble(const T &val)										{setValue(val);}
	//���������
	inline	operator		T()	const							{return	hostValue();}
	inline	const	nDouble	&operator=(const nDouble &val)		{return	setValue(val);}
	inline	const	nDouble	&operator=(const T &val)			{return	setValue(val);}
	inline	const	nDouble	&operator+=(const nDouble &val)		{return	setValue(hostValue()+val.hostValue());}
	inline	const	nDouble	&operator+=(const T &val)			{return	setValue(hostValue()+val);}
	inline	const	nDouble	&operator-=(const nDouble &val)		{return	setValue(hostValue()-val.hostValue());}
	inline	const	nDouble	&operator-=(const T &val)			{return	setValue(hostValue()-val);}
	inline	const	nDouble	&operator*=(const nDouble &val)		{return	setValue(hostValue()*val.hostValue());}
	inline	const	nDouble	&operator*=(const T &val)			{return	setValue(hostValue()*val);}
	inline	const	nDouble	&operator/=(const nDouble &val)		{return	setValue(hostValue()/val.hostValue());}
	inline	const	nDouble	&operator/=(const T &val)			{return	setValue(hostValue()/val);}
	inline	const	nDouble	&operator++()						{return	setValue(hostValue()+1);}
	inline	const	nDouble	&operator--()						{return	setValue(hostValue()-1);}
	inline	nDouble			operator++(int)						{nDouble	temp(*this);	++*this;	return	temp;}
	inline	nDouble			operator--(int)						{nDouble	temp(*this);	--*this;	return	temp;}
	//��ȡ
	inline	const	T		&netValue()	const					{return	m_value.value;}
	inline	T				hostValue()	const
	{
#if defined(ACE_BIG_ENDIAN)
		_t_byte_	temp;
		temp.byte.b0 = m_value.byte.b7;
		temp.byte.b1 = m_value.byte.b6;
		temp.byte.b2 = m_value.byte.b5;
		temp.byte.b3 = m_value.byte.b4;
		temp.byte.b4 = m_value.byte.b3;
		temp.byte.b5 = m_value.byte.b2;
		temp.byte.b6 = m_value.byte.b1;
		temp.byte.b7 = m_value.byte.b0;
		return	temp.value;
#else
		return	m_value.value;
#endif
	}
	//д��
	inline	const	nDouble	&setValue(const nDouble &val)		{m_value.value = val.netValue();	return	*this;}
	inline	const	nDouble	&setValue(const T &val)
	{
#if defined(ACE_BIG_ENDIAN)
		_t_byte_	temp;
		temp.value = val;
		m_value.byte.b0 = temp.byte.b7;
		m_value.byte.b1 = temp.byte.b6;
		m_value.byte.b2 = temp.byte.b5;
		m_value.byte.b3 = temp.byte.b4;
		m_value.byte.b4 = temp.byte.b3;
		m_value.byte.b5 = temp.byte.b2;
		m_value.byte.b6 = temp.byte.b1;
		m_value.byte.b7 = temp.byte.b0;
#else
		m_value.value = val;
#endif
		return	*this;
	}
private:
	union	_t_byte_
	{
		T	value;
		struct
		{
			hUChar	b0;
			hUChar	b1;
			hUChar	b2;
			hUChar	b3;
			hUChar	b4;
			hUChar	b5;
			hUChar	b6;
			hUChar	b7;
		}byte;
	};
	_t_byte_	m_value;
};
//����ʱ��
typedef	struct
{
	nUInt16		year;
	nUInt8		mon;
	nUInt8		day;
	nUInt8		hour;
	nUInt8		min;
	nUInt8		sec;
	nUInt16		msec;
	nUInt8		wday;
	nUInt16		yday;
}nDateTime;

#endif

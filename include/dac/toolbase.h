/*==============================================================================
* �ļ����� : toolbase.h
* �ļ����� ������һЩ����������
* ģ�� : ǰ�òɼ��ӿ�
* ����ʱ�� : 2009-07-13
* ���� : lj
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef _PDT_DAC_TOOLBASE_H_
#define _PDT_DAC_TOOLBASE_H_

#include "utl/datatype.h"
#include <ace/config-all.h>
#include <string>

//=============================================================================
#if defined (TOOL_LIB)
#  define	TOOL_EXPORT	ACE_Proper_Export_Flag
#else
#  define	TOOL_EXPORT	ACE_Proper_Import_Flag
#endif	// TOOL_LIB
//=============================================================================

namespace PDT
{
	using namespace std;

	class TOOL_EXPORT CToolFunctions
	{
	public:
			static char char2Hex(char ch);
			/*
			function:���������ݳ��Ƚ���У��
			input:nLen:�������ݳ���
			return:У����
			writer:wwg,2007-8-14
			*/
			static hUInt16 getLCheckSum(int nLen);

			/*
			function:����У��
			input:unsigned char:Դ�ַ���len��У���ַ�����
			return:У����
			writer:wlq,2010-10-25
			*/
			static hUChar checkXor(hUChar *input,int len);

			/*
			function:��unsigned char λ��ת����bit0-bit7��bit1-bit6��bit2-bit5��bit3-bit4����λ��
			input:unsigned char:Դ�ַ�
			return:����֮����ַ�
			writer:wlq,2010-10-25
			*/
			static hUChar bitExchange(hUChar input);
			

			/*
			function:���������ݳ��Ƚ��к�У��
			intput:p:��ҪУ�������;nOffSet:ƫ����;nLen:����;���صľ����������ݳ���
			return:У��ֵ���
			writer:2007-8-14
			*/
			static hUInt16 checkLSum(hUChar *p,int nOffSet,int nLen,int &nDataLength);
			
			/*
			function:����У����
			input:����������;nOffSet:��ʼ����λ��;nLen:���ĳ���
			return:���غ�У����
			writer:wwg,2007-8-14
			*/
			static hUInt16 checkSum(hUChar *p,int nOffSet,int nLen);
			

			/*
			function:����У����
			input:����������;nOffSet:��ʼ����λ��;nLen:���ĳ���
			return:���غ�У����
			writer:wwg,2007-8-14
			*/
			static hUInt16 checkSumEx(hUChar *p,int nOffset,int nLen);
			

			/*
			function:��ȡ��У����
			input:p:����������;nOffSet:ƫ����;nLen:�������ݳ���
			return:��У����ֵ
			writer:wwg,2007-8-14
			*/
			static hUInt16 getCheckSum(hUChar *p,int nOffSet,int nLen);
			

			/*
			function:��16��������ת�����ַ���
			input:pStr:�洢�ַ������ڴ�;nBufferSize:�����С;pData:ԭ����;nDataLen:ԭ���ݳ���
			return:ת��֮��ĳ���
			writer:wwg,2007-8-14
			*/
			static int hex2String(char *pStr,int nBuffSize,int nOffSet,hUChar *pData,hUInt32 nDataLen);
			

			/*
			function:stringת���� hex
			input:char:Դ�ַ���
			return:ת��֮��ĳ���
			writer:wwg,2007-8-27
			*/

			static int string2Hex(char *pStr, hUChar *pData);
			

			/*
			function:�ҳ��ַ����ַ����г��ֵ�λ��
			input:pSrc:Դ�ַ���;offset:ƫ����;nLen:�ַ�����;pdest:Ŀ���ַ�
			return:λ��
			writer:wwg,2007-9-13
			*/
			static int midChar(char *pSrc,int offset,int nlen,char pdest);
				

			/*
			function:��4�ֽ�ʮ�����Ʊ�ʾ�ĸ�����ת���ɸ�����
			input:nLen:��ת��������;nOffSet:ƫ����
			return:ת����ĸ�����
			writer:tyw,2007-9-7
			*/
			static float convertByteDouble(hUChar *p,int nOffSet);
			

			/*
			function:��Hexת���� Ascii
			input:pSrc:Դ�ַ���;offset:ƫ����;nLen:����;pDest:Ŀ��
			return:ת��֮��ĳ���
			writer:wwg,2007-9-17
			*/
			static int hex2Ascii(hUChar *pSrc,int offset,int nLen,hUChar *pDest);
			

			/*
			function:��ascii��ת����hex
			input:pSrc:Դ�ַ���;offset:ƫ����;nLen:Դ������;pDest:Ŀ�괮
			return:32 31 31 30 45 44 34 33 44 30 31 32 46 46 30 31 46 46 46 46 46 46 30 31 46 46 46 46 46 46 46 38 44 46
			*/
			static int ascii2Hex(hUChar *pSrc,int offset,int nLen,hUChar *pDest);
				

			/*
			function:��hexת����char
			input:pDest:Ŀ��;pSrc:Դ����;offset:λ��;nLen;Դ�������ݳ���
			return:void
			writer:wwg,2007-10-11
			*/
			static void hex2Char(char *pDest,hUChar *pSrc,int offset,int nLen);
			

			/*
			function:����С��λ��������ת����С��
			input:percentage:С��λ��;src:Դ����
			return:����ת��֮���ֵ
			writer:Wwg,2007-9-19
			*/
			static float int2Float(int percentage,int src);
			

			/*
			function:��������ת����bcd��
			input:Դ����
			return:bcd�����
			writer:wwg,2007-10-12
			*/
			static int int2BCD(int nsrc);
			

			static int bcd2Int(hUChar bTemp);
			

			/*
			function:��bcd��ת��������
			input:Դ����
			return:������
			write:wwg,2007-10-13
			*/
			static int bcd2Int(int nsrc);

			/*
			function:��bin��ת����bcd��
			input:Դ���ݣ������ָ���ַ
			return:��
			write:
			*/			
			static void	bcd2bin1(unsigned char v1,unsigned char *v2);


			/*
			function:��bin��ת����bcd��
			input:Դ���ݣ������ָ���ַ
			return:��
			write:
			*/			
			static void	bin2bcd2(unsigned short v1,unsigned short *v2);


			/*
			function:��bcd��ת����bin��
			input:Դ���ݣ������ָ���ַ
			return:��
			write:
			*/			
			static void	bcd2bin2(unsigned short v1,unsigned short *v2);


			/*
			function:��bcd��ת�����޷�������
			input:Դ���ݣ������ָ���ַ
			return:������
			write:
			*/
			static int		bcd4bin4(unsigned int	v1,unsigned int *v2);

			

			/*
			function:����CRC16У��
			input:��ҪУ�������;len:У�����ݵĳ���
			return:У����
			writer:wwg,2007-11-08
			*/
			static hUInt16 modbusCRC(hUChar * pData, int len);
			

			/*
			Descritpion:��ȡ���ڵ�У��λ������λ��ֹͣλ
			Arguments:verify:У��λ;databit:����λ;stopbit:ֹͣλ
			Return:������
			Author:wwg,2008-5-7
			*/
			static hUChar getSerialParam(char verify,int databit,int stopbit);
			

			/*
			*----����CDTУ��
			bch,У����ֵ
			p,У��������ʼָ��
			*/
			static hUInt16 makeCRC(hUChar *p, int offset, int crc_len);

			static void MakeBch (hUChar *bch,hUChar * p);

			/*
				*----����CDTУ��
				len,У���ֽڳ���
				crc,У����ֵ
				p,У��������ʼָ��
			*/
			static void MakeCRC8(short len,hUChar *crc,hUChar * p);
				

			/*
			Routine:��׼��CDTУ��
			*/
			static hUChar cdtCheck(hUChar *pBuff,int len);
			


			//************************************************************************************
			// *���������� : ascii2Char
			// *  ��    �� : ascii2Char
			// *  ��ڲ��� : pBuff ���ݴ洢����len ���ݳ���
			// *  �� �� ֵ : strTemp ת���������	
			// *  ��    ע : 
			//************************************************************************************
			static string ascii2Char(hUChar  *pBuff, int len);


			//************************************************************************************
			// *���������� : bufferMove
			// *  ��    �� : ������£�����һ������move��head
			// *  ��ڲ��� : buffer:���� ��buffer_size:buffer�Ĵ�С�� offset:��ʼλ�ã� count:�ַ�����
			// *  �� �� ֵ : 
			// *  ��    ע : 
			//************************************************************************************
			static hUChar* bufferMove(hUChar *buffer, int buffer_size, int offset,int count );



			/*
			* ����˵��������uchar ���͵����ݴ�
			* �������أ�-1:û�ҵ���>=0:�ҵ���λ��
			*/
			//************************************************************************************
			// *���������� : findData
			// *  ��    �� : ����˵��������uchar ���͵����ݴ�
			// *  ��ڲ��� : trgData:Դ�ַ����� srgLeng:Դ�ַ������� �� srcdata,�����ַ����� srcLen ,�ַ�������
			// *  �� �� ֵ : 
			// *  ��    ע : 
			//************************************************************************************
			static int findData(hUChar* trgData,int trgLen,hUChar* srcData,int srcLen);


			//************************************************************************************
			// *���������� : floatToInt
			// *  ��    �� : ����˵�����������뽫floatתΪi
			// *  ��ڲ��� : hFloat:����ĸ���ֵ
			// *  �� �� ֵ : ת���������ֵ
			// *  ��    ע : 
			//************************************************************************************
			static int floatToInt(hFloat val);
		};
}

#endif // _PDT_DAC_TOOLBASE_H_

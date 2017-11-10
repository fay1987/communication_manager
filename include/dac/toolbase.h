/*==============================================================================
* 文件名称 : toolbase.h
* 文件功能 ：基本一些函数基本类
* 模块 : 前置采集接口
* 创建时间 : 2009-07-13
* 作者 : lj
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
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
			function:对数据内容长度进行校验
			input:nLen:数据内容长度
			return:校验结果
			writer:wwg,2007-8-14
			*/
			static hUInt16 getLCheckSum(int nLen);

			/*
			function:异或和校验
			input:unsigned char:源字符，len，校验字符长度
			return:校验结果
			writer:wlq,2010-10-25
			*/
			static hUChar checkXor(hUChar *input,int len);

			/*
			function:将unsigned char 位倒转，即bit0-bit7、bit1-bit6、bit2-bit5、bit3-bit4互换位置
			input:unsigned char:源字符
			return:互换之后的字符
			writer:wlq,2010-10-25
			*/
			static hUChar bitExchange(hUChar input);
			

			/*
			function:对数据内容长度进行和校验
			intput:p:需要校验的内容;nOffSet:偏移量;nLen:长度;返回的具体数据内容长度
			return:校验值结果
			writer:2007-8-14
			*/
			static hUInt16 checkLSum(hUChar *p,int nOffSet,int nLen,int &nDataLength);
			
			/*
			function:检查和校验结果
			input:待检查的内容;nOffSet:开始检查的位置;nLen:检查的长度
			return:返回和校验结果
			writer:wwg,2007-8-14
			*/
			static hUInt16 checkSum(hUChar *p,int nOffSet,int nLen);
			

			/*
			function:检查和校验结果
			input:待检查的内容;nOffSet:开始检查的位置;nLen:检查的长度
			return:返回和校验结果
			writer:wwg,2007-8-14
			*/
			static hUInt16 checkSumEx(hUChar *p,int nOffset,int nLen);
			

			/*
			function:获取和校验结果
			input:p:待检查的内容;nOffSet:偏移量;nLen:数据内容长度
			return:和校验结果值
			writer:wwg,2007-8-14
			*/
			static hUInt16 getCheckSum(hUChar *p,int nOffSet,int nLen);
			

			/*
			function:将16进制数据转换成字符串
			input:pStr:存储字符串的内存;nBufferSize:缓存大小;pData:原数据;nDataLen:原数据长度
			return:转换之后的长度
			writer:wwg,2007-8-14
			*/
			static int hex2String(char *pStr,int nBuffSize,int nOffSet,hUChar *pData,hUInt32 nDataLen);
			

			/*
			function:string转换到 hex
			input:char:源字符串
			return:转换之后的长度
			writer:wwg,2007-8-27
			*/

			static int string2Hex(char *pStr, hUChar *pData);
			

			/*
			function:找出字符在字符串中出现的位置
			input:pSrc:源字符串;offset:偏移量;nLen:字符长度;pdest:目标字符
			return:位置
			writer:wwg,2007-9-13
			*/
			static int midChar(char *pSrc,int offset,int nlen,char pdest);
				

			/*
			function:将4字节十六进制表示的浮点数转换成浮点数
			input:nLen:待转换的内容;nOffSet:偏移量
			return:转换后的浮点数
			writer:tyw,2007-9-7
			*/
			static float convertByteDouble(hUChar *p,int nOffSet);
			

			/*
			function:将Hex转换成 Ascii
			input:pSrc:源字符串;offset:偏移量;nLen:长度;pDest:目标
			return:转换之后的长度
			writer:wwg,2007-9-17
			*/
			static int hex2Ascii(hUChar *pSrc,int offset,int nLen,hUChar *pDest);
			

			/*
			function:将ascii码转换成hex
			input:pSrc:源字符串;offset:偏移量;nLen:源串长度;pDest:目标串
			return:32 31 31 30 45 44 34 33 44 30 31 32 46 46 30 31 46 46 46 46 46 46 30 31 46 46 46 46 46 46 46 38 44 46
			*/
			static int ascii2Hex(hUChar *pSrc,int offset,int nLen,hUChar *pDest);
				

			/*
			function:将hex转换成char
			input:pDest:目标;pSrc:源数据;offset:位移;nLen;源数据内容长度
			return:void
			writer:wwg,2007-10-11
			*/
			static void hex2Char(char *pDest,hUChar *pSrc,int offset,int nLen);
			

			/*
			function:根据小数位数将整数转换成小数
			input:percentage:小数位数;src:源数据
			return:返回转换之后的值
			writer:Wwg,2007-9-19
			*/
			static float int2Float(int percentage,int src);
			

			/*
			function:将整型数转换成bcd码
			input:源数据
			return:bcd结果码
			writer:wwg,2007-10-12
			*/
			static int int2BCD(int nsrc);
			

			static int bcd2Int(hUChar bTemp);
			

			/*
			function:将bcd码转换成整型
			input:源数据
			return:整形数
			write:wwg,2007-10-13
			*/
			static int bcd2Int(int nsrc);

			/*
			function:将bin码转换成bcd码
			input:源数据，输出的指针地址
			return:无
			write:
			*/			
			static void	bcd2bin1(unsigned char v1,unsigned char *v2);


			/*
			function:将bin码转换成bcd码
			input:源数据，输出的指针地址
			return:无
			write:
			*/			
			static void	bin2bcd2(unsigned short v1,unsigned short *v2);


			/*
			function:将bcd码转换成bin码
			input:源数据，输出的指针地址
			return:无
			write:
			*/			
			static void	bcd2bin2(unsigned short v1,unsigned short *v2);


			/*
			function:将bcd码转换成无符号整型
			input:源数据，输出的指针地址
			return:整形数
			write:
			*/
			static int		bcd4bin4(unsigned int	v1,unsigned int *v2);

			

			/*
			function:进行CRC16校验
			input:需要校验的数据;len:校验数据的长度
			return:校验结果
			writer:wwg,2007-11-08
			*/
			static hUInt16 modbusCRC(hUChar * pData, int len);
			

			/*
			Descritpion:获取串口的校验位，数据位，停止位
			Arguments:verify:校验位;databit:数据位;stopbit:停止位
			Return:计算结果
			Author:wwg,2008-5-7
			*/
			static hUChar getSerialParam(char verify,int databit,int stopbit);
			

			/*
			*----用于CDT校验
			bch,校验结果值
			p,校验字流开始指针
			*/
			static hUInt16 makeCRC(hUChar *p, int offset, int crc_len);

			static void MakeBch (hUChar *bch,hUChar * p);

			/*
				*----用于CDT校验
				len,校验字节长度
				crc,校验结果值
				p,校验字流开始指针
			*/
			static void MakeCRC8(short len,hUChar *crc,hUChar * p);
				

			/*
			Routine:标准的CDT校验
			*/
			static hUChar cdtCheck(hUChar *pBuff,int len);
			


			//************************************************************************************
			// *　函数名称 : ascii2Char
			// *  功    能 : ascii2Char
			// *  入口参数 : pBuff 数据存储区，len 数据长度
			// *  返 回 值 : strTemp 转换后的数据	
			// *  备    注 : 
			//************************************************************************************
			static string ascii2Char(hUChar  *pBuff, int len);


			//************************************************************************************
			// *　函数名称 : bufferMove
			// *  功    能 : 缓冲更新，将后一段数据move到head
			// *  入口参数 : buffer:缓存 ，buffer_size:buffer的大小， offset:开始位置， count:字符个数
			// *  返 回 值 : 
			// *  备    注 : 
			//************************************************************************************
			static hUChar* bufferMove(hUChar *buffer, int buffer_size, int offset,int count );



			/*
			* 函数说明：查找uchar 类型的数据串
			* 函数返回：-1:没找到；>=0:找到的位置
			*/
			//************************************************************************************
			// *　函数名称 : findData
			// *  功    能 : 函数说明：查找uchar 类型的数据串
			// *  入口参数 : trgData:源字符串， srgLeng:源字符串长度 ， srcdata,查找字符串， srcLen ,字符串长度
			// *  返 回 值 : 
			// *  备    注 : 
			//************************************************************************************
			static int findData(hUChar* trgData,int trgLen,hUChar* srcData,int srcLen);


			//************************************************************************************
			// *　函数名称 : floatToInt
			// *  功    能 : 函数说明：四舍五入将float转为i
			// *  入口参数 : hFloat:输入的浮点值
			// *  返 回 值 : 转换后的整型值
			// *  备    注 : 
			//************************************************************************************
			static int floatToInt(hFloat val);
		};
}

#endif // _PDT_DAC_TOOLBASE_H_

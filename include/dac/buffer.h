/*==============================================================================
* 文件名称 : bufferinf.h
* 文件功能 ：定义报文缓冲接口
* 模块 : 前置采集接口
* 创建时间 : 2009-06-08
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_BUFFER_H_
#define	_PDT_DAC_BUFFER_H_

#include "dac/dacshm.h"
#include "dac/dacdef.h"

namespace PDT
{
	class DAC_EXPORT CBuffer
	{
	public:
		CBuffer();
		CBuffer(hInt32 channel,hUChar type = BUFFER_RX);
		~CBuffer();

	public:
		bool			init(hInt32 channel,hUChar type = BUFFER_RX);
		bool			get(hUChar& val);
		hInt32			get(hUChar* buf,hUInt32 len);

		bool			put(hUChar val);
		bool			put(const hUChar *buf,hUInt32 len);

		hInt32			length()	const;		//可用字节数
		hInt32			remain()	const;		//剩余空间长度

		bool			back(hInt32 num);		//将get的数据放回
	private:
		DAC_COM_PARA		*m_pComPara;	
		DAC_COM_INFO		*m_pComInfo;	

		hInt32				m_channel;
		hUChar				m_type;

		//报文相关
		hUInt32				m_read;			//接收缓冲区读指针		

	};
}
#endif	//_PDT_DAC_BUFFER_H_

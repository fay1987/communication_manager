/*==============================================================================
* 文件名称 : message.h
* 文件功能 ：定义解析后报文操作接口
* 模块 : 前置采集接口
* 创建时间 : 2009-06-08
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_MESSAGE_H_
#define	_PDT_DAC_MESSAGE_H_

#include "dac/dacshm.h"

namespace PDT
{
	class DAC_EXPORT CMessage
	{
	public:
		CMessage();
		CMessage(hInt32 route);
		virtual ~CMessage();

	public:
		//////////////////////////////报文解析区///////////////////////////////////////
		bool			init(hInt32 route);	
		bool			hasMsg()	const;
		bool			get(DAC_MESSAGE& msg);
		bool			put(const DAC_MESSAGE& msg );

	private:
		DAC_COM_PARA	*m_pComPara;	
		DAC_COM_INFO	*m_pComInfo;	

		//报文相关
		hUInt32			m_read;		//解析后报文缓冲区读指针
		hUInt32			m_route;

	public:
		//由外部指定从哪里开始读取msg
		hInt32			writePos(hInt32 route)	const;
		bool			hasMsg(hInt32 route,hUInt32 readPos)	const;
		bool			get(hInt32 route,hUInt32 readPos,DAC_MESSAGE& msg);
	};
}
#endif	//_PDT_DAC_MESSAGE_H_

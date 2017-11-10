/*==============================================================================
* 文件名称 : ctrlinf.h
* 模块 : 控制处理接口
* 创建时间 : 2009-07-20 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_CTRLINF_H_
#define	_PDT_DAC_CTRLINF_H_

#include "dac/dacshm.h"
#include "dac/comminf.h"

#define CTRL_CMD_TIMEOUT	20			//20s
#define CTRL_ACK_TIMEOUT	20			//20s

namespace PDT
{
	class DAC_EXPORT CCtrlInf 
	{
	public:
		CCtrlInf();
		virtual ~CCtrlInf();

		/*  1、以下函数用来访问命令缓冲区
		*	2、buf的内容:ctrl_head + 具体的命令结构,见 dacctrl.h
		*/

		//添加命令,返回true-成功,false-失败
		bool	add(hUInt32 type,void* buf,hInt32 len,hInt32 groupNo=-1);
		//删除所有命令,返回true-成功,false-失败
		bool	remove();
		//检查命令超时等情况
		void	check();
		//获取一个有效命令,返回获取到的命令有效字节数, <=0失败，>0成功
		int		get(void* buf,hInt32 len );
		//获取指定数据组的一个有效命令,返回获取到的命令有效字节数, <=0失败，>0成功
		int		get(hInt32 groupNo,void* buf,hInt32 len);
		int		get(const char* groupCode,void* buf,hInt32 len);
		//获取指定数据组指定类型的一个有效命令,返回获取到的命令有效字节数, <=0失败，>0成功
		int		get(hInt32 groupNo,hUInt32 type,void* buf,hInt32 len);
		int		get(const char* groupCode,hUInt32 type,void* buf,hInt32 len);

		//获取参数加载命令
		int		getParaLoad(void* buf,hInt32 len);
		//获取人工操作命令
		int		getManOperate(void* buf,hInt32 len);
		//获取规约控制命令
		int		getProtocol(hInt32 groupNo,void* buf,hInt32 len);

		/*  1、以下函数用来访问命令返校缓冲区
		*	2、返回值false-失败，true-成功
		*	3、buf的内容:ctrl_head + ctrl_pro_yk_ack,见 fdcctrl.h
		*/
		//添加一个返校命令
		bool	addAck(hUInt32 type,void* buf,hInt32 len,hUInt32 groupNo=-1);
		//删除返校命令
		bool	removeAck();
		//检查反校超时等情况
		void	checkAck();
		//获取一个返校命令
		int		getAck(void* buf,hInt32 len);
		//获取指定数据组的一个返校命令
		int		getAck(hInt32 groupNo,void* buf,hInt32 len);
		int		getAck(const char* groupCode,void* buf,hInt32 len);
		//获取指定数据组指定控制号的一个返校命令
		//int		getAck(hInt32 groupNo,hInt32 ctrlNo,void *buf,hInt32 len);
		//int		getAck(const char* groupCode,hInt32 ctrlNo,void* buf,hInt32 len);
	public:
		static const char*		describe(hUInt32 type);	
	protected:
		CShareMemory	m_shm;
		CCommInf		m_commInf;

		DAC_CTRL_CMD*	getCtrl();
		int				getCtrl(hUInt32 beginType,hUInt32 endType,void* buf,hInt32 len);
		int				getCtrl(hInt32 groupNo,hUInt32 beginType,hUInt32 endType,void* buf,hInt32 len);
		DAC_CTRL_CMD*	getCtrlAck();
	};
}

#endif	//_ECON_FDC_CTRLINF_H_

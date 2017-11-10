/*==============================================================================
* 文件名称 : ctrlinf.cpp
* 模块 : 采集控制处理接口
* 创建时间 : 2009-07-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "dac/ctrlinf.h"
#include "dac/dacdef.h"
#include <ace/OS.h>
#include <sys/blog.h>

using namespace PDT;

CCtrlInf::CCtrlInf()
{
}

CCtrlInf::~CCtrlInf()
{
}

/*  1、以下函数用来访问命令缓冲区
*	2、返回值false-失败，true-成功
*	3、buf的内容:ctrl_head + 具体的命令结构,见 dacctrl.h
*/

//添加命令
bool	CCtrlInf::add(hUInt32 type,void* buf,hInt32 len,hInt32 groupNo/* =-1 */)
{
	if ( !buf || len >= DAC_CTRL_LEN )
		return false;
	DAC_CTRL_CMD* pCtrl = getCtrl();
	if ( !pCtrl )
		return false;
	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( pCtrl[i].valid )	
			continue;
		memcpy( pCtrl[i].buf,buf,len );
		pCtrl[i].type = type;
		pCtrl[i].groupNo = groupNo;
		pCtrl[i].inputTime = ACE_OS::time(NULL);
		pCtrl[i].len = len;
		pCtrl[i].valid = true;
		return true;
	}

	logprint(LOG_CTRLINF_BASE,"<ctrlinf add>命令缓冲区已满,add失败");
	return false;
}

//删除所有命令
bool	CCtrlInf::remove()
{
	DAC_CTRL_CMD* pCtrl = getCtrl();
	if ( !pCtrl )
		return false;

	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		pCtrl[i].valid = false;
	}

	return true;
}

void CCtrlInf::check()
{
	DAC_CTRL_CMD* pCtrl = getCtrl();
	if ( !pCtrl ) return ;

	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( !pCtrl[i].valid )	
			continue;

		//timeout check
		if ( time(NULL) - pCtrl[i].inputTime >= CTRL_CMD_TIMEOUT )
		{
			pCtrl[i].valid = false; 
			logprint(LOG_CTRLINF_BASE,"<ctrlinf>命令超时");
		}
		//check other condiction's cmd,such as yk timeout...
	}//end for

}

//获取一个有效命令
int	CCtrlInf::get(void* buf,hInt32 len)
{
	if ( !buf  )
		return 0;
	DAC_CTRL_CMD* pCtrl = getCtrl();
	if ( !pCtrl )
		return 0;

	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( !pCtrl[i].valid )	
			continue;
		if ( len < pCtrl[i].len )
		{
			logprint(LOG_CTRLINF_BASE,"<ctrlinf>缓冲区长度不够(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
			return 0;
		}
		memcpy( buf,pCtrl[i].buf,pCtrl[i].len );
		pCtrl[i].valid = false;
		return pCtrl[i].len;
	}

	return 0;
}

//获取指定数据组的一个有效命令
int	CCtrlInf::get(hInt32 groupNo,void* buf,hInt32 len)
{
	if ( !buf || groupNo <0 )
		return 0;
	DAC_CTRL_CMD* pCtrl = getCtrl();
	if ( !pCtrl )
		return 0;

	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( !pCtrl[i].valid )	
			continue;

		if ( pCtrl[i].groupNo == groupNo )
		{
			if ( len < pCtrl[i].len )
			{
				logprint(LOG_CTRLINF_BASE,"<ctrlinf>缓冲区长度不够(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
				return 0;
			}
			memcpy( buf,pCtrl[i].buf,pCtrl[i].len );
			pCtrl[i].valid = false;
			return pCtrl[i].len;
		}
	}

	return 0;
}

int	CCtrlInf::get(const char* groupCode,void* buf,hInt32 len)
{
	if ( !buf || !groupCode )
		return 0;
	DAC_CTRL_CMD* pCtrl = getCtrl();
	if ( !pCtrl )
		return 0;

	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( !pCtrl[i].valid )	
			continue;

		if ( pCtrl[i].groupNo == m_commInf.groupNo(groupCode) )
		{
			if ( len < pCtrl[i].len )
			{
				logprint(LOG_CTRLINF_BASE,"<ctrlinf>缓冲区长度不够(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
				return 0;
			}
			memcpy( buf,pCtrl[i].buf,pCtrl[i].len );
			pCtrl[i].valid = false;
			return pCtrl[i].len;
		}
	}

	return 0;
}

//获取指定数据组指定类型的一个有效命令
int	CCtrlInf::get(hInt32 groupNo,hUInt32 type,void* buf,hInt32 len)
{
	if ( !buf || groupNo < 0 )
		return 0;
	DAC_CTRL_CMD* pCtrl = getCtrl();
	if ( !pCtrl )
		return 0;

	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( !pCtrl[i].valid )	
			continue;
		if ( pCtrl[i].groupNo == groupNo && pCtrl[i].type == type )
		{
			if ( len < pCtrl[i].len )
			{
				logprint(LOG_CTRLINF_BASE,"<ctrlinf>缓冲区长度不够(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
				return 0;
			}
			memcpy( buf,pCtrl[i].buf,pCtrl[i].len );
			pCtrl[i].valid = false;
			return pCtrl[i].len;
		}
	}

	return 0;
}

int	CCtrlInf::get(const char* groupCode,hUInt32 type,void* buf,hInt32 len)
{
	if ( !buf || !groupCode )
		return 0;
	DAC_CTRL_CMD* pCtrl = getCtrl();
	if ( !pCtrl )
		return 0;

	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( !pCtrl[i].valid )	
			continue;

		if ( pCtrl[i].groupNo == m_commInf.groupNo(groupCode) && pCtrl[i].type == type )
		{
			if ( len < pCtrl[i].len )
			{
				logprint(LOG_CTRLINF_BASE,"<ctrlinf>缓冲区长度不够(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
				return 0;
			}
			memcpy( buf,pCtrl[i].buf,pCtrl[i].len );
			pCtrl[i].valid = false;
			return pCtrl[i].len;
		}
	}

	return 0;
}

int		CCtrlInf::getParaLoad(void* buf,hInt32 len)
{
	return getCtrl(CTRL_LOADPARA_ALL,CTRL_LOADPARA_KWH,buf,len);
}

int		CCtrlInf::getManOperate(void* buf,hInt32 len)
{
	return getCtrl(CTRL_MAN_START_CHANNEL,CTRL_MAN_DISABLE_GROUP,buf,len);
}

int		CCtrlInf::getProtocol(hInt32 groupNo,void* buf,hInt32 len)
{
	return getCtrl(groupNo,CTRL_PRO_RESET_RTU,CTRL_PRO_UNKNOWN,buf,len);
}

/*  1、以下函数用来访问命令返校缓冲区
*	2、返回值false-失败，true-成功
*	3、buf的内容:ctrl_head + ctrl_pro_yk_ack,见 dacctrl.h
*/
//添加一个返校命令
bool	CCtrlInf::addAck(hUInt32 type,void* buf,hInt32 len,hUInt32 groupNo/* =-1 */)
{
	if ( !buf || len >= DAC_CTRL_LEN )
		return false;
	DAC_CTRL_CMD* pCtrlAck = getCtrlAck();
	if ( !pCtrlAck )
		return false;

	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( pCtrlAck[i].valid )	
			continue;
		memcpy( pCtrlAck[i].buf,buf,len );
		pCtrlAck[i].groupNo = groupNo;
		pCtrlAck[i].type = type;
		pCtrlAck[i].inputTime = time(NULL);
		pCtrlAck[i].len = len;
		pCtrlAck[i].valid = true;
		return true;
	}

	return false;
}

//删除返校命令
bool	CCtrlInf::removeAck()
{
	DAC_CTRL_CMD* pCtrlAck = getCtrlAck();
	if ( !pCtrlAck )
		return false;
	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		pCtrlAck[i].valid = false;
	}

	return true;
}

void CCtrlInf::checkAck()
{
	DAC_CTRL_CMD* pCtrlAck = getCtrlAck();
	if ( !pCtrlAck ) return ;

	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( !pCtrlAck[i].valid )	
			continue;

		//check timeout
		if ( time(NULL) - pCtrlAck[i].inputTime >= CTRL_ACK_TIMEOUT )
		{
			pCtrlAck[i].valid = false;
			logprint(LOG_CTRLINF_BASE,"<ctrlinf ack> 命令超时");
		}
		//check other condition here...
	}

}

//获取一个返校命令
int	CCtrlInf::getAck(void* buf,hInt32 len)
{
	if ( !buf )
		return 0;
	DAC_CTRL_CMD* pCtrlAck = getCtrlAck();
	if ( !pCtrlAck )
		return 0;
	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( !pCtrlAck[i].valid )	
			continue;
		if ( len < pCtrlAck[i].len )
		{
			logprint(LOG_CTRLINF_BASE,"<ctrlinf ack> 缓冲区长度不够(len=%d < ctrlLen=%d)",len,(hInt32)pCtrlAck[i].len);
			return 0;
		}
		memcpy( buf,pCtrlAck[i].buf,pCtrlAck[i].len );
		pCtrlAck[i].valid = false;
		return pCtrlAck[i].len;
	}

	return 0;
}

int	CCtrlInf::getAck(hInt32 groupNo,void* buf,hInt32 len)
{
	if ( !buf )
		return 0;
	DAC_CTRL_CMD* pCtrlAck = getCtrlAck();
	if ( !pCtrlAck )
		return 0;
	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( !pCtrlAck[i].valid )	
			continue;

		if ( pCtrlAck[i].groupNo == groupNo )
		{
			if ( len < pCtrlAck[i].len )
			{
				logprint(LOG_CTRLINF_BASE,"<ctrlinf ack> 缓冲区长度不够(len=%d < ctrlLen=%d)",len,(hInt32)pCtrlAck[i].len);
				return 0;
			}
			memcpy( buf,pCtrlAck[i].buf,pCtrlAck[i].len );
			pCtrlAck[i].valid = false;
			return pCtrlAck[i].len;
		}

	}
	return 0;
}

int	CCtrlInf::getAck(const char* groupCode,void* buf,hInt32 len)
{
	if ( !buf || !groupCode )
		return 0;
	DAC_CTRL_CMD* pCtrlAck = getCtrlAck();
	if ( !pCtrlAck )
		return 0;
	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( !pCtrlAck[i].valid )	
			continue;

		if ( pCtrlAck[i].groupNo == m_commInf.groupNo(groupCode) )
		{
			if ( len < pCtrlAck[i].len )
			{
				logprint(LOG_CTRLINF_BASE,"<ctrlinf ack> 缓冲区长度不够(len=%d < ctrlLen=%d)",len,(hInt32)pCtrlAck[i].len);
				return 0;
			}
			memcpy( buf,pCtrlAck[i].buf,pCtrlAck[i].len );
			pCtrlAck[i].valid = false;
			return pCtrlAck[i].len;
		}

	}
	return 0;
}

//暂不支持
/*
int	CCtrlInf::getAck(hInt32 groupNo,hInt32 ctrlNo,void *buf,hInt32 len)
{
	if ( !buf || groupNo < 0 || ctrlNo <0 )
		return 0;
	DAC_CTRL_CMD* pCtrlAck = getCtrlAck();
	if ( !pCtrlAck )
		return 0;

	return 0;
}*/

//暂不支持
/*
int	CCtrlInf::getAck(const char* groupCode,hInt32 ctrlNo,void* buf,hInt32 len)
{
	if ( !buf || !groupCode || ctrlNo <0 )
		return 0;
	DAC_CTRL_CMD* pCtrlAck = getCtrlAck();
	if ( !pCtrlAck )
		return 0;

	return 0;
}*/


/*  
*	以下为辅助函数
*/
DAC_CTRL_CMD* CCtrlInf::getCtrl()
{
	if ( !m_shm.m_ptrCom )
		return NULL;

	return m_shm.m_ptrCom->ctrl;
}

int	CCtrlInf::getCtrl(hUInt32 beginType,hUInt32 endType,void* buf,hInt32 len)
{
	if ( !buf || len <= 0 )
		return 0;
	DAC_CTRL_CMD* pCtrl = getCtrl();
	if ( !pCtrl )
		return 0;

	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( !pCtrl[i].valid )	
			continue;

		//wfp changed at 20100106 from || to &&
		if ( pCtrl[i].type >= beginType && pCtrl[i].type <= endType )
		{
			if ( len < pCtrl[i].len )
			{
				logprint(LOG_CTRLINF_BASE,"<ctrlinf>缓冲区长度不够(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
				return 0;
			}
			memcpy( buf,pCtrl[i].buf,pCtrl[i].len );
			pCtrl[i].valid = false;
			return pCtrl[i].len;
		}
	}//end for

	return 0;
}

int	CCtrlInf::getCtrl(hInt32 groupNo,hUInt32 beginType,hUInt32 endType,void* buf,hInt32 len)
{
	if ( !buf || len <= 0 )
		return 0;
	DAC_CTRL_CMD* pCtrl = getCtrl();
	if ( !pCtrl )
		return 0;

	for (int i=0;i<DAC_CTRL_NUM;i++)
	{
		if ( !pCtrl[i].valid )	
			continue;

		if ( pCtrl[i].groupNo != groupNo ) 
			continue;

		if ( pCtrl[i].type >= beginType && pCtrl[i].type <= endType )
		{
			if ( len < pCtrl[i].len )
			{
				logprint(LOG_CTRLINF_BASE,"<ctrlinf>缓冲区长度不够(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
				return 0;
			}
			memcpy( buf,pCtrl[i].buf,pCtrl[i].len );
			pCtrl[i].valid = false;
			return pCtrl[i].len;
		}
	}//end for

	return 0;
}

DAC_CTRL_CMD* CCtrlInf::getCtrlAck()
{
	if ( !m_shm.m_ptrCom )
		return NULL;
	return m_shm.m_ptrCom->ack;
}

const char* CCtrlInf::describe(hUInt32 type)
{
	switch (type)
	{
	case CTRL_LOADPARA_ALL:
		return "加载参数ALL";
	case CTRL_LOADPARA_CHANNEL:
		return "加载参数CHANNEL";
	case CTRL_LOADPARA_GROUP:
		return "加载参数GROUP";
	case CTRL_LOADPARA_ROUTE:
		return "加载参数ROUTE";
	case CTRL_LOADPARA_PROTOCOL:
		return "加载参数PROTOCOL";
	case CTRL_LOADPARA_COM:
		return "加载参数通讯类型";
	case CTRL_LOADPARA_YC:
		return "加载参数YC";
	case CTRL_LOADPARA_YX:
		return "加载参数YX";
	case CTRL_LOADPARA_KWH:
		return "加载参数KWH";
	//通道、路径、数据组控制
	case CTRL_MAN_START_CHANNEL:
		return "人工启动通道";
	case CTRL_MAN_STOP_CHANNEL:
		return "人工停止通道";
	case CTRL_MAN_ENABLE_CHANNEL:
		return "人工启用通道";
	case CTRL_MAN_DISABLE_CHANNEL:
		return "人工禁用通道";
	case CTRL_MAN_START_ROUTE:
		return "人工启动路径";
	case CTRL_MAN_STOP_ROUTE:
		return "人工停止路径";
	case CTRL_MAN_ENABLE_ROUTE:
		return "人工启用路径";
	case CTRL_MAN_DISABLE_ROUTE:
		return "人工禁用路径";
	case CTRL_MAN_RESET_PROTOCOL:
		return "人工重启规约";
	case CTRL_MAN_START_GROUP:
		return "人工启动数据组";
	case CTRL_MAN_STOP_GROUP:
		return "人工停止数据组";
	case CTRL_MAN_ENABLE_GROUP:
		return "人工启用数据组";
	case CTRL_MAN_DISABLE_GROUP:
		return "人工禁用数据组";
	//规约
	case CTRL_PRO_RESET_RTU:
		return "复位RTU";
	case CTRL_PRO_SYNC_TIME:
		return "对时";
	case CTRL_PRO_CALL_ALL_DATA:
		return "总召唤";
	case CTRL_PRO_CALL_ALL_DATA_YC:
		return "总召遥测";
	case CTRL_PRO_CALL_ALL_DATA_YX:
		return "总召遥信";
	case CTRL_PRO_CALL_ALL_DATA_KWH:
		return "总召电度";
	case CTRL_PRO_CALL_SUBGROUP_DATA:
		return "分组召唤(101规约)";	
	case CTRL_PRO_YK:
		return "遥控";
	case CTRL_PRO_YT:
		return "遥调";
	case CTRL_PRO_SETPOINT_YC:
		return "设点遥测";
	case CTRL_PRO_SETPOINT_YX:
		return "设点遥信";
	case CTRL_PRO_SETPOINT_KWH:
		return "设点电度";
	case CTRL_PRO_DIRECT_OPERATE:
		return "直接操作";
	//反校
	case CTRL_PRO_ACK_YK:
		return "规约遥控反校";
	default:
		return "未知";

	}
}


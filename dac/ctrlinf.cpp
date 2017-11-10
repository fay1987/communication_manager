/*==============================================================================
* �ļ����� : ctrlinf.cpp
* ģ�� : �ɼ����ƴ���ӿ�
* ����ʱ�� : 2009-07-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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

/*  1�����º������������������
*	2������ֵfalse-ʧ�ܣ�true-�ɹ�
*	3��buf������:ctrl_head + ���������ṹ,�� dacctrl.h
*/

//�������
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

	logprint(LOG_CTRLINF_BASE,"<ctrlinf add>�����������,addʧ��");
	return false;
}

//ɾ����������
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
			logprint(LOG_CTRLINF_BASE,"<ctrlinf>���ʱ");
		}
		//check other condiction's cmd,such as yk timeout...
	}//end for

}

//��ȡһ����Ч����
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
			logprint(LOG_CTRLINF_BASE,"<ctrlinf>���������Ȳ���(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
			return 0;
		}
		memcpy( buf,pCtrl[i].buf,pCtrl[i].len );
		pCtrl[i].valid = false;
		return pCtrl[i].len;
	}

	return 0;
}

//��ȡָ���������һ����Ч����
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
				logprint(LOG_CTRLINF_BASE,"<ctrlinf>���������Ȳ���(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
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
				logprint(LOG_CTRLINF_BASE,"<ctrlinf>���������Ȳ���(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
				return 0;
			}
			memcpy( buf,pCtrl[i].buf,pCtrl[i].len );
			pCtrl[i].valid = false;
			return pCtrl[i].len;
		}
	}

	return 0;
}

//��ȡָ��������ָ�����͵�һ����Ч����
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
				logprint(LOG_CTRLINF_BASE,"<ctrlinf>���������Ȳ���(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
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
				logprint(LOG_CTRLINF_BASE,"<ctrlinf>���������Ȳ���(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
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

/*  1�����º��������������У������
*	2������ֵfalse-ʧ�ܣ�true-�ɹ�
*	3��buf������:ctrl_head + ctrl_pro_yk_ack,�� dacctrl.h
*/
//���һ����У����
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

//ɾ����У����
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
			logprint(LOG_CTRLINF_BASE,"<ctrlinf ack> ���ʱ");
		}
		//check other condition here...
	}

}

//��ȡһ����У����
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
			logprint(LOG_CTRLINF_BASE,"<ctrlinf ack> ���������Ȳ���(len=%d < ctrlLen=%d)",len,(hInt32)pCtrlAck[i].len);
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
				logprint(LOG_CTRLINF_BASE,"<ctrlinf ack> ���������Ȳ���(len=%d < ctrlLen=%d)",len,(hInt32)pCtrlAck[i].len);
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
				logprint(LOG_CTRLINF_BASE,"<ctrlinf ack> ���������Ȳ���(len=%d < ctrlLen=%d)",len,(hInt32)pCtrlAck[i].len);
				return 0;
			}
			memcpy( buf,pCtrlAck[i].buf,pCtrlAck[i].len );
			pCtrlAck[i].valid = false;
			return pCtrlAck[i].len;
		}

	}
	return 0;
}

//�ݲ�֧��
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

//�ݲ�֧��
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
*	����Ϊ��������
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
				logprint(LOG_CTRLINF_BASE,"<ctrlinf>���������Ȳ���(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
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
				logprint(LOG_CTRLINF_BASE,"<ctrlinf>���������Ȳ���(len=%d < ctrlLen=%d)",len,(hInt32)pCtrl[i].len);
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
		return "���ز���ALL";
	case CTRL_LOADPARA_CHANNEL:
		return "���ز���CHANNEL";
	case CTRL_LOADPARA_GROUP:
		return "���ز���GROUP";
	case CTRL_LOADPARA_ROUTE:
		return "���ز���ROUTE";
	case CTRL_LOADPARA_PROTOCOL:
		return "���ز���PROTOCOL";
	case CTRL_LOADPARA_COM:
		return "���ز���ͨѶ����";
	case CTRL_LOADPARA_YC:
		return "���ز���YC";
	case CTRL_LOADPARA_YX:
		return "���ز���YX";
	case CTRL_LOADPARA_KWH:
		return "���ز���KWH";
	//ͨ����·�������������
	case CTRL_MAN_START_CHANNEL:
		return "�˹�����ͨ��";
	case CTRL_MAN_STOP_CHANNEL:
		return "�˹�ֹͣͨ��";
	case CTRL_MAN_ENABLE_CHANNEL:
		return "�˹�����ͨ��";
	case CTRL_MAN_DISABLE_CHANNEL:
		return "�˹�����ͨ��";
	case CTRL_MAN_START_ROUTE:
		return "�˹�����·��";
	case CTRL_MAN_STOP_ROUTE:
		return "�˹�ֹͣ·��";
	case CTRL_MAN_ENABLE_ROUTE:
		return "�˹�����·��";
	case CTRL_MAN_DISABLE_ROUTE:
		return "�˹�����·��";
	case CTRL_MAN_RESET_PROTOCOL:
		return "�˹�������Լ";
	case CTRL_MAN_START_GROUP:
		return "�˹�����������";
	case CTRL_MAN_STOP_GROUP:
		return "�˹�ֹͣ������";
	case CTRL_MAN_ENABLE_GROUP:
		return "�˹�����������";
	case CTRL_MAN_DISABLE_GROUP:
		return "�˹�����������";
	//��Լ
	case CTRL_PRO_RESET_RTU:
		return "��λRTU";
	case CTRL_PRO_SYNC_TIME:
		return "��ʱ";
	case CTRL_PRO_CALL_ALL_DATA:
		return "���ٻ�";
	case CTRL_PRO_CALL_ALL_DATA_YC:
		return "����ң��";
	case CTRL_PRO_CALL_ALL_DATA_YX:
		return "����ң��";
	case CTRL_PRO_CALL_ALL_DATA_KWH:
		return "���ٵ��";
	case CTRL_PRO_CALL_SUBGROUP_DATA:
		return "�����ٻ�(101��Լ)";	
	case CTRL_PRO_YK:
		return "ң��";
	case CTRL_PRO_YT:
		return "ң��";
	case CTRL_PRO_SETPOINT_YC:
		return "���ң��";
	case CTRL_PRO_SETPOINT_YX:
		return "���ң��";
	case CTRL_PRO_SETPOINT_KWH:
		return "�����";
	case CTRL_PRO_DIRECT_OPERATE:
		return "ֱ�Ӳ���";
	//��У
	case CTRL_PRO_ACK_YK:
		return "��Լң�ط�У";
	default:
		return "δ֪";

	}
}


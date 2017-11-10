/*===================================================================
*	ģ�����ƣ�	��Լ����
*	��������:	2013��11��27�� 9:36
*	�� �� ��: 	e:\pdt3000\program\dac\dac\src\protocol.cpp
*	�ļ�·��:	e:\pdt3000\program\dac\dac\src
*	ģ������:	protocol
*	�ļ�����:	cpp
	
*	��    ��:	wlq
*	��    ��:	V2.0
	
*	ģ��˵��:	��Լ����

*	�޸İ汾��	V2.1
*	�޸����ߣ�	wlq
*	�޸����ڣ�	27/11/2013
*	�޸�ժҪ��

====================================================================*/
#include "dac/protocol.h"
#include "utl/datetime.h"
#include <ace/OS.h>

using namespace PDT;

CProtocol::CProtocol()
:m_channel(-1),m_route(-1)
{
	m_msgLen = 0;
}

CProtocol::~CProtocol()
{
}

//��ʼ��ʱ����
bool CProtocol::init(hInt32 channel,hInt32 route)
{
	//wfp changed at 20110616 for ���Ӻ�10ms�ڼ��յ��ı��Ĺ�Լ�޷����������
	//���÷���:ͨ����·���Ŷ����䣬���������ñ��ļ���ʾ������
	if ( (m_channel != channel) || (m_route != route) )
	{
		m_channel = channel;
		m_route = route;

		m_rxBuffer.init(channel);
		m_txBuffer.init(channel,BUFFER_TX);
		m_message.init(route);
	}
	////////////////////wfp changed end///////////////////////////////////////

	m_isSending = false;

	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(m_route);
	if ( pRouteInfo == NULL ) return false;
	pRouteInfo->cmd.all_data = true;
	pRouteInfo->cmd.all_yc = true;
	pRouteInfo->cmd.all_yx = true;

	pRouteInfo->switchFlag = false;

	return true;
}

bool CProtocol::readFeature()
{
	return true;
}

/*
*���������ܣ�����Լ��Ҫ�յ���Ӧ��������У���ʹ�øú�����������
*������������true:���������ڷ��ͣ�false:��
*/
hBool CProtocol::isSending() const
{
	return m_isSending;
}

/*
 *���������ܣ�mainTx()�б����Ƿ�������������
 *������������true:����������ͣ�false:������
 */
void CProtocol::reportCmdSend(hBool isSend /* = true */)
{
	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(m_route);
	if ( pRouteInfo == NULL ) return;

	if ( isSend ) 
	{
		m_isSending = true;
		pRouteInfo->switchFlag = false;
	}
	else
	{
		m_isSending = false;
		pRouteInfo->switchFlag = true;
	}
}

/*
*���������ܣ�mainRx()�б����յ���������Ļ�У����
*������������true:������ȷ��false:���ݴ���
*/
void CProtocol::repotrAckRecv(hBool isOk /* = true */)
{
	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(m_route);
	if ( pRouteInfo == NULL ) return;

	m_isSending = false;
	pRouteInfo->switchFlag = true;
}

//���ջ����������ֽ���
hUInt32 CProtocol::length()	const
{	
	int ret = m_rxBuffer.length();
	if ( ret <= 0 ) return 0;
	return (hUInt32)ret;
}

bool CProtocol::back(hUInt32 num)
{
	return m_rxBuffer.back(num);
}

bool CProtocol::get(hUChar& val)
{
	return m_rxBuffer.get(val);
}

hUInt32	CProtocol::get(hUChar *buf,hUInt32 len)
{
	int ret = m_rxBuffer.get(buf,len);
	if ( ret <= 0 ) return 0;
	return ret;
}

//���ͻ�����
bool CProtocol::put(hUChar val)
{
	return m_txBuffer.put(val);
}

bool CProtocol::put(const hUChar *buf,hUInt32 len)
{
	return m_txBuffer.put(buf,len);
}

hUInt32 CProtocol::remain()
{
	int ret = m_txBuffer.remain();
	if ( ret <= 0 ) return 0;
	return ret;
}

//��Ϣ����
bool CProtocol::msgCopy(const hUChar* buf,hUInt32 len)
{
	if ( buf == NULL ) return false;
	if ( len < 0 || (m_msgLen + len) >= DAC_MSG_DATA_LEN ) return false;

	memcpy(m_msg+m_msgLen,buf,len);
	m_msgLen += len;
	return true;
}

bool CProtocol::msgRemove(hUInt32 len)
{
	if ( len < 0 || len > m_msgLen) return false;

	m_msgLen -= len;
	return true;
}

void CProtocol::msgClean()
{
	m_msgLen = 0;
}

bool CProtocol::msgDisplay(hUInt8 type,const char* desc)
{
	bool ret = msgDisplay(type,desc,m_msg,m_msgLen);
	m_msgLen = 0;
	return ret;
}

bool CProtocol::msgDisplay(hUInt8 type,const char* desc,const hUChar* data,hUInt32 len)
{
	if ( desc == NULL || data == NULL || len <= 0 || len >= DAC_MSG_DATA_LEN ) return false;

	DAC_MESSAGE msg;
	CDateTime datetime = CDateTime::currentDateTime();
	msg.time.year = (hUInt16)datetime.year();
	msg.time.mon = (hUInt8)datetime.month();
	msg.time.day = (hUInt8)datetime.day();
	msg.time.hour = (hUInt8)datetime.hour();
	msg.time.min = (hUInt8)datetime.minute();
	msg.time.sec = (hUInt8)datetime.second();
	msg.time.msec = (hUInt16)(datetime.microsec()/1000);

	msg.type = type;
	ACE_OS::strncpy(msg.desc,desc,DAC_DESC_LEN);
	msg.length = len;
	ACE_OS::memcpy(msg.data,data,len);

	return m_message.put(msg);
}

bool CProtocol::addSendFrmNum(int frmNum/* =1 */)
{
	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(m_route);
	if ( pRouteInfo == NULL ) return false;

	if ( pRouteInfo->txFrmCnt >= INT_MAX ) pRouteInfo->txFrmCnt = 0;
	pRouteInfo->txFrmCnt += frmNum;
	return true;
}

bool CProtocol::addRecvOkFrmNum(int frmNum/* =1 */)
{
	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(m_route);
	if ( pRouteInfo == NULL ) return false;

	if ( pRouteInfo->okFrmCnt >= INT_MAX ) pRouteInfo->okFrmCnt = 0;
	pRouteInfo->okFrmCnt += frmNum;

	if ( pRouteInfo->rxFrmCnt >= INT_MAX ) pRouteInfo->rxFrmCnt = 0;
	pRouteInfo->rxFrmCnt += frmNum;

	pRouteInfo->lastDataOkTime = (hUInt32)ACE_OS::time(0);

	return true;
}

bool CProtocol::addRecvErrFrmNum(int frmNum/* =1 */)
{
	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(m_route);
	if ( pRouteInfo == NULL ) return false;

	if ( pRouteInfo->errFrmCnt >= INT_MAX ) pRouteInfo->errFrmCnt = 0;
	pRouteInfo->errFrmCnt += frmNum;

	if ( pRouteInfo->rxFrmCnt >= INT_MAX ) pRouteInfo->rxFrmCnt = 0;
	pRouteInfo->rxFrmCnt += frmNum;

	return true;
}

bool CProtocol::getCtrlCmd(char* cmdBuf,int len)
{
	if ( cmdBuf == NULL || len < CTRL_COMMON_LEN ) return false;

	char buf[DAC_CTRL_LEN];
	DAC_ROUTE* pRoute = m_commInf.route(m_route);
	if ( pRoute == NULL ) return false;

	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(m_route);
	if ( pRouteInfo == NULL ) return false;

	if( !pRouteInfo->mainFlag ) return false;

	if  ( m_ctrlInf.getProtocol(pRoute->group,buf,DAC_CTRL_LEN) )
	{
		ctrl_head* pHead = (ctrl_head*)buf;
		if ( pHead->type == CTRL_PRO_COMMON )
		{
			ctrl_pro_common* pCommon = (ctrl_pro_common*)(buf+sizeof(ctrl_head));
			ACE_OS::memcpy(cmdBuf,pCommon->cmdBuf,CTRL_COMMON_LEN);
			return true;
		}

	}//end if
	return false;
}

bool CProtocol::getCtrlCmdByRoute(hInt32 routeno,char* cmdBuf,int len)
{
	if ( cmdBuf == NULL || len < CTRL_COMMON_LEN ) return false;

	char buf[DAC_CTRL_LEN];
	DAC_ROUTE* pRoute = m_commInf.route(routeno);
	if ( pRoute == NULL ) return false;

	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(m_route);
	if ( pRouteInfo == NULL ) return false;

	if( !pRouteInfo->mainFlag ) return false;

	if  ( m_ctrlInf.getProtocol(pRoute->group,buf,DAC_CTRL_LEN) )
	{
		ctrl_head* pHead = (ctrl_head*)buf;
		if ( pHead->type == CTRL_PRO_COMMON )
		{
			ctrl_pro_common* pCommon = (ctrl_pro_common*)(buf+sizeof(ctrl_head));
			ACE_OS::memcpy(cmdBuf,pCommon->cmdBuf,CTRL_COMMON_LEN);
			return true;
		}

	}//end if
	return false;
}

bool CProtocol::setCtrlCmdAck(const char* ackBuf,int len)
{
	if ( ackBuf == NULL || len <= 0 ) return false;
	DAC_ROUTE* pRoute = m_commInf.route(m_route);
	if ( pRoute == NULL ) return false;

	int copyLen = len;
	if ( copyLen >= CTRL_COMMON_LEN ) copyLen = CTRL_COMMON_LEN;

	ctrl_pro_common_ack commonAck;
	commonAck.groupNo = pRoute->group;
	commonAck.length = copyLen;
	ACE_OS::memcpy(commonAck.ackBuf,ackBuf,copyLen);

	return commonCmdAck(&commonAck);
}

/*
 * ����˵�����ú���ͬCDacCtrl.commonCmdAck()���������ڴ˴����ܵ�CDacCtrl�ĺ�����������¶���
 */
bool CProtocol::commonCmdAck(const ctrl_pro_common_ack* pCommonAck)
{
	if ( !pCommonAck )
		return false;
	if ( !m_commInf.isGroupValid(pCommonAck->groupNo) )
		return false;

	unsigned char buf[512];
	hInt32 headLen,dataLen;

	headLen = sizeof( ctrl_head );
	dataLen = sizeof( ctrl_pro_common_ack);
	ctrl_head*	pHead = (ctrl_head*)buf;
	pHead->type = CTRL_PRO_COMMON_ACK;
	ctrl_pro_common_ack* pData = (ctrl_pro_common_ack*)(buf + headLen);
	ACE_OS::memcpy(pData,pCommonAck,dataLen );

	return m_ctrlInf.addAck(CTRL_PRO_COMMON_ACK,buf, headLen + dataLen,pCommonAck->groupNo);
}




#include "dac/stateformat.h"

using namespace PDT;

CString CStateFormat::channelStateDesc(hInt32 state) 
{
	switch (state)
	{
	case CHANNEL_STATE_LISTEN:
		return "����";
	case CHANNEL_STATE_DOWN:
		return "�Ͽ�";
	case CHANNEL_STATE_UP:
		return "����";
	case CHANNEL_STATE_DISABLED:
		return "����";
	default:
		return "δ֪";
	}
}

CString CStateFormat::groupStateDesc(hInt32 state)
{
	switch (state)
	{
	case GROUP_STATE_DOWN:
		return "ֹͣ";
	case GROUP_STATE_UP:
		return "����";
	case GROUP_STATE_DISABLED:
		return "����";
	default:
		return "δ֪";
	}
}

CString CStateFormat::routeStateDesc(hInt32 state)
{
	switch (state)
	{
	case ROUTE_STATE_DOWN:
		return "ֹͣ";
	case ROUTE_STATE_UP:
		return "����";
	case ROUTE_STATE_STANDBY:
		return "�ȱ�";
	case ROUTE_STATE_FREE:
		return "����";
	case ROUTE_STATE_DISABLED:
		return "����";
	default:
		return "δ֪";
	}
}

CString CStateFormat::mainFlagDesc(hBool flag)
{
	if (flag == true)
		return "��";
	else 
		return "��";
}

CString CStateFormat::channelTypeDesc(hUInt8 type)
{
	switch (type)
	{
	case CHANNEL_TYPE_SERIAL:
		return "����";
	case CHANNEL_TYPE_TCPCLIENT:
		return "TCP�ͻ���";
	case CHANNEL_TYPE_TCPSERVER:
		return "TCP�����";
	case CHANNEL_TYPE_UDP:
		return "UDP";
	case CHANNEL_TYPE_GATECLIENT:
		return "���ؿͻ���";
	case CHANNEL_TYPE_SERIALWIRELESS:
		return "���ߴ���";
	default:
		return "δ֪";
	}
}

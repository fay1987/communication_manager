#include "dac/stateformat.h"

using namespace PDT;

CString CStateFormat::channelStateDesc(hInt32 state) 
{
	switch (state)
	{
	case CHANNEL_STATE_LISTEN:
		return "监听";
	case CHANNEL_STATE_DOWN:
		return "断开";
	case CHANNEL_STATE_UP:
		return "连接";
	case CHANNEL_STATE_DISABLED:
		return "禁用";
	default:
		return "未知";
	}
}

CString CStateFormat::groupStateDesc(hInt32 state)
{
	switch (state)
	{
	case GROUP_STATE_DOWN:
		return "停止";
	case GROUP_STATE_UP:
		return "运行";
	case GROUP_STATE_DISABLED:
		return "禁用";
	default:
		return "未知";
	}
}

CString CStateFormat::routeStateDesc(hInt32 state)
{
	switch (state)
	{
	case ROUTE_STATE_DOWN:
		return "停止";
	case ROUTE_STATE_UP:
		return "运行";
	case ROUTE_STATE_STANDBY:
		return "热备";
	case ROUTE_STATE_FREE:
		return "空闲";
	case ROUTE_STATE_DISABLED:
		return "禁用";
	default:
		return "未知";
	}
}

CString CStateFormat::mainFlagDesc(hBool flag)
{
	if (flag == true)
		return "主";
	else 
		return "备";
}

CString CStateFormat::channelTypeDesc(hUInt8 type)
{
	switch (type)
	{
	case CHANNEL_TYPE_SERIAL:
		return "串口";
	case CHANNEL_TYPE_TCPCLIENT:
		return "TCP客户端";
	case CHANNEL_TYPE_TCPSERVER:
		return "TCP服务端";
	case CHANNEL_TYPE_UDP:
		return "UDP";
	case CHANNEL_TYPE_GATECLIENT:
		return "网关客户端";
	case CHANNEL_TYPE_SERIALWIRELESS:
		return "无线串口";
	default:
		return "未知";
	}
}

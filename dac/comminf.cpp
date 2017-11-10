/*==============================================================================
* 文件名称 : comminf.cpp
* 模块 : 前置数据访问接口
* 创建时间 : 2009-04-28 15:55:23
* 作者 : 尹宏昊
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "dac/comminf.h"

using namespace PDT;

CCommInf::CCommInf()
{
	CShareMemory shareMemory;
	if ( shareMemory.m_ptrCom == NULL )
	{
		m_pComPara = NULL;
		m_pComInfo = NULL;
		m_pComCtrl = NULL;
		m_pComAck = NULL;
	}
	else
	{
		m_pComPara = &shareMemory.m_ptrCom->para;
		m_pComInfo = &shareMemory.m_ptrCom->info;
		m_pComCtrl = shareMemory.m_ptrCom->ctrl;
		m_pComAck = shareMemory.m_ptrCom->ack;

		//初始化信息变化区的读指针
		m_chgChannelRead = m_pComInfo->chgChannelWrite;
		m_chgGroupRead = m_pComInfo->chgGroupWrite;
		m_chgRouteRead = m_pComInfo->chgRouteWrite;
	}
}

CCommInf::~CCommInf()
{
}

///////////////////////////参数///////////////////////////////////////////////
DAC_SYSTEM*	CCommInf::system()
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return &m_pComPara->system;
}

DAC_STATION* CCommInf::station(const char* code,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;
	return station( stationNo(code),isValidCheck );
}

DAC_STATION* CCommInf::station(hInt32 no/* =0 */,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;
	if ( !isValidCheck )
	{
		if (no < 0 || no >= m_pComInfo->system.stationNum) return 0;
		return &m_pComPara->station[no];
	}

	if (isStationValid(no))
	{
		return &m_pComPara->station[no];
	}

	return 0;
}

const DAC_STATION* CCommInf::station(const char* code,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;
	return station( stationNo(code),isValidCheck );
}

const DAC_STATION* CCommInf::station(hInt32 no/* =0 */,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;
	if ( !isValidCheck )
	{
		if (no < 0 || no >= m_pComInfo->system.stationNum) return 0;
		return &m_pComPara->station[no];
	}

	if (isStationValid(no))
	{
		return &m_pComPara->station[no];
	}

	return 0;
}




DAC_SENDDEV* CCommInf::senddev(hInt32 no/* =0 */,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;
	if ( !isValidCheck )
	{
		if (no < 0 || no >= m_pComInfo->system.senddevNum) return 0;
		return &m_pComPara->senddev[no];
	}

	if (isSenddevValid(no))
	{
		return &m_pComPara->senddev[no];
	}

	return 0;
}

const DAC_SENDDEV* CCommInf::senddev(hInt32 no/* =0 */,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;
	if ( !isValidCheck )
	{
		if (no < 0 || no >= m_pComInfo->system.senddevNum) return 0;
		return &m_pComPara->senddev[no];
	}

	if (isSenddevValid(no))
	{
		return &m_pComPara->senddev[no];
	}

	return 0;
}

DAC_SENDDEV* CCommInf::getSenddevbygroup(hInt32 no)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;
	if (no < 0 || no >= m_pComInfo->system.groupNum) return 0;
	DAC_GROUP* grpptr = group(no);

	for (int i = 0; i < m_pComInfo->system.senddevNum; i++)
	{
		if (strcmp(grpptr->code,m_pComPara->senddev[i].grpcode) == 0 )
			return &m_pComPara->senddev[i];
	}

	return 0;
}


DAC_CHANNEL* CCommInf::channel(const char* code,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return channel( channelNo(code),isValidCheck );
}
DAC_CHANNEL* CCommInf::channel(hInt32 no/* =0 */,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if ( !isValidCheck ) 
	{
		if (no < 0 || no >= m_pComInfo->system.channelNum) return 0;
		return &m_pComPara->channel[no];
	}

	if (isChannelValid(no))
	{
		return &m_pComPara->channel[no];
	}

	return 0;
}

const DAC_CHANNEL*	CCommInf::channel(const char* code,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return channel( channelNo(code),isValidCheck );
}
const DAC_CHANNEL* CCommInf::channel(hInt32 no/* =0 */,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if ( !isValidCheck ) 
	{
		if (no < 0 || no >= m_pComInfo->system.channelNum) return 0;
		return &m_pComPara->channel[no];
	}

	if (isChannelValid(no))
	{
		return &m_pComPara->channel[no];
	}

	return 0;
}

DAC_GROUP* CCommInf::group(const char* code,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return group( groupNo(code),isValidCheck );
}
DAC_GROUP* CCommInf::group(hInt32 no/* =0 */,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if ( !isValidCheck ) 
	{
		if (no < 0 || no >= m_pComInfo->system.groupNum) return 0;
		return &m_pComPara->group[no];
	}

	if (isGroupValid(no))
	{
		return &m_pComPara->group[no];
	}

	return 0;
}

const DAC_GROUP* CCommInf::group(const char* code,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return group( groupNo(code),isValidCheck );
}
const DAC_GROUP* CCommInf::group(hInt32 no/* =0 */,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if ( !isValidCheck ) 
	{
		if (no < 0 || no >= m_pComInfo->system.groupNum) return 0;
		return &m_pComPara->group[no];
	}

	if (isGroupValid(no))
	{
		return &m_pComPara->group[no];
	}

	return 0;
}

DAC_ROUTE* CCommInf::route(const char* code,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return route( routeNo(code),isValidCheck );
}
DAC_ROUTE* CCommInf::route(hInt32 no/* =0 */,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if (!isValidCheck) 
	{
		if (no < 0 || no >= m_pComInfo->system.routeNum) return 0;
		return &m_pComPara->route[no];
	}

	if (isRouteValid(no))
	{
		return &m_pComPara->route[no];
	}

	return 0;
}

const DAC_ROUTE* CCommInf::route(const char* code,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return route( routeNo(code),isValidCheck );
}

const DAC_ROUTE* CCommInf::route(hInt32 no/* =0 */,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if (!isValidCheck) 
	{
		if (no < 0 || no >= m_pComInfo->system.routeNum) return 0;
		return &m_pComPara->route[no];
	}

	if (isRouteValid(no))
	{
		return &m_pComPara->route[no];
	}

	return 0;
}

DAC_PROTOCOL* CCommInf::protocol(const char* code,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return protocol( protocolNo(code),isValidCheck );
}
DAC_PROTOCOL* CCommInf::protocol(hInt32 no/* =0 */,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if (!isValidCheck) 
	{
		if (no < 0 || no >= DAC_PROTOCOL_NUM) return 0;
		return &m_pComPara->protocol[no];
	}

	if (isProtocolValid(no))
	{
		return &m_pComPara->protocol[no];
	}

	return 0;
}

const DAC_PROTOCOL*	CCommInf::protocol(const char* code,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return protocol( protocolNo(code),isValidCheck );
}
const DAC_PROTOCOL*	CCommInf::protocol(hInt32 no/* =0 */,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if (!isValidCheck) 
	{
		if (no < 0 || no >= DAC_PROTOCOL_NUM) return 0;
		return &m_pComPara->protocol[no];
	}

	if (isProtocolValid(no))
	{
		return &m_pComPara->protocol[no];
	}

	return 0;
}

DAC_YK* CCommInf::yk(const char* routeName,const char* code)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	for (int i=0;i<DAC_YK_NUM;i++)
	{
		if ( !m_pComPara->yk[i].valid ) continue;

		if ( strcmp(routeName,m_pComPara->yk[i].routeCode) == 0 
			&& strcmp(code,m_pComPara->yk[i].code) == 0 )
			return &m_pComPara->yk[i];
	}

	return 0;
}

DAC_YK* CCommInf::ykShmPtr()
{
	return &m_pComPara->yk[0];
}

const DAC_YK* CCommInf::yk(const char* routeName,const char* code) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	for (int i=0;i<DAC_YK_NUM;i++)
	{
		if ( !m_pComPara->yk[i].valid ) continue;

		if ( strcmp(routeName,m_pComPara->yk[i].routeCode) == 0 
			&& strcmp(code,m_pComPara->yk[i].code) == 0 )
			return &m_pComPara->yk[i];
	}

	return 0;
}

const DAC_YK* CCommInf::ykShmPtr() const
{
	return &m_pComPara->yk[0];
}

DAC_YT* CCommInf::yt(const char* routeName,const char* code)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	for (int i=0;i<DAC_YT_NUM;i++)
	{
		if ( !m_pComPara->yt[i].valid ) continue;

		if ( strcmp(routeName,m_pComPara->yt[i].routeCode) == 0 
			&& strcmp(code,m_pComPara->yt[i].code) == 0 )
			return &m_pComPara->yt[i];
	}

	return 0;
}

DAC_YT* CCommInf::ytShmPtr()
{
	return &m_pComPara->yt[0];
}

const DAC_YT* CCommInf::yt(const char* routeName,const char* code) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	for (int i=0;i<DAC_YT_NUM;i++)
	{
		if ( !m_pComPara->yt[i].valid ) continue;

		if ( strcmp(routeName,m_pComPara->yt[i].routeCode) == 0 
			&& strcmp(code,m_pComPara->yt[i].code) == 0 )
			return &m_pComPara->yt[i];
	}

	return 0;
}

const DAC_YT* CCommInf::ytShmPtr() const
{
	return &m_pComPara->yt[0];
}

///////////////////////////信息///////////////////////////////////////////////
DAC_SYSTEM_INFO* CCommInf::systemInfo()
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return &m_pComInfo->system;
}

const DAC_SYSTEM_INFO* CCommInf::systemInfo() const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return &m_pComInfo->system;
}

DAC_CHANNEL_INFO* CCommInf::channelInfo(const char* code,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return channelInfo( channelNo(code),isValidCheck );
}
DAC_CHANNEL_INFO* CCommInf::channelInfo(hInt32 no/* =0 */,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if (!isValidCheck) 
	{
		if (no < 0 || no >= m_pComInfo->system.channelNum) return 0;
		return &m_pComInfo->channel[no];
	}

	if (isChannelValid(no))
	{
		return &m_pComInfo->channel[no];
	}

	return 0;
}

const DAC_CHANNEL_INFO* CCommInf::channelInfo(const char* code,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return channelInfo( channelNo(code),isValidCheck );
}
const DAC_CHANNEL_INFO* CCommInf::channelInfo(hInt32 no/* =0 */,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if (!isValidCheck) 
	{
		if (no < 0 || no >= m_pComInfo->system.channelNum) return 0;
		return &m_pComInfo->channel[no];
	}

	if (isChannelValid(no))
	{
		return &m_pComInfo->channel[no];
	}

	return 0;
}

DAC_GROUP_INFO*	CCommInf::groupInfo(const char* code,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return groupInfo( groupNo(code),isValidCheck );
}
DAC_GROUP_INFO*	CCommInf::groupInfo(hInt32 no/* =0 */,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if (!isValidCheck) 
	{
		if (no < 0 || no >= m_pComInfo->system.groupNum) return 0;
		return &m_pComInfo->group[no];
	}

	if (isGroupValid(no))
	{
		return &m_pComInfo->group[no];
	}
	
	return 0;
}

const DAC_GROUP_INFO* CCommInf::groupInfo(const char* code,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return groupInfo( groupNo(code),isValidCheck );
}
const DAC_GROUP_INFO* CCommInf::groupInfo(hInt32 no/* =0 */,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if (!isValidCheck) 
	{
		if (no < 0 || no >= m_pComInfo->system.groupNum) return 0;
		return &m_pComInfo->group[no];
	}

	if (isGroupValid(no))
	{
		return &m_pComInfo->group[no];
	}

	return 0;
}

DAC_ROUTE_INFO*	CCommInf::routeInfo(const char* code,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return routeInfo( routeNo(code),isValidCheck );
}
DAC_ROUTE_INFO*	CCommInf::routeInfo(hInt32 no/* =0 */,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if (!isValidCheck) 
	{
		if (no < 0 || no >= m_pComInfo->system.routeNum) return 0;
		return &m_pComInfo->route[no];
	}

	if (isRouteValid(no))
	{
		return &m_pComInfo->route[no];
	}

	return 0;
}

const DAC_ROUTE_INFO* CCommInf::routeInfo(const char* code,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	return routeInfo( routeNo(code),isValidCheck );
}
const DAC_ROUTE_INFO* CCommInf::routeInfo(hInt32 no/* =0 */,bool isValidCheck /* = true */) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return 0;

	if (!isValidCheck) 
	{
		if (no < 0 || no >= m_pComInfo->system.routeNum) return 0;
		return &m_pComInfo->route[no];
	}

	if (isRouteValid(no))
	{
		return &m_pComInfo->route[no];
	}

	return 0;
}
bool CCommInf::isChannelValid(hInt32 no) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return false;

	if (no < 0 || no >= m_pComInfo->system.channelNum) return false;

	if ( !m_pComPara->channel[no].valid ) return false;

	return true;
}
bool CCommInf::isStationValid(hInt32 no) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return false;

	if (no < 0 || no >= m_pComInfo->system.stationNum) return false;
	if ( !m_pComPara->station[no].valid ) return false;

	return true;
}

bool CCommInf::isSenddevValid(hInt32 no) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return false;

	if (no < 0 || no >= m_pComInfo->system.senddevNum) return false;
	if ( !m_pComPara->senddev[no].valid ) return false;

	return true;
}

bool CCommInf::isGroupValid(hInt32 no) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return false;

	if (no < 0 || no >= m_pComInfo->system.groupNum) return false;
	if ( !m_pComPara->group[no].valid ) return false;

	return true;
}
bool CCommInf::isRouteValid(hInt32 no) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return false;

	if (no < 0 || no >= m_pComInfo->system.routeNum) return false;

	if ( !m_pComPara->route[no].valid ) return false;

	return true;
}
bool CCommInf::isProtocolValid(hInt32 no) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return false;

	if (no < 0 || no >= DAC_PROTOCOL_NUM) return false;
	if ( !m_pComPara->protocol[no].valid ) return false;

	return true;
}
hInt32 CCommInf::stationNo(const char* code) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return -1;

	if (code == 0) return -1;

	for (hUInt32 i=0;i<m_pComInfo->system.stationNum;i++)
	{
		if ( strcmp(code,m_pComPara->station[i].code) == 0 )
			return i;
	}

	return -1;
}
hInt32 CCommInf::channelNo(const char* code) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return -1;

	if (code == 0) return -1;

	for (hUInt32 i=0;i<m_pComInfo->system.channelNum;i++)
	{
		if ( strcmp(code,m_pComPara->channel[i].code) == 0 )
			return i;
	}

	return -1;
}
hInt32 CCommInf::groupNo(const char* code) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return -1;

	if (code == 0) return -1;

	for (hUInt32 i=0;i<m_pComInfo->system.groupNum;i++)
	{
		if ( strcmp(code,m_pComPara->group[i].code) == 0 )
			return i;
	}

	return -1;
}
hInt32 CCommInf::routeNo(const char* code) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return -1;

	if (code == 0) return -1;

	for (hUInt32 i=0;i<m_pComInfo->system.routeNum;i++)
	{
		if ( strcmp(code,m_pComPara->route[i].code) == 0 )
			return i;
	}

	return -1;
}
hInt32 CCommInf::protocolNo(const char* code) const
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return -1;

	if (code == 0) return -1;

	for (hUInt32 i=0;i<DAC_PROTOCOL_NUM;i++)
	{
		if ( strcmp(code,m_pComPara->protocol[i].code) == 0 )
			return i;
	}

	return -1;
}

hInt32 CCommInf::groupNoOfRoute(hInt32 routeNo,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return -1;
	if (routeNo < 0 || routeNo >= m_pComInfo->system.routeNum) return -1;

	if (!isValidCheck) return m_pComPara->route[routeNo].group;

	if ( m_pComPara->route[routeNo].valid )
	{
		return m_pComPara->route[routeNo].group;
	}
	
	return -1;
}
hInt32 CCommInf::groupNoOfRoute(const char* routeName,bool isValidCheck /* = true */)
{
	if (m_pComPara == NULL || m_pComInfo == NULL) return -1;

	return groupNoOfRoute( routeNo(routeName),isValidCheck );
}

/*
* 函数功能:设置通道状态,若信息变化且需要保存到变化区,则设置变化区
*/
bool CCommInf::setChannelState(hInt32 no,hUInt8 state)
{
	DAC_CHANNEL_INFO* pLocalInfo = channelInfo(no,false);
	if (pLocalInfo == NULL)	return false;

	//设置信息
	pLocalInfo->state = state;

	//设置信息变化区
	if ( state != pLocalInfo->state )
	{
		hInt32 write = m_pComInfo->chgChannelWrite;
		m_pComInfo->chgChannel[write].no = no;
		memcpy( &m_pComInfo->chgChannel[write].info,pLocalInfo,sizeof(DAC_CHANNEL_INFO) ); 
		m_pComInfo->chgChannelWrite = (write + 1) % (2 * DAC_CHANNEL_NUM);
	}

	return true;
}

/*
* 函数功能:设置数据组状态,若信息变化且需要保存到变化区,则设置变化区
*/
bool CCommInf::setGroupState(hInt32 no,hUInt8 state)
{
	DAC_GROUP_INFO* pLocalInfo = groupInfo(no,false);
	if (pLocalInfo == NULL)	return false;

	//设置信息变化区
	if ( state != pLocalInfo->state )
	{
		pLocalInfo->state = state;
		hInt32 write = m_pComInfo->chgGroupWrite;
		m_pComInfo->chgGroup[write].no = no;
		memcpy( &m_pComInfo->chgGroup[write].info,pLocalInfo,sizeof(DAC_GROUP_INFO) ); 
		m_pComInfo->chgGroupWrite = (write + 1) % (2 * DAC_GROUP_NUM);
	}

	return true;
}

/*
* 函数功能:设置路径状态,若信息变化且需要保存到变化区,则设置变化区
*/
bool CCommInf::setRouteState(hInt32 no,hUInt8 state)
{
	DAC_ROUTE_INFO* pLocalInfo = routeInfo(no,false);
	if (pLocalInfo == NULL)	return false;

	//设置信息变化区
	if ( state != pLocalInfo->state )
	{
		pLocalInfo->state = state;
		hInt32 write = m_pComInfo->chgRouteWrite;
		m_pComInfo->chgRoute[write].no = no;
		memcpy( &m_pComInfo->chgRoute[write].info,pLocalInfo,sizeof(DAC_ROUTE_INFO) ); 
		m_pComInfo->chgRouteWrite = (write + 1) % (2 * DAC_ROUTE_NUM);
	}

	return true;
}

/*
 * 函数功能:设置路径人工停止标志,若信息变化且需要保存到变化区,则设置变化区
 */
bool CCommInf::setRouteManStop(hInt32 no,hBool manStop)
{
	DAC_ROUTE_INFO* pLocalInfo = routeInfo(no,false);
	if (pLocalInfo == NULL)	return false;

	//设置信息变化区
	if ( manStop != pLocalInfo->manStop )
	{
		pLocalInfo->manStop = manStop;
		hInt32 write = m_pComInfo->chgRouteWrite;
		m_pComInfo->chgRoute[write].no = no;
		memcpy( &m_pComInfo->chgRoute[write].info,pLocalInfo,sizeof(DAC_ROUTE_INFO) ); 
		m_pComInfo->chgRouteWrite = (write + 1) % (2 * DAC_ROUTE_NUM);
	}

	return true;
}

/*
 * 函数功能:可获取通道信息的拷贝
 */
bool CCommInf::getChannelInfo(hInt32 no,DAC_CHANNEL_INFO& info)
{
	DAC_CHANNEL_INFO* pLocalInfo = channelInfo(no,false);
	if (pLocalInfo == NULL)	return false;

	//获取信息
	memcpy(&info,pLocalInfo,sizeof(DAC_CHANNEL_INFO));

	return true;
}

/*
 * 函数功能:获取数据组信息的拷贝
 */
bool CCommInf::getGroupInfo(hInt32 no,DAC_GROUP_INFO& info)
{
	DAC_GROUP_INFO* pLocalInfo = groupInfo(no,false);
	if (pLocalInfo == NULL)	return false;

	//获取信息
	memcpy(&info,pLocalInfo,sizeof(DAC_GROUP_INFO));

	return true;
}

/*
 * 函数功能:获取路径信息
 */
bool CCommInf::getRouteInfo(hInt32 no,DAC_ROUTE_INFO& info)
{
	DAC_ROUTE_INFO* pLocalInfo = routeInfo(no,false);
	if (pLocalInfo == NULL)	return false;

	//获取信息
	memcpy(&info,pLocalInfo,sizeof(DAC_ROUTE_INFO));

	return true;
}

/*
 * 函数功能:设置通道信息,若信息变化且需要保存到变化区,则设置变化区
 */
bool CCommInf::setChannelInfo(hInt32 no,const DAC_CHANNEL_INFO& info)
{
	DAC_CHANNEL_INFO* pLocalInfo = channelInfo(no,false);
	if (pLocalInfo == NULL)	return false;

	//设置信息变化区
	if ( info.state != pLocalInfo->state )
	{
		//设置信息
		memcpy(pLocalInfo,&info,sizeof(DAC_CHANNEL_INFO));

		hInt32 write = m_pComInfo->chgChannelWrite;
		m_pComInfo->chgChannel[write].no = no;
		memcpy( &m_pComInfo->chgChannel[write].info,&info,sizeof(DAC_CHANNEL_INFO) ); 
		m_pComInfo->chgChannelWrite = (write + 1) % (2 * DAC_CHANNEL_NUM);
	}
	else 
	{
		//设置信息
		memcpy(pLocalInfo,&info,sizeof(DAC_CHANNEL_INFO));
	}

	return true;
}

/*
 * 函数功能:设置数据组信息,若信息变化且需要保存到变化区,则设置变化区
 */
bool CCommInf::setGroupInfo(hInt32 no,const DAC_GROUP_INFO& info)
{
	DAC_GROUP_INFO* pLocalInfo = groupInfo(no,false);
	if (pLocalInfo == NULL)	return false;

	//设置信息变化区
	if ( info.state != pLocalInfo->state )
	{
		//设置信息
		memcpy(pLocalInfo,&info,sizeof(DAC_GROUP_INFO));

		hInt32 write = m_pComInfo->chgGroupWrite;
		m_pComInfo->chgGroup[write].no = no;
		memcpy( &m_pComInfo->chgGroup[write].info,&info,sizeof(DAC_GROUP_INFO) ); 
		m_pComInfo->chgGroupWrite = (write + 1) % (2 * DAC_GROUP_NUM);
	}
	else
	{
		//设置信息
		memcpy(pLocalInfo,&info,sizeof(DAC_GROUP_INFO));
	}

	return true;
}

/*
 * 函数功能:设置路径信息,若信息变化且需要保存到变化区,则设置变化区
 */
bool CCommInf::setRouteInfo(hInt32 no,const DAC_ROUTE_INFO& info)
{
	DAC_ROUTE_INFO* pLocalInfo = routeInfo(no,false);
	if (pLocalInfo == NULL)	return false;

	//设置信息变化区
	if ( info.state != pLocalInfo->state 
		 ||info.lastUpTime != pLocalInfo->lastUpTime
		 ||info.lastDownTime != pLocalInfo->lastDownTime )
	{
		//设置信息
		memcpy(pLocalInfo,&info,sizeof(DAC_ROUTE_INFO));

		hInt32 write = m_pComInfo->chgRouteWrite;
		m_pComInfo->chgRoute[write].no = no;
		memcpy( &m_pComInfo->chgRoute[write].info,&info,sizeof(DAC_ROUTE_INFO) ); 
		m_pComInfo->chgRouteWrite = (write + 1) % (2 * DAC_ROUTE_NUM);
	}
	else
	{
		//设置信息
		memcpy(pLocalInfo,&info,sizeof(DAC_ROUTE_INFO));
	}

	return true;
}

bool CCommInf::hasChgChannelInfo()
{
	if (m_chgChannelRead == m_pComInfo->chgChannelWrite)	return false;

	return true;
}

bool CCommInf::hasChgGroupInfo()
{
	if (m_chgGroupRead == m_pComInfo->chgGroupWrite)	return false;

	return true;
}

bool CCommInf::hasChgRouteInfo()
{
	if (m_chgRouteRead == m_pComInfo->chgRouteWrite)	return false;

	return true;
}

bool CCommInf::chgChannelInfo(DAC_CHG_CHANNEL_INFO& info)
{
	if( !hasChgChannelInfo() )	return	false;

	m_chgChannelRead = (m_chgChannelRead + 1) % (2 * DAC_CHANNEL_NUM);
	memcpy(&info,&m_pComInfo->chgChannel[m_chgChannelRead],sizeof(DAC_CHG_CHANNEL_INFO));

	return true;
}

bool CCommInf::chgGroupInfo(DAC_CHG_GROUP_INFO& info)
{
	if( !hasChgGroupInfo() )	return	false;

	m_chgGroupRead = (m_chgGroupRead + 1) % (2 * DAC_GROUP_NUM);
	memcpy(&info,&m_pComInfo->chgGroup[m_chgGroupRead],sizeof(DAC_CHG_GROUP_INFO));

	return true;
}

bool CCommInf::chgRouteInfo(DAC_CHG_ROUTE_INFO& info)
{
	if( !hasChgRouteInfo() )	return	false;

	m_chgRouteRead = (m_chgRouteRead + 1) % (2 * DAC_ROUTE_NUM);
	memcpy(&info,&m_pComInfo->chgRoute[m_chgRouteRead],sizeof(DAC_CHG_ROUTE_INFO));

	return true;
}

void CCommInf::resetAllCommInfo()
{
	if ( !m_pComInfo )
		return;
	ACE_OS::memset( m_pComInfo,0,sizeof(DAC_COM_INFO) );
	//清空控制命令及反校
	//int size = sizeof(DAC_CTRL_CMD)*DAC_CTRL_NUM;
	//for (int i=0;i<DAC_CTRL_NUM;i++)
	//{
	//	ACE_OS::memset( &m_pComCtrl[i],0,sizeof(DAC_CTRL_CMD) );
	//	ACE_OS::memset( &m_pComCtrl[i],0,sizeof(DAC_CTRL_CMD));
	//}
	ACE_OS::memset( m_pComCtrl,0,sizeof(DAC_CTRL_CMD)*DAC_CTRL_NUM );
	ACE_OS::memset( m_pComAck,0,sizeof(DAC_CTRL_CMD)*DAC_CTRL_NUM );
	//for (hUInt32 i=0;i<DAC_GROUP_NUM;i++)
	//{
	//	m_pComInfo->group[i].mainRoute = -1;
	//}
}

DAC_YCCON* PDT::CCommInf::yccon( int ntype )
{
	if ( !m_pComPara )
		return NULL;
	DAC_YCCON* ptr = NULL;
	for (int i = 0; i < DAC_YK_NUM; i++)
	{
		ptr = m_pComPara->yccon + i;
		if (!ptr || !(ptr->valid))
		{
			break;
		}
		if (ptr->ntype == ntype)
		{
			return ptr;
		}
	}
	return NULL;
}

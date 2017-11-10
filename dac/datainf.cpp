/*==============================================================================
* 文件名称 : datainf.cpp
* 模块 : 前置数据访问接口
* 创建时间 : 2008-04-28 15:55:23
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "dac/datainf.h"
#include "dac/dacdef.h"
#include "sys/blog.h"
#include <math.h>
#include "event/type.h"

using namespace PDT;


CDataInf::CDataInf()
{
	init();
}

CDataInf::~CDataInf()
{
}
//------------------------------------------------------------------------------
void	CDataInf::init()
{
	memcpy(readYc,m_shm.m_ptrYC->write,DAC_ROUTE_NUM*sizeof(hInt32));
	memcpy(readYx,m_shm.m_ptrYX->write,DAC_ROUTE_NUM*sizeof(hInt32));
	memcpy(readKwh,m_shm.m_ptrKWH->write,DAC_ROUTE_NUM*sizeof(hInt32));
	memcpy(readSoe,m_shm.m_ptrSOE->write,DAC_ROUTE_NUM*sizeof(hInt32));
}
//------------------------------------------------------------------------------
hInt32	CDataInf::groupNo(const char *code)	const
{
	if(!code)	return	-1;
	DAC_GROUP	*pGroup;
	for(hUInt32 i=0;i<m_shm.m_ptrCom->info.system.groupNum;i++)
	{
		pGroup = m_shm.m_ptrCom->para.group + i;
		if(pGroup->valid && strcmp(code,pGroup->code)==0)	return	i;
	}
	return	-1;
}

hInt32	CDataInf::routeNo(const char *code)	const
{
	if(!code)	return	-1;
	DAC_ROUTE	*pRoute;
	for(hUInt32 i=0;i<m_shm.m_ptrCom->info.system.routeNum;i++)
	{
		pRoute = m_shm.m_ptrCom->para.route + i;
		if(pRoute->valid && strcmp(code,pRoute->code)==0)	return	i;
	}
	return	-1;
}

hInt32	CDataInf::groupOfRoute(hInt32 route)	const
{
	if(route<0 || route>=m_shm.m_ptrCom->info.system.routeNum || !m_shm.m_ptrCom->para.route[route].valid)	return	-1;
	return	m_shm.m_ptrCom->para.route[route].group;
}

hInt32	CDataInf::groupOfRoute(const char *route)	const
{
	return	groupOfRoute(routeNo(route));
}
//------------------------------------------------------------------------------
hInt32	CDataInf::ycNo(hInt32 group,const char *code)	const
{
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	-1;
	if(!code)	return	-1;
	DAC_GROUP		*pGroup = m_shm.m_ptrCom->para.group + group;
	const	DAC_YC	*pYc = m_shm.ycPara(group);
	for(hUInt32 i=0;i<pGroup->ycNum;i++)
	{
		if(strcmp(code,pYc[i].code) == 0)	return	i;
	}
	return	-1;
}

hInt32	CDataInf::ycNo(const char *group,const char *code)	const
{
	return	ycNo(groupNo(group),code);
}

hInt32	CDataInf::yxNo(hInt32 group,const char *code)	const
{
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	-1;
	if(!code)	return	-1;
	DAC_GROUP		*pGroup = m_shm.m_ptrCom->para.group + group;
	const	DAC_YX	*pYx = m_shm.yxPara(group);
	for(hUInt32 i=0;i<pGroup->yxNum;i++)
	{
		if(strcmp(code,pYx[i].code) == 0)	return	i;
	}
	return	-1;
}

hInt32	CDataInf::yxNo(const char *group,const char *code)	const
{
	return	yxNo(groupNo(group),code);
}

hInt32	CDataInf::kwhNo(hInt32 group,const char *code)	const
{
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	-1;
	if(!code)	return	-1;
	DAC_GROUP		*pGroup = m_shm.m_ptrCom->para.group + group;
	const	DAC_KWH	*pKwh = m_shm.kwhPara(group);
	for(hUInt32 i=0;i<pGroup->kwhNum;i++)
	{
		if(strcmp(code,pKwh[i].code) == 0)	return	i;
	}
	return	-1;
}

hInt32	CDataInf::kwhNo(const char *group,const char *code)	const
{
	return	kwhNo(groupNo(group),code);
}
//------------------------------------------------------------------------------
DAC_YC*	CDataInf::ycPara(hInt32 group,hInt32 no)
{
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	0;
	if(no<0 || no>=m_shm.m_ptrCom->para.group[group].ycNum)	return	0;
	DAC_YC	*pYc = m_shm.ycPara(group);
	if(!pYc)	return	0;
	return	(pYc+no);
}

DAC_YC*	CDataInf::ycPara(hInt32 group,const char *code)
{
	return	ycPara(group,ycNo(group,code));
}

DAC_YC*	CDataInf::ycPara(const char *group,hInt32 no)
{
	return	ycPara(groupNo(group),no);
}

DAC_YC*	CDataInf::ycPara(const char *group,const char *code)
{
	return	ycPara(groupNo(group),ycNo(group,code));
}

const	DAC_YC*	CDataInf::ycPara(hInt32 group,hInt32 no)	const
{
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	0;
	if(no<0 || no>=m_shm.m_ptrCom->para.group[group].ycNum)	return	0;
	const	DAC_YC	*pYc = m_shm.ycPara(group);
	if(!pYc)	return	0;
	return	(pYc+no);
}

const	DAC_YC*	CDataInf::ycPara(hInt32 group,const char *code)	const
{
	return	ycPara(group,ycNo(group,code));
}

const	DAC_YC*	CDataInf::ycPara(const char *group,hInt32 no)	const
{
	return	ycPara(groupNo(group),no);
}

const	DAC_YC*	CDataInf::ycPara(const char *group,const char *code)	const
{
	return	ycPara(groupNo(group),ycNo(group,code));
}
//------------------------------------------------------------------------------
DAC_YX*	CDataInf::yxPara(hInt32 group,hInt32 no)
{
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	0;
	if(no<0 || no>=m_shm.m_ptrCom->para.group[group].yxNum)	return	0;
	DAC_YX	*pYx = m_shm.yxPara(group);
	if(!pYx)	return	0;
	return	(pYx+no);
}

DAC_YX*	CDataInf::yxPara(hInt32 group,const char *code)
{
	return	yxPara(group,yxNo(group,code));
}

DAC_YX*	CDataInf::yxPara(const char *group,hInt32 no)
{
	return	yxPara(groupNo(group),no);
}

DAC_YX*	CDataInf::yxPara(const char *group,const char *code)
{
	return	yxPara(groupNo(group),yxNo(group,code));
}

const	DAC_YX*	CDataInf::yxPara(hInt32 group,hInt32 no)	const
{
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	0;
	if(no<0 || no>=m_shm.m_ptrCom->para.group[group].yxNum)	return	0;
	const	DAC_YX	*pYx = m_shm.yxPara(group);
	if(!pYx)	return	0;
	return	(pYx+no);
}

const	DAC_YX*	CDataInf::yxPara(hInt32 group,const char *code)	const
{
	return	yxPara(group,yxNo(group,code));
}

const	DAC_YX*	CDataInf::yxPara(const char *group,hInt32 no)	const
{
	return	yxPara(groupNo(group),no);
}

const	DAC_YX*	CDataInf::yxPara(const char *group,const char *code)	const
{
	return	yxPara(groupNo(group),yxNo(group,code));
}
//------------------------------------------------------------------------------
DAC_KWH*	CDataInf::kwhPara(hInt32 group,hInt32 no)
{
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	0;
	if(no<0 || no>=m_shm.m_ptrCom->para.group[group].kwhNum)	return	0;
	DAC_KWH	*pKwh = m_shm.kwhPara(group);
	if(!pKwh)	return	0;
	return	(pKwh+no);
}

DAC_KWH*	CDataInf::kwhPara(hInt32 group,const char *code)
{
	return	kwhPara(group,kwhNo(group,code));
}

DAC_KWH*	CDataInf::kwhPara(const char *group,hInt32 no)
{
	return	kwhPara(groupNo(group),no);
}

DAC_KWH*	CDataInf::kwhPara(const char *group,const char *code)
{
	return	kwhPara(groupNo(group),kwhNo(group,code));
}

const	DAC_KWH*	CDataInf::kwhPara(hInt32 group,hInt32 no)	const
{
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	0;
	if(no<0 || no>=m_shm.m_ptrCom->para.group[group].kwhNum)	return	0;
	const	DAC_KWH	*pKwh = m_shm.kwhPara(group);
	if(!pKwh)	return	0;
	return	(pKwh+no);
}

const	DAC_KWH*	CDataInf::kwhPara(hInt32 group,const char *code)	const
{
	return	kwhPara(group,kwhNo(group,code));
}

const	DAC_KWH*	CDataInf::kwhPara(const char *group,hInt32 no)	const
{
	return	kwhPara(groupNo(group),no);
}

const	DAC_KWH*	CDataInf::kwhPara(const char *group,const char *code)	const
{
	return	kwhPara(groupNo(group),kwhNo(group,code));
}
//------------------------------------------------------------------------------
DAC_YC_DATA*	CDataInf::ycData(hInt32 route,hInt32 no)
{
	hInt32	group = groupOfRoute(route);
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	0;
	if(no<0 || no>=m_shm.m_ptrCom->para.group[group].ycNum)	return	0;
	DAC_YC_DATA	*pYcData = m_shm.ycData(route);
	if(!pYcData)	return	0;
	return	(pYcData+no);
}

DAC_YC_DATA*	CDataInf::ycData(hInt32 route,const char *code)
{
	return	ycData(route,ycNo(groupOfRoute(route),code));
}

DAC_YC_DATA*	CDataInf::ycData(const char *route,hInt32 no)
{
	return	ycData(routeNo(route),no);
}

DAC_YC_DATA*	CDataInf::ycData(const char *route,const char *code)
{
	return	ycData(routeNo(route),ycNo(groupOfRoute(route),code));
}

const	DAC_YC_DATA*	CDataInf::ycData(hInt32 route,hInt32 no)	const
{
	hInt32	group = groupOfRoute(route);
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	0;
	if(no<0 || no>=m_shm.m_ptrCom->para.group[group].ycNum)	return	0;
	const	DAC_YC_DATA	*pYcData = m_shm.ycData(route);
	if(!pYcData)	return	0;
	return	(pYcData+no);
}

const	DAC_YC_DATA*	CDataInf::ycData(hInt32 route,const char *code)	const
{
	return	ycData(route,ycNo(groupOfRoute(route),code));
}

const	DAC_YC_DATA*	CDataInf::ycData(const char *route,hInt32 no)	const
{
	return	ycData(routeNo(route),no);
}

const	DAC_YC_DATA*	CDataInf::ycData(const char *route,const char *code)	const
{
	return	ycData(routeNo(route),ycNo(groupOfRoute(route),code));
}

DAC_YX_DATA*	CDataInf::yxData(hInt32 route,hInt32 no)
{
	hInt32	group = groupOfRoute(route);
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	0;
	if(no<0 || no>=m_shm.m_ptrCom->para.group[group].yxNum)	return	0;
	DAC_YX_DATA	*pYxData = m_shm.yxData(route);
	if(!pYxData)	return	0;
	return	(pYxData+no);
}

DAC_YX_DATA*	CDataInf::yxData(hInt32 route,const char *code)
{
	return	yxData(route,yxNo(groupOfRoute(route),code));
}

DAC_YX_DATA*	CDataInf::yxData(const char *route,hInt32 no)
{
	return	yxData(routeNo(route),no);
}

DAC_YX_DATA*	CDataInf::yxData(const char *route,const char *code)
{
	return	yxData(routeNo(route),yxNo(groupOfRoute(route),code));
}

const	DAC_YX_DATA*	CDataInf::yxData(hInt32 route,hInt32 no)	const
{
	hInt32	group = groupOfRoute(route);
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	0;
	if(no<0 || no>=m_shm.m_ptrCom->para.group[group].yxNum)	return	0;
	const	DAC_YX_DATA	*pYxData = m_shm.yxData(route);
	if(!pYxData)	return	0;
	return	(pYxData+no);
}

const	DAC_YX_DATA*	CDataInf::yxData(hInt32 route,const char *code)	const
{
	return	yxData(route,yxNo(groupOfRoute(route),code));
}

const	DAC_YX_DATA*	CDataInf::yxData(const char *route,hInt32 no)	const
{
	return	yxData(routeNo(route),no);
}

const	DAC_YX_DATA*	CDataInf::yxData(const char *route,const char *code)	const
{
	return	yxData(routeNo(route),yxNo(groupOfRoute(route),code));
}

DAC_KWH_DATA*	CDataInf::kwhData(hInt32 route,hInt32 no)
{
	hInt32	group = groupOfRoute(route);
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	0;
	if(no<0 || no>=m_shm.m_ptrCom->para.group[group].kwhNum)	return	0;
	DAC_KWH_DATA	*pKwhData = m_shm.kwhData(route);
	if(!pKwhData)	return	0;
	return	(pKwhData+no);
}

DAC_KWH_DATA*	CDataInf::kwhData(hInt32 route,const char *code)
{
	return	kwhData(route,kwhNo(groupOfRoute(route),code));
}

DAC_KWH_DATA*	CDataInf::kwhData(const char *route,hInt32 no)
{
	return	kwhData(routeNo(route),no);
}

DAC_KWH_DATA*	CDataInf::kwhData(const char *route,const char *code)
{
	return	kwhData(routeNo(route),kwhNo(groupOfRoute(route),code));
}

const	DAC_KWH_DATA*	CDataInf::kwhData(hInt32 route,hInt32 no)	const
{
	hInt32	group = groupOfRoute(route);
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum || !m_shm.m_ptrCom->para.group[group].valid)	return	0;
	if(no<0 || no>=m_shm.m_ptrCom->para.group[group].kwhNum)	return	0;
	const	DAC_KWH_DATA	*pKwhData = m_shm.kwhData(route);
	if(!pKwhData)	return	0;
	return	(pKwhData+no);
}

const	DAC_KWH_DATA*	CDataInf::kwhData(hInt32 route,const char *code)	const
{
	return	kwhData(route,kwhNo(groupOfRoute(route),code));
}

const	DAC_KWH_DATA*	CDataInf::kwhData(const char *route,hInt32 no)	const
{
	return	kwhData(routeNo(route),no);
}

const	DAC_KWH_DATA*	CDataInf::kwhData(const char *route,const char *code)	const
{
	return	kwhData(routeNo(route),kwhNo(groupOfRoute(route),code));
}

const	DAC_SOE_DATA*	CDataInf::soeData(hInt32 route)
{
	if(!hasSoe(route))	return	0;
	hInt32		group = groupOfRoute(route);
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum)	return	0;
	if(!m_shm.m_ptrCom->para.group[group].valid)	return	0;
	hInt32	read = readSoe[route];
	readSoe[route] = (read + 1) % (2 * m_shm.m_ptrSOE->mapInfo[group].num);
	return	(m_shm.soeData(route)+read);
}

const	DAC_SOE_DATA*	CDataInf::soeData(const char *route)
{
	return	soeData(routeNo(route));
}

const	DAC_YC_CHG_DATA*	CDataInf::ycChgData(hInt32 route)
{
	if(!hasChgYc(route))	return	0;
	hInt32		group = groupOfRoute(route);
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum)	return	0;
	if(!m_shm.m_ptrCom->para.group[group].valid)	return	0;
	hInt32	read = readYc[route];
	readYc[route] = (read + 1) % (2 * m_shm.m_ptrYC->mapInfo[group].num);
	return	(m_shm.ycChgData(route)+read);
}

const	DAC_YC_CHG_DATA*	CDataInf::ycChgData(const char *route)
{
	return	ycChgData(routeNo(route));
}

const	DAC_YX_CHG_DATA*	CDataInf::yxChgData(hInt32 route)
{
	if(!hasChgYx(route))	return	0;
	hInt32		group = groupOfRoute(route);
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum)	return	0;
	if(!m_shm.m_ptrCom->para.group[group].valid)	return	0;
	hInt32	read = readYx[route];
	readYx[route] = (read + 1) % (2 * m_shm.m_ptrYX->mapInfo[group].num);
	return	(m_shm.yxChgData(route)+read);
}

const	DAC_YX_CHG_DATA*	CDataInf::yxChgData(const char *route)
{
	return	yxChgData(routeNo(route));
}

const	DAC_KWH_CHG_DATA*	CDataInf::kwhChgData(hInt32 route)
{
	if(!hasChgKwh(route))	return	0;
	hInt32		group = groupOfRoute(route);
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum)	return	0;
	if(!m_shm.m_ptrCom->para.group[group].valid)	return	0;
	hInt32	read = readKwh[route];
	readKwh[route] = (read + 1) % (2 * m_shm.m_ptrKWH->mapInfo[group].num);
	return	(m_shm.kwhChgData(route)+read);
}

const	DAC_KWH_CHG_DATA*	CDataInf::kwhChgData(const char *route)
{
	return	kwhChgData(routeNo(route));
}

bool	CDataInf::yc(hInt32 route,hInt32 no,DAC_YC_DATA &val)	const
{
	const	DAC_YC_DATA	*pData = ycData(route,no);
	if(!pData)	return	false;
	memcpy(&val,pData,sizeof(DAC_YC_DATA));
	return	true;
}

bool	CDataInf::yc(hInt32 route,const char *code,DAC_YC_DATA &val)	const
{
	return	yc(route,ycNo(groupOfRoute(route),code),val);
}

bool	CDataInf::yc(const char *route,hInt32 no,DAC_YC_DATA &val)	const
{
	return	yc(routeNo(route),no,val);
}

bool	CDataInf::yc(const char *route,const char *code,DAC_YC_DATA &val)	const
{
	return	yc(routeNo(route),ycNo(groupOfRoute(route),code),val);
}

bool	CDataInf::ycMainRoute(hInt32 group,hInt32 no,DAC_YC_DATA &val)	const
{
	return yc( mainRoute(group),no,val );
}

bool	CDataInf::yx(hInt32 route,hInt32 no,DAC_YX_DATA &val)	const
{
	const	DAC_YX_DATA	*pData = yxData(route,no);
	if(!pData)	return	false;
	memcpy(&val,pData,sizeof(DAC_YX_DATA));
	return	true;
}

bool	CDataInf::yx(hInt32 route,const char *code,DAC_YX_DATA &val)	const
{
	return	yx(route,yxNo(groupOfRoute(route),code),val);
}

bool	CDataInf::yx(const char *route,hInt32 no,DAC_YX_DATA &val)	const
{
	return	yx(routeNo(route),no,val);
}

bool	CDataInf::yx(const char *route,const char *code,DAC_YX_DATA &val)	const
{
	return	yx(routeNo(route),yxNo(groupOfRoute(route),code),val);
}

bool	CDataInf::yxMainRoute(hInt32 group,hInt32 yxNo,DAC_YX_DATA &val)	const
{
	return yx( mainRoute(group),yxNo,val );
}

bool	CDataInf::kwh(hInt32 route,hInt32 no,DAC_KWH_DATA &val)	const
{
	const	DAC_KWH_DATA	*pData = kwhData(route,no);
	if(!pData)	return	false;
	memcpy(&val,pData,sizeof(DAC_KWH_DATA));
	return	true;
}

bool	CDataInf::kwh(hInt32 route,const char *code,DAC_KWH_DATA &val)	const
{
	return	kwh(route,kwhNo(groupOfRoute(route),code),val);
}

bool	CDataInf::kwh(const char *route,hInt32 no,DAC_KWH_DATA &val)	const
{
	return	kwh(routeNo(route),no,val);
}

bool	CDataInf::kwh(const char *route,const char *code,DAC_KWH_DATA &val)	const
{
	return	kwh(routeNo(route),kwhNo(groupOfRoute(route),code),val);
}

bool CDataInf::kwhMainRoute(hInt32 group,hInt32 kwhNo,DAC_KWH_DATA &val)	const
{
	return kwh( mainRoute(group),kwhNo,val );
}

bool	CDataInf::soe(hInt32 route,DAC_SOE_DATA &val)
{
	const	DAC_SOE_DATA	*pData = soeData(route);
	if(!pData)	return	false;
	memcpy(&val,pData,sizeof(DAC_SOE_DATA));
	return	true;
}

bool	CDataInf::soe(const char *route,DAC_SOE_DATA &val)
{
	return	soe(routeNo(route),val);
}

bool	CDataInf::soeMainRoute(hInt32 group,DAC_SOE_DATA &val)
{
	return soe( mainRoute(group),val );
}

bool	CDataInf::chgYc(hInt32 route,DAC_YC_CHG_DATA &val)
{
	const	DAC_YC_CHG_DATA	*pData = ycChgData(route);
	if(!pData)	return	false;
	memcpy(&val,pData,sizeof(DAC_YC_CHG_DATA));
	return	true;
}

bool	CDataInf::chgYc(const char *route,DAC_YC_CHG_DATA &val)
{
	return	chgYc(routeNo(route),val);
}

bool	CDataInf::chgYcMainRoute(hInt32 group,DAC_YC_CHG_DATA &val)
{
	return chgYc( mainRoute(group),val );
}

bool	CDataInf::chgYx(hInt32 route,DAC_YX_CHG_DATA &val)
{
	const	DAC_YX_CHG_DATA	*pData = yxChgData(route);
	if(!pData)	return	false;
	memcpy(&val,pData,sizeof(DAC_YX_CHG_DATA));
	return	true;
}

bool	CDataInf::chgYx(const char *route,DAC_YX_CHG_DATA &val)
{
	return	chgYx(routeNo(route),val);
}

bool	CDataInf::chgYxMainRoute(hInt32 group,DAC_YX_CHG_DATA &val)
{
	return chgYx( mainRoute(group),val );
}

bool	CDataInf::chgKwh(hInt32 route,DAC_KWH_CHG_DATA &val)
{
	const	DAC_KWH_CHG_DATA	*pData = kwhChgData(route);
	if(!pData)	return	false;
	memcpy(&val,pData,sizeof(DAC_KWH_CHG_DATA));
	return	true;
}

bool	CDataInf::chgKwh(const char *route,DAC_KWH_CHG_DATA &val)
{
	return	chgKwh(routeNo(route),val);
}

bool	CDataInf::chgKwhMainRoute(hInt32 group,DAC_KWH_CHG_DATA &val)
{
	return chgKwh( mainRoute(group),val );
}

bool	CDataInf::setYc(hInt32 route,hInt32 no,const DAC_YC_DATA &val,bool isRaw/* =true */)
{
	DAC_ROUTE* pRoute = m_shm.m_ptrCom->para.route + route;
	DAC_GROUP* pGroup = m_shm.m_ptrCom->para.group + pRoute->group;
	DAC_GROUP_INFO* pGroupInfo = m_shm.m_ptrCom->info.group + pRoute->group;
	if ( !pGroup->valid || pGroupInfo->manStop ) return false;
	if ( !ycPara(pRoute->group,no) || !ycPara(pRoute->group,no)->valid ) return false;

	DAC_YC_DATA	*pData = ycData(route,no);
	if(!pData)	return	false;
	if(memcmp(pData,&val,sizeof(DAC_YC_DATA)) == 0)	return	true;
	nFloat lastVal = pData->val;
	bool isInit = PDT_BIT_ENABLED( (hUInt32)pData->quality,QUALITY_BIT_INIT );

	//设置数据
	if ( filter(pRoute->group,no,pData,val) ) return true;
	pData->quality = val.quality;
	pData->updateTime = val.updateTime;
	//memcpy(pData,&val,sizeof(DAC_YC_DATA));
	if(pData->updateTime == 0)	pData->updateTime = time(0);

	//工程量转换
	if ( isRaw )
	{
		float coefk = ycPara(pRoute->group,no)->coefK;
		float coefb = ycPara(pRoute->group,no)->coefB;
		if ( PDT_ISZERO(coefk) ) coefk = 1.0;
		if ( PDT_ISZERO(coefb) ) coefb = 0;
		pData->val = pData->val * coefk + coefb;
	}

	//设置变化数据
	hInt32	write = m_shm.m_ptrYC->write[route];
	DAC_YC_CHG_DATA	*pChgData = m_shm.ycChgData(route) + write;
	pChgData->no = no;
	pChgData->lastVal = lastVal;
	memcpy(&pChgData->data,pData,sizeof(DAC_YC_DATA));
	hUInt32 quality = pChgData->data.quality;
	if ( isInit )
	{//第一帧数据需通知变数据为init
		PDT_SET_BITS(quality,QUALITY_BIT_INIT);
		pChgData->data.quality = quality;
	}
	m_shm.m_ptrYC->write[route] = ( write + 1 ) % (2 * m_shm.m_ptrYC->mapInfo[m_shm.m_ptrCom->para.route[route].group].num);

	//清空初始化标记
	quality = pData->quality;
	if ( PDT_BIT_ENABLED(quality,QUALITY_BIT_INIT) )
	{
		PDT_CLR_BITS(quality,QUALITY_BIT_INIT);
		pData->quality = quality;
	}
	return	true;
}

bool	CDataInf::setYc(hInt32 route,const char *code,const DAC_YC_DATA &val,bool isRaw/* =true */)
{
	return	setYc(route,ycNo(groupOfRoute(route),code),val,isRaw);
}

bool	CDataInf::setYc(const char *route,hInt32 no,const DAC_YC_DATA &val,bool isRaw/* =true */)
{
	return	setYc(routeNo(route),no,val,isRaw);
}

bool	CDataInf::setYc(const char *route,const char *code,const DAC_YC_DATA &val,bool isRaw/* =true */)
{
	return	setYc(routeNo(route),ycNo(groupOfRoute(route),code),val,isRaw);
}

bool	CDataInf::setYcMainRoute(hInt32 group,hInt32 no,const DAC_YC_DATA &val,bool isRaw/* =true */)
{
	return setYc( mainRoute(group),no,val,isRaw );
}

bool	CDataInf::setYx(hInt32 route,hInt32 no,const DAC_YX_DATA &val,bool isRaw/* =true */,bool eventFlag/* =false */)
{
	DAC_ROUTE* pRoute = m_shm.m_ptrCom->para.route + route;
	DAC_GROUP* pGroup = m_shm.m_ptrCom->para.group + pRoute->group;
	DAC_GROUP_INFO* pGroupInfo = m_shm.m_ptrCom->info.group + pRoute->group;
	if ( !pGroup->valid || pGroupInfo->manStop ) return false;
	if ( !yxPara(pRoute->group,no) || !yxPara(pRoute->group,no)->valid ) return false;

	DAC_YX_DATA	*pData = yxData(route,no);
	if(!pData)	return	false;
	if(memcmp(pData,&val,sizeof(DAC_YX_DATA)) == 0)	return	true;
	bool isInit = PDT_BIT_ENABLED( (hUInt32)pData->quality,QUALITY_BIT_INIT );
	//产生事项
	if(eventFlag)	createYxEvent(m_shm.m_ptrCom->para.route[route].group,no,*pData,val);
	//设置数据
	nUInt8 lastVal = pData->val;
	memcpy(pData,&val,sizeof(DAC_YX_DATA));
	if(pData->updateTime == 0)	pData->updateTime = time(0);
	
	//工程量转换
	if ( isRaw )
	{
		bool reverse = yxPara(pRoute->group,no)->reverse;
		if ( reverse ) 
			pData->val = (pData->val==0 ? 1:0);
	}

	//设置变化数据
	hInt32	write = m_shm.m_ptrYX->write[route];
	DAC_YX_CHG_DATA	*pChgData = m_shm.yxChgData(route) + write;
	pChgData->no = no;
	pChgData->lastVal = lastVal;
	memcpy(&pChgData->data,pData,sizeof(DAC_YX_DATA));
	hUInt32 quality = pChgData->data.quality;
	if ( isInit )
	{//第一帧数据需通知变数据为init
		PDT_SET_BITS(quality,QUALITY_BIT_INIT);
		pChgData->data.quality = quality;
	}
	m_shm.m_ptrYX->write[route] = ( write +1 ) % (2 * m_shm.m_ptrYX->mapInfo[m_shm.m_ptrCom->para.route[route].group].num);

	//清空初始化标记
	quality = pData->quality;
	if ( PDT_BIT_ENABLED(quality,QUALITY_BIT_INIT) )
	{
		PDT_CLR_BITS(quality,QUALITY_BIT_INIT);
		pData->quality = quality;
	}
	return	true;
}

bool	CDataInf::setYx(hInt32 route,const char *code,const DAC_YX_DATA &val,bool isRaw/* =true */,bool eventFlag/* =false */)
{
	return	setYx(route,yxNo(groupOfRoute(route),code),val,isRaw,eventFlag);
}

bool	CDataInf::setYx(const char *route,hInt32 no,const DAC_YX_DATA &val,bool isRaw/* =true */,bool eventFlag/* =false */)
{
	return	setYx(routeNo(route),no,val,isRaw,eventFlag);
}

bool	CDataInf::setYx(const char *route,const char *code,const DAC_YX_DATA &val,bool isRaw/* =true */,bool eventFlag/* =false */)
{
	return	setYx(routeNo(route),yxNo(groupOfRoute(route),code),val,isRaw,eventFlag);
}

bool	CDataInf::setYxMainRoute(hInt32 group,hInt32 no,const DAC_YX_DATA &val,bool isRaw/* =true */,bool eventFlag/* =false */)
{
	return setYx( mainRoute(group),no,val,isRaw,eventFlag );
}

bool	CDataInf::setKwh(hInt32 route,hInt32 no,const DAC_KWH_DATA &val,bool isRaw/* =true */)
{
	DAC_ROUTE* pRoute = m_shm.m_ptrCom->para.route + route;
	DAC_GROUP* pGroup = m_shm.m_ptrCom->para.group + pRoute->group;
	DAC_GROUP_INFO* pGroupInfo = m_shm.m_ptrCom->info.group + pRoute->group;
	if ( !pGroup->valid || pGroupInfo->manStop ) return false;
	if ( !kwhPara(pRoute->group,no) || !kwhPara(pRoute->group,no)->valid ) return false;

	DAC_KWH_DATA	*pData = kwhData(route,no);
	if(!pData)	return	false;
	if(memcmp(pData,&val,sizeof(DAC_KWH_DATA)) == 0)	return	true;
	nDouble lastVal = pData->val;
	bool isInit = PDT_BIT_ENABLED( (hUInt32)pData->quality,QUALITY_BIT_INIT );

	//设置数据
	memcpy(pData,&val,sizeof(DAC_KWH_DATA));
	if(pData->updateTime == 0)	pData->updateTime = time(0);

	//工程量转换
	if ( isRaw )
	{
		float coefk = kwhPara(pRoute->group,no)->coefK;
		float coefb = kwhPara(pRoute->group,no)->coefB;
		if ( PDT_ISZERO(coefk) ) coefk = 1.0;
		if ( PDT_ISZERO(coefb) ) coefb = 0;
		pData->val = pData->val * coefk + coefb;
	}

	//设置变化数据
	hInt32	write = m_shm.m_ptrKWH->write[route];
	DAC_KWH_CHG_DATA	*pChgData = m_shm.kwhChgData(route) + write;
	pChgData->no = no;
	pChgData->lastVal = lastVal;
	memcpy(&pChgData->data,pData,sizeof(DAC_KWH_DATA));
	hUInt32 quality = pChgData->data.quality;
	if ( isInit )
	{//第一帧数据需通知变数据为init
		PDT_SET_BITS(quality,QUALITY_BIT_INIT);
		pChgData->data.quality = quality;
	}
	m_shm.m_ptrKWH->write[route] = (write + 1) % (2 * m_shm.m_ptrKWH->mapInfo[m_shm.m_ptrCom->para.route[route].group].num);

	//清空初始化标记
	quality = pData->quality;
	if ( PDT_BIT_ENABLED(quality,QUALITY_BIT_INIT) )
	{
		PDT_CLR_BITS(quality,QUALITY_BIT_INIT);
		pData->quality = quality;
	}
	return	true;
}

bool	CDataInf::setKwh(hInt32 route,const char *code,const DAC_KWH_DATA &val,bool isRaw/* =true */)
{
	return	setKwh(route,kwhNo(groupOfRoute(route),code),val,isRaw);
}

bool	CDataInf::setKwh(const char *route,hInt32 no,const DAC_KWH_DATA &val,bool isRaw/* =true */)
{
	return	setKwh(routeNo(route),no,val,isRaw);
}

bool	CDataInf::setKwh(const char *route,const char *code,const DAC_KWH_DATA &val,bool isRaw/* =true */)
{
	return	setKwh(routeNo(route),kwhNo(groupOfRoute(route),code),val,isRaw);
}

bool	CDataInf::setKwhMainRoute(hInt32 group,hInt32 no,const DAC_KWH_DATA &val,bool isRaw/* =true */)
{
	return setKwh( mainRoute(group),no,val,isRaw );
}

bool	CDataInf::setSoe(hInt32 route,const DAC_SOE_DATA &val,bool valueFlag/* =false */,bool isRaw/* =true */,bool eventFlag/* =false */)
{
	hInt32	group = groupOfRoute(route);
	if(group<0 || group>=m_shm.m_ptrCom->info.system.groupNum)	return	false;

	DAC_ROUTE* pRoute = m_shm.m_ptrCom->para.route + route;
	DAC_GROUP* pGroup = m_shm.m_ptrCom->para.group + group;
	DAC_GROUP_INFO* pGroupInfo = m_shm.m_ptrCom->info.group + group;
	if ( !pGroup->valid || pGroupInfo->manStop ) return false;

	hInt32	write = m_shm.m_ptrSOE->write[route];
	DAC_SOE_DATA	*pData = m_shm.soeData(route) + write;
	memcpy(pData,&val,sizeof(DAC_SOE_DATA));
	m_shm.m_ptrSOE->write[route] = (write + 1) % (2 * m_shm.m_ptrSOE->mapInfo[group].num);
	if(valueFlag)
	{
		DAC_YX_DATA	data;
		memset(&data,0,sizeof(DAC_YX_DATA));
		data.val = val.val;
		data.quality = val.quality;
		setYx(route,val.no,data,isRaw);
	}
	if(eventFlag)	createSoeEvent(group,val);
	return	true;
}

bool	CDataInf::setSoe(const char *route,const DAC_SOE_DATA &val,bool valueFlag/* =false */,bool isRaw/* =true */,bool eventFlag/* =false */)
{
	return	setSoe(routeNo(route),val,valueFlag,isRaw,eventFlag);
}

bool	CDataInf::setSoeMainRoute(hInt32 group,const DAC_SOE_DATA &val,bool valueFlag/* =false */,bool isRaw/* =true */,bool eventFlag/* =false */)
{
	return setSoe( mainRoute(group),val,valueFlag,isRaw,eventFlag );
}

bool	CDataInf::hasSoe(hInt32 route)
{
	if(route<0 || route>=m_shm.m_ptrCom->info.system.routeNum || !m_shm.m_ptrCom->para.route[route].valid)	return	false;
	if(readSoe[route] == m_shm.m_ptrSOE->write[route])	return	false;
	return	true;
}

bool	CDataInf::hasSoe(const char *route)
{
	return	hasSoe(routeNo(route));
}

bool	CDataInf::hasSoeMainRoute(hInt32 group)
{
	return hasSoe( mainRoute(group) );
}

bool	CDataInf::hasChgYc(hInt32 route)
{
	if(route<0 || route>=m_shm.m_ptrCom->info.system.routeNum || !m_shm.m_ptrCom->para.route[route].valid)	return	false;
	if(readYc[route] == m_shm.m_ptrYC->write[route])	return	false;
	return	true;
}

bool	CDataInf::hasChgYc(const char *route)
{
	return	hasChgYc(routeNo(route));
}

bool	CDataInf::hasChgYcMainRoute(hInt32 group)
{
	return hasChgYc( mainRoute(group) );
}

bool	CDataInf::hasChgYx(hInt32 route)
{
	if(route<0 || route>=m_shm.m_ptrCom->info.system.routeNum || !m_shm.m_ptrCom->para.route[route].valid)	return	false;
	if(readYx[route] == m_shm.m_ptrYX->write[route])	return	false;
	return	true;
}

bool	CDataInf::hasChgYx(const char *route)
{
	return	hasChgYx(routeNo(route));
}

bool	CDataInf::hasChgYxMainRoute(hInt32 group)
{
	return hasChgYx( mainRoute(group) );
}

bool	CDataInf::hasChgKwh(hInt32 route)
{
	if(route<0 || route>=m_shm.m_ptrCom->info.system.routeNum || !m_shm.m_ptrCom->para.route[route].valid)	return	false;
	if(readKwh[route] == m_shm.m_ptrKWH->write[route])	return	false;
	return	true;
}

bool	CDataInf::hasChgKwh(const char *route)
{
	return	hasChgKwh(routeNo(route));
}

bool	CDataInf::hasChgKwhMainRoute(hInt32 group)
{
	return hasChgKwh( mainRoute(group) );
}

void CDataInf::resetAllDataPara()
{
	hUInt32 num = 0;
	hUInt8 limitType = 0;
	for (hUInt32 i=0;i<DAC_GROUP_NUM;i++)
	{
		//yc
		//num = m_shm.m_ptrCom->para.group[i].ycNum;
		num = m_shm.m_ptrYC->mapInfo[i].num;			//wfp changed at 20100629 以实际上次map的num为基准
		if ( ycPara(i) != NULL && num > 0)
		{
			//wfp changed at 20110826 for 记录上次的越限类型，防止被初始化
			for (hUInt32 j=0;j<num;j++)
			{
				limitType = (m_shm.ycPara(i)+j)->limitType;
				ACE_OS::memset( m_shm.ycPara(i)+j,0,sizeof(DAC_YC) );
				(m_shm.ycPara(i)+j)->limitType = limitType;
			}

			//ACE_OS::memset( m_shm.ycPara(i),0,sizeof(DAC_YC) * num );
		}

		//yx
		//num = m_shm.m_ptrCom->para.group[i].yxNum;
		num = m_shm.m_ptrYX->mapInfo[i].num;			//wfp changed at 20100629
		if ( yxPara(i) && num > 0 )
		{
			ACE_OS::memset( m_shm.yxPara(i),0,sizeof(DAC_YX) * num );
		}
		
		//kwh
		//num = m_shm.m_ptrCom->para.group[i].kwhNum;
		num = m_shm.m_ptrKWH->mapInfo[i].num;			//wfp changed at 20100629
		if ( kwhPara(i) && num > 0 )
		{
			ACE_OS::memset( m_shm.kwhPara(i),0,sizeof(DAC_KWH) * num );
		}

	}
}

void CDataInf::resetAllDataInfo()
{
	for (hUInt32 i=0;i<m_shm.m_ptrCom->info.system.groupNum;i++ )
	{
		resetDataInfo( i );
	}
}

void CDataInf::resetDataInfo(hInt32 group)
{
	if ( group < 0 || group > m_shm.m_ptrCom->info.system.groupNum )
		return;
	DAC_GROUP *pGroup = &m_shm.m_ptrCom->para.group[group];
	DAC_GROUP_INFO *pGroupInfo = &m_shm.m_ptrCom->info.group[group];
	hInt32 routeNo;
	hInt32 maxNo = pGroup->ycNum;
	maxNo = maxNo < pGroup->yxNum ? pGroup->yxNum : maxNo;
	maxNo = maxNo < pGroup->kwhNum ? pGroup->kwhNum : maxNo;
	hUInt32 quality;
	for ( int i=0;i<pGroupInfo->routeNum;i++)
	{
		routeNo = pGroupInfo->routes[i];
		if ( routeNo < 0 || routeNo >= m_shm.m_ptrCom->info.system.routeNum )
			continue;

		for ( int j=0;j<maxNo;j++ )
		{
			if ( j < pGroup->ycNum )	//清空遥测数据信息
			{
				DAC_YC_DATA* pData = ycData( routeNo,j );
				if ( pData )
				{
					quality = pData->quality;
					PDT_SET_BITS(quality,QUALITY_BIT_IV);
					PDT_SET_BITS(quality,QUALITY_BIT_INIT);	//置初始化标记
					pData->quality = quality;
				}
			}
			
			if ( j < pGroup->yxNum )	//清空遥信数据信息
			{
				DAC_YX_DATA* pData = yxData( routeNo,j );
				if ( pData )
				{
					quality = pData->quality;
					PDT_SET_BITS(quality,QUALITY_BIT_IV);
					PDT_SET_BITS(quality,QUALITY_BIT_INIT);	//置初始化标记
					pData->quality = quality;
				}
			}

			if ( j < pGroup->kwhNum )	//清空电度数据信息
			{
				DAC_KWH_DATA* pData = kwhData( routeNo,j );
				if ( pData )
				{
					quality = pData->quality;
					PDT_SET_BITS(quality,QUALITY_BIT_IV);
					PDT_SET_BITS(quality,QUALITY_BIT_INIT);	//置初始化标记
					pData->quality = quality;
				}
			}	
			
		}//end for	
	}//end for
}

bool	CDataInf::createYxEvent(hInt32 group,hInt32 yx,const DAC_YX_DATA &oldVal,const DAC_YX_DATA &newVal)
{
	return	false;
}

bool	CDataInf::createSoeEvent(hInt32 group,const DAC_SOE_DATA &val)
{
	DAC_YX* pPara = yxPara(group,val.no);
	if ( pPara == NULL || !pPara->valid ) return false;

	if ( group < 0 || group > m_shm.m_ptrCom->info.system.groupNum )
		return false;
	DAC_GROUP *pGroup = &m_shm.m_ptrCom->para.group[group];
	
	if ( pGroup->station < 0 || pGroup->station > m_shm.m_ptrCom->info.system.stationNum )
		return false;
	DAC_STATION* pStation = &m_shm.m_ptrCom->para.station[pGroup->station];

	nSysEvent	sysEvent;
	ACE_OS::memset(&sysEvent,0,sizeof(nSysEvent));
	sysEvent.m_eventSort = EVENT_SORT_SOE;
	sysEvent.m_eventType = pPara->eventType;
	if ( pStation != NULL)
	{
		ACE_OS::strsncpy(sysEvent.m_stationCode,pStation->code,SHORTNAME_LEN);
		ACE_OS::strsncpy(sysEvent.m_stationName,pStation->name,SHORTNAME_LEN);
	}
		
	ACE_OS::strsncpy(sysEvent.m_code,pPara->code,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_name,pPara->name,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_parentCode,pGroup->code,SHORTNAME_LEN);
	ACE_OS::strsncpy(sysEvent.m_parentName,pGroup->name,SHORTNAME_LEN);

	nDateTime dateTime = val.soeTime;
	sysEvent.m_ymd = dateTime.year*100*100 + dateTime.mon*100 + dateTime.day;
	sysEvent.m_hmsms= dateTime.hour*100*100*1000 + dateTime.min*100*1000 + dateTime.sec*1000 + dateTime.msec;

	sysEvent.m_security = pPara->eventLevel;
	
	//判断是否是恢复
	if ( val.val == pPara->normal )		//与正常状态相同
	{
		sysEvent.m_isRevert = true;
		sysEvent.SmsOut = 0;
		//sysEvent.m_stateValue1 = chgData.lastVal;
	}
	else 
	{
		sysEvent.m_isRevert	= false;
		sysEvent.SmsOut = pPara->isMsgAlarm;
		sysEvent.m_stateValue1 = pPara->normal;
	}

	sysEvent.PrintOut = 1;
	sysEvent.m_stateValue = val.val;
	//

	return (bool)m_event.insertEvent(sysEvent);
	//return	false;
}

/*
 * 函数功能：返回指定数据组的主路径
 * 函数返回：-1：失败
 * 函数说明：本系统保留one group -- multi routes 模型，但配置不支持，且暂不支持main route,目前用首路径替代
 */
hInt32 CDataInf::mainRoute(hInt32 group) const
{
	if ( !m_shm.m_ptrCom )
		return -1;

	if (group < 0 || group >= m_shm.m_ptrCom->info.system.groupNum )
		return -1;

	if ( !m_shm.m_ptrCom->para.group[group].valid )
		return -1;

	//wfp add at 20090715
	if ( m_shm.m_ptrCom->info.group[group].routeNum <= 0 )
		return -1;

	//return m_shm.m_ptrCom->info.group[group].routes[0];

	return m_shm.m_ptrCom->info.group[group].mainRoute;
}

/*************************************************************
函数功能：数据过滤
变量：	  无
处理：    进行对value进行非法数据过滤、突变处理、零漂处理，
判断用域：	零值范围ZeroVal；
			最大非法MaxInvld；
			最小非法MinInvld；
			最大突变MaxChange；
			突变计数overIncCnt；
返回：	  true:值被过滤；即该值无效，不存共享内存
**************************************************************/
bool CDataInf::filter(hInt32 group,hInt32 no,DAC_YC_DATA* pData,const DAC_YC_DATA& inputData)
{
	if ( pData == NULL ) return true;
	DAC_YC* pPara = ycPara(group,no);
	if ( pPara == NULL || !pPara->valid ) return true;
	nFloat inputVal = inputData.val;

	//零漂判定
	if( fabs(inputVal) <= pPara->zeroVal )		//零漂--剔除
	{
		logprint(LOG_DATAINF_BASE,"<datainf>group=%d,no=%d,val=%f 零漂",group,no,(float)inputVal);
		inputVal = 0.0;
		//return false;		//wfp masked at 20100406 for 应等待非法和突变判定才能入数据
	}

	//非法判定
	if( !PDT_ISZERO(pPara->maxIllegal-pPara->minIllegal) ) 
	{
		if( inputVal > pPara->maxIllegal || inputVal < pPara->minIllegal ) 
		{//非法
			logprint(LOG_DATAINF_BASE,"<datainf>group=%d,no=%d,val=%f 非法",group,no,(float)inputVal);
			return true;						//丢弃非法数据，因而置数据未变化标记（仍沿用上次数据），返回。
		}
	}

	//突变判定
	if( PDT_ISZERO(pPara->maxChange) || PDT_BIT_ENABLED((hUInt32)pData->quality,QUALITY_BIT_INIT)	)
	{
		pData->val = inputVal;	//无最大突变范围限制，不做突变检查。
	}
	else							//要做突变检查		
	{
		if( fabs(inputVal-pData->val) > fabs(pPara->maxChange) )//数据变化超出最大突变范围，
		{
			if( pPara->overIncCnt == 0 )
			{
				pPara->overIncCnt = 1;	//首次
			}
			else 
			{
				pPara->overIncCnt += 1;
				if( pPara->overIncCnt > DAC_OVER_INC_TIME )
				{
					pPara->overIncCnt = 0;
					pData->val = inputVal;
					return false;
				}
			}
			logprint(LOG_DATAINF_BASE,"<datainf>group=%d,no=%d,val=%f 突变",group,no,(float)inputVal);
			return true;
		}
		else	//变化未超出范围，认可该数据，重置突变计数为0。
		{
			pData->val = inputVal;
			pPara->overIncCnt = 0;
		}
	}//end if 突变检查

	return false;
}

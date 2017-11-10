#include "utl/directory.h"
#include "dac/dacshm.h"
#include "dac/dacdef.h"
#include "sys/plat_api.h"
#include "sys/sysmacro.h"
#include "sys/blog.h"

using	namespace	PDT;

#define	DAC_SHM_DIR				"dat/map/dac"
#define	DAC_SHM_FILE_COM		"com.dat"
#define	DAC_SHM_FILE_YC			"yc.dat"
#define	DAC_SHM_FILE_YX			"yx.dat"
#define	DAC_SHM_FILE_KWH		"kwh.dat"
#define	DAC_SHM_FILE_SOE		"soe.dat"


//notify by wlq 20170307 支持1024通道
#define	DAC_YC_NUM				256			//每个路径数据区预计遥测个数
#define DAC_YX_NUM				512			//每个路径数据区预计遥信个数
#define DAC_KWH_NUM				10			//每个路径数据区预计电度个数

//以下支持多测点
//#define	DAC_YC_NUM				10240			//每个路径数据区预计遥测个数
//#define DAC_YX_NUM				10240			//每个路径数据区预计遥信个数
//#define DAC_KWH_NUM				128			//每个路径数据区预计电度个数

//mondify end by wlq 20170307

#define	DAC_SHM_HEAD_SIZE		256			//内存头大小

DAC_SHM_COM		*CShareMemory::m_ptrCom = NULL;
DAC_SHM_YC		*CShareMemory::m_ptrYC = NULL;
DAC_SHM_YX		*CShareMemory::m_ptrYX = NULL;
DAC_SHM_KWH		*CShareMemory::m_ptrKWH = NULL;
DAC_SHM_SOE		*CShareMemory::m_ptrSOE = NULL;

CShareMemory::Malloc	*CShareMemory::m_shmCom = NULL;
CShareMemory::Malloc	*CShareMemory::m_shmYC = NULL;
CShareMemory::Malloc	*CShareMemory::m_shmYX = NULL;
CShareMemory::Malloc	*CShareMemory::m_shmKWH = NULL;
CShareMemory::Malloc	*CShareMemory::m_shmSOE = NULL;
//=============================================================================
CShareMemory::CShareMemory()
{
	map();
}

CShareMemory::~CShareMemory()
{
}
//-----------------------------------------------------------------------------
void	CShareMemory::map()
{
	//创建目录
	char	path[MAXPATHLEN];
	ACE_OS::sprintf(path,"%s/%s",ACE_OS::getenv(SYS_ENV_VAR),DAC_SHM_DIR);
	CDir::makeDir(path);
	//计算各个内存池的总大小
	size_t	comSize = sizeof(DAC_SHM_COM) + DAC_SHM_HEAD_SIZE;
	size_t	ycSize = DAC_SHM_HEAD_SIZE + sizeof(DAC_SHM_YC) + DAC_ROUTE_NUM * (DAC_SHM_HEAD_SIZE + 2 * DAC_YC_NUM * (sizeof(DAC_YC_DATA)+sizeof(DAC_YC_CHG_DATA)));
	size_t	yxSize = DAC_SHM_HEAD_SIZE + sizeof(DAC_SHM_YX) + DAC_ROUTE_NUM * (DAC_SHM_HEAD_SIZE + 2 * DAC_YX_NUM * (sizeof(DAC_YX_DATA)+sizeof(DAC_YX_CHG_DATA)));
	size_t	kwhSize = DAC_SHM_HEAD_SIZE + sizeof(DAC_SHM_KWH) + DAC_ROUTE_NUM * (DAC_SHM_HEAD_SIZE + 2 * DAC_KWH_NUM * (sizeof(DAC_KWH_DATA)+sizeof(DAC_KWH_CHG_DATA)));
	size_t	soeSize = DAC_SHM_HEAD_SIZE + sizeof(DAC_SHM_SOE) + DAC_ROUTE_NUM * (DAC_SHM_HEAD_SIZE + 3 * DAC_YX_NUM * sizeof(DAC_SOE_DATA));
	//目前遥测的参数和数据都放在共享内存中，若发现大小达到扩展瓶颈出现的问题，可使用以下方式
	//size_t	ycSize = DAC_SHM_HEAD_SIZE + sizeof(DAC_SHM_YC) + DAC_GROUP_NUM * (DAC_SHM_HEAD_SIZE + DAC_YC_NUM*sizeof(DAC_YC)) + DAC_ROUTE_NUM * (DAC_SHM_HEAD_SIZE + DAC_YC_NUM * (sizeof(DAC_YC_DATA)+2*sizeof(DAC_YC_CHG_DATA)));
	//size_t	yxSize = DAC_SHM_HEAD_SIZE + sizeof(DAC_SHM_YX) + DAC_GROUP_NUM * (DAC_SHM_HEAD_SIZE + DAC_YX_NUM*sizeof(DAC_YX)) + DAC_ROUTE_NUM * (DAC_SHM_HEAD_SIZE + DAC_YX_NUM * (sizeof(DAC_YX_DATA)+2*sizeof(DAC_YX_CHG_DATA)));
	//size_t	kwhSize = DAC_SHM_HEAD_SIZE + sizeof(DAC_SHM_KWH) + DAC_GROUP_NUM * (DAC_SHM_HEAD_SIZE + DAC_KWH_NUM*sizeof(DAC_KWH)) + DAC_ROUTE_NUM * (DAC_SHM_HEAD_SIZE + DAC_KWH_NUM * (sizeof(DAC_KWH_DATA)+2*sizeof(DAC_KWH_CHG_DATA)));

	//创建共享内存池
	ACE_MMAP_Memory_Pool_Options	options((void *)0,ACE_MMAP_Memory_Pool_Options::NEVER_FIXED,1,comSize);
	ACE_MMAP_Memory_Pool_Options	optionsYC((void *)0,ACE_MMAP_Memory_Pool_Options::NEVER_FIXED,1,ycSize);
	ACE_MMAP_Memory_Pool_Options	optionsYX((void *)0,ACE_MMAP_Memory_Pool_Options::NEVER_FIXED,1,yxSize);
	ACE_MMAP_Memory_Pool_Options	optionsKWH((void *)0,ACE_MMAP_Memory_Pool_Options::NEVER_FIXED,1,kwhSize);
	ACE_MMAP_Memory_Pool_Options	optionsSOE((void *)0,ACE_MMAP_Memory_Pool_Options::NEVER_FIXED,1,soeSize);
	//分配内存
	char name[MAXPATHLEN+MAXNAMELEN];
	if(!m_shmCom)
	{
		ACE_OS::sprintf( name,"%s/%s",path,DAC_SHM_FILE_COM);
		m_shmCom = new	Malloc(name,name,&options);
	}
	if(!m_shmYC)
	{
		ACE_OS::sprintf( name,"%s/%s",path,DAC_SHM_FILE_YC);
		m_shmYC = new	Malloc(name,name,&optionsYC);
	}
	if(!m_shmYX)
	{
		ACE_OS::sprintf( name,"%s/%s",path,DAC_SHM_FILE_YX);
		m_shmYX = new	Malloc(name,name,&optionsYX);
	}
	if(!m_shmKWH)	
	{
		ACE_OS::sprintf( name,"%s/%s",path,DAC_SHM_FILE_KWH);
		m_shmKWH = new	Malloc(name,name,&optionsKWH);
	}
	if(!m_shmSOE)	
	{
		ACE_OS::sprintf( name,"%s/%s",path,DAC_SHM_FILE_SOE);
		m_shmSOE = new	Malloc(name,name,&optionsSOE);
	}
	//在各个内存池中创建，创建一个索引块
	if(!m_ptrCom && m_shmCom->find("shm",(void *&)m_ptrCom)!=0)
	{
		m_ptrCom = (DAC_SHM_COM *)m_shmCom->calloc(  sizeof(DAC_SHM_COM) );
		m_shmCom->bind("shm",m_ptrCom);
	}
	if(!m_ptrYC && m_shmYC->find("shm",(void *&)m_ptrYC)!=0)
	{
		m_ptrYC = (DAC_SHM_YC *)m_shmYC->calloc( sizeof(DAC_SHM_YC) );
		m_shmYC->bind("shm",(void *&)m_ptrYC);
	}
	if(!m_ptrYX && m_shmYX->find("shm",(void *&)m_ptrYX)!=0)
	{
		m_ptrYX = (DAC_SHM_YX *)m_shmYX->calloc(sizeof(DAC_SHM_YX));
		m_shmYX->bind("shm",m_ptrYX);
	}
	if(!m_ptrKWH && m_shmKWH->find("shm",(void *&)m_ptrKWH)!=0)
	{
		m_ptrKWH = (DAC_SHM_KWH *)m_shmKWH->calloc(sizeof(DAC_SHM_KWH));
		m_shmKWH->bind("shm",m_ptrKWH);
	}
	if(!m_ptrSOE && m_shmSOE->find("shm",(void *&)m_ptrSOE)!=0)
	{
		m_ptrSOE = (DAC_SHM_SOE *)m_shmSOE->calloc(sizeof(DAC_SHM_SOE));
		m_shmSOE->bind("shm",m_ptrSOE);
	}
}

void	CShareMemory::unMap()
{
}
//-----------------------------------------------------------------------------
void	CShareMemory::mapYc(int groupNo)
{
	if(groupNo<0 || groupNo>=DAC_GROUP_NUM)		return;
	DAC_GROUP_INFO	*pGroupInfo = &m_ptrCom->info.group[groupNo];
	DAC_MAP_INFO	*pMapInfo = &m_ptrYC->mapInfo[groupNo];
	hUInt32			newNum = m_ptrCom->para.group[groupNo].ycNum;
	hUInt32			oldNum = pMapInfo->num;
	hUInt32			i,j;
	hInt32			routeNo;
	char			groupName[16] = {0};
	char			routeName[16] = {0};
	DAC_YC			*pPara = ycPara(groupNo);
	DAC_YC_DATA		*pData;
	ACE_OS::sprintf(groupName,"g%d",groupNo);
	//释放
	if( pPara )
	{
		//参数
		if(oldNum != newNum)
		{
			m_shmYC->unbind(groupName);
			m_shmYC->free( pPara );
			m_ptrYC->para[groupNo] = 0;
			pPara = NULL;
		}
		//数据
		for(i=0;i<pMapInfo->routeNum;i++)
		{
			routeNo = pMapInfo->routes[i];
			if(routeNo<0 || routeNo>=DAC_ROUTE_NUM)	continue;
			//释放条件 ： 数据个数不等、不属于当前数据组、
			if(oldNum == newNum)
			{
				for(j=0;j<pGroupInfo->routeNum;j++)
				{
					if(routeNo == pGroupInfo->routes[j])	break;
				}
				if( j<pGroupInfo->routeNum)	continue;
			}
			//释放
			pData = ycData(routeNo);
			if(!pData)	continue;
			ACE_OS::sprintf(routeName,"r%d",routeNo);
			m_shmYC->unbind(routeName);
			m_shmYC->free(pData);
			m_ptrYC->data[routeNo] = 0;
			m_ptrYC->chgData[routeNo] = 0;
		}
	}
	//分配参数
	if(!pPara && newNum>0) 
	{
		pPara = (DAC_YC *)m_shmYC->calloc( newNum * sizeof(DAC_YC) );
		m_shmYC->bind(groupName,pPara);
		m_ptrYC->para[groupNo] = (size_t)pPara - (size_t)m_shmYC->base_addr();
		pMapInfo->num = newNum;
		logprint(LOG_DACSHM_BASE,"mapYcPara : Name = %s , Size = %d",groupName,newNum *sizeof(DAC_YC));
	}
	pMapInfo->routeNum = pGroupInfo->routeNum;
	for(i=0;i<DAC_ROUTE_IN_GROUP;i++)	pMapInfo->routes[i] = pGroupInfo->routes[i];
	//数据
	for(i=0;i<pMapInfo->routeNum;i++)
	{
		routeNo = pMapInfo->routes[i];
		if(routeNo<0 || routeNo>=DAC_ROUTE_NUM)	continue;
		pData = ycData(routeNo);
		if(!pData && newNum>0)		//wfp changed at 20100315
		{
			pData = (DAC_YC_DATA *)m_shmYC->calloc( newNum * sizeof(DAC_YC_DATA) + 2 * newNum * sizeof( DAC_YC_CHG_DATA) );
			ACE_OS::sprintf(routeName,"r%d",routeNo);
			m_shmYC->bind( routeName , pData );
			m_ptrYC->data[routeNo] = (size_t)pData - (size_t)m_shmYC->base_addr();
			m_ptrYC->chgData[routeNo] = (size_t)(pData+newNum) - (size_t)m_shmYC->base_addr();
			logprint(LOG_DACSHM_BASE,"mapYcData : Name = %s , Size = %d",routeName,newNum*sizeof(DAC_YC_DATA)+newNum*sizeof(DAC_YC_CHG_DATA));
		}
	}//end for
}

void	CShareMemory::mapYx(int groupNo)
{
	if(groupNo<0 || groupNo>=DAC_GROUP_NUM)		return;
	DAC_GROUP_INFO	*pGroupInfo = &m_ptrCom->info.group[groupNo];
	DAC_MAP_INFO	*pMapInfo = &m_ptrYX->mapInfo[groupNo];
	hUInt32			newNum = m_ptrCom->para.group[groupNo].yxNum;
	hUInt32			oldNum = pMapInfo->num;
	hUInt32			i,j;
	hInt32			routeNo;
	char			groupName[16] = {0};
	char			routeName[16] = {0};
	DAC_YX			*pPara = yxPara(groupNo); 
	DAC_YX_DATA		*pData;
	DAC_SOE_DATA	*pSoeData;
	ACE_OS::sprintf(groupName,"g%d",groupNo);
	//释放
	if( pPara )
	{
		//参数
		if(oldNum != newNum)
		{
			m_shmYX->unbind(groupName);
			m_shmYX->free( pPara );
			m_ptrYX->para[groupNo] = 0;
			pPara = NULL;
		}
		//数据
		for(i=0;i<pMapInfo->routeNum;i++)
		{
			routeNo = pMapInfo->routes[i];
			if(routeNo<0 || routeNo>=DAC_ROUTE_NUM)	continue;
			//释放条件 ： 数据个数不等、不属于当前数据组
			if(oldNum == newNum)
			{
				for(j=0;j<pGroupInfo->routeNum;j++)
				{
					if(routeNo == pGroupInfo->routes[j])	break;
				}
				if( j<pGroupInfo->routeNum )	continue;
			}
			//释放YX
			pData = yxData(routeNo);
			if(!pData)	continue;
			ACE_OS::sprintf(routeName,"r%d",routeNo);
			m_shmYX->unbind(routeName);
			m_shmYX->free( pData );
			m_ptrYX->data[routeNo] = 0;
			m_ptrYX->chgData[routeNo] = 0;
			//释放SOE
			pSoeData = soeData(routeNo);	
			if(!pSoeData)	continue;
			m_shmSOE->unbind(routeName);
			m_shmSOE->free( pSoeData );
			m_ptrSOE->data[routeNo] = 0;
		}
	}
	//分配参数
	if(!pPara && newNum>0)
	{
		pPara = (DAC_YX *)m_shmYX->calloc( newNum * sizeof(DAC_YX) );
		m_shmYX->bind(groupName,pPara);
		m_ptrYX->para[groupNo] = (size_t)pPara - (size_t)m_shmYX->base_addr();
		pMapInfo->num = newNum;
		m_ptrSOE->mapInfo[groupNo].num = newNum;  //wfp add for setSoe
		logprint(LOG_DACSHM_BASE,"mapYxPara : Name = %s , Size = %d",groupName,newNum *sizeof(DAC_YX));
	}
	pMapInfo->routeNum = pGroupInfo->routeNum;
	m_ptrSOE->mapInfo[groupNo].routeNum = pGroupInfo->routeNum;			//wfp add for setSoe
	for(i=0;i<DAC_ROUTE_IN_GROUP;i++)	
	{
		pMapInfo->routes[i] = pGroupInfo->routes[i];
		m_ptrSOE->mapInfo[groupNo].routes[i] = pGroupInfo->routes[i];	 //wfp add for setSoe
	}
	//数据
	for(i=0;i<pMapInfo->routeNum;i++)
	{
		routeNo = pMapInfo->routes[i];
		if(routeNo<0 || routeNo>=DAC_ROUTE_NUM)	continue;
		pData = yxData(routeNo);
		if(!pData && newNum>0)
		{
			//YX
			pData = (DAC_YX_DATA *)m_shmYX->calloc( newNum * sizeof(DAC_YX_DATA) + 2 * newNum * sizeof( DAC_YX_CHG_DATA) );
			ACE_OS::sprintf(routeName,"r%d",routeNo);
			m_shmYX->bind( routeName , pData );
			m_ptrYX->data[routeNo] = (size_t)pData - (size_t)m_shmYX->base_addr();
			m_ptrYX->chgData[routeNo] = (size_t)(pData+newNum) - (size_t)m_shmYX->base_addr();
			logprint(LOG_DACSHM_BASE,"mapYxData : Name = %s , Size = %d",routeName,newNum*sizeof(DAC_YX_DATA)+newNum*sizeof(DAC_YX_CHG_DATA));
			//SOE
			pSoeData = (DAC_SOE_DATA *)m_shmSOE->calloc( newNum * sizeof(DAC_SOE_DATA) );	
			m_shmSOE->bind( routeName , pSoeData );
			m_ptrSOE->data[routeNo] = (size_t)pSoeData - (size_t)m_shmSOE->base_addr();
			logprint(LOG_DACSHM_BASE,"mapSoeData : Name = %s , Size = %d",routeName,newNum * sizeof(DAC_SOE_DATA) );
		}
	}//end for
}

void	CShareMemory::mapKwh(int groupNo)
{
	if(groupNo<0 || groupNo>=DAC_GROUP_NUM)		return;
	DAC_GROUP_INFO	*pGroupInfo = &m_ptrCom->info.group[groupNo];
	DAC_MAP_INFO	*pMapInfo = &m_ptrKWH->mapInfo[groupNo];
	hUInt32			newNum = m_ptrCom->para.group[groupNo].kwhNum;
	hUInt32			oldNum = pMapInfo->num;
	hUInt32			i,j;
	hInt32			routeNo;
	char			groupName[16] = {0};
	char			routeName[16] = {0};
	DAC_KWH			*pPara = kwhPara(groupNo); 
	DAC_KWH_DATA	*pData;
	ACE_OS::sprintf(groupName,"g%d",groupNo);
	//释放
	if( pPara )
	{
		//参数
		if(oldNum != newNum)
		{
			m_shmKWH->unbind(groupName);
			m_shmKWH->free(pPara);
			m_ptrKWH->para[groupNo] = 0;
			pPara = NULL;
		}
		//数据
		for(i=0;i<pMapInfo->routeNum;i++)
		{
			routeNo = pMapInfo->routes[i];
			if(routeNo<0 || routeNo>=DAC_ROUTE_NUM)	continue;
			//释放条件 ： 数据个数不等、不属于当前数据组、属于当前数据组但不是本地路径
			if(oldNum == newNum)
			{
				for(j=0;j<pGroupInfo->routeNum;j++)
				{
					if(routeNo == pGroupInfo->routes[j])	break;
				}
				if( j<pGroupInfo->routeNum )	continue;
			}
			//释放
			pData = kwhData(routeNo);
			if(!pData)	continue;
			ACE_OS::sprintf(routeName,"r%d",routeNo);
			m_shmKWH->unbind(routeName);
			m_shmKWH->free( pData );
			m_ptrKWH->data[routeNo] = 0;
			m_ptrKWH->chgData[routeNo] = 0;
		}
	}
	//分配参数
	if(!pPara && newNum>0)
	{
		pPara = (DAC_KWH *)m_shmKWH->calloc( newNum * sizeof(DAC_KWH) );
		m_shmKWH->bind( groupName , pPara );
		m_ptrKWH->para[groupNo] = (size_t)pPara - (size_t)m_shmKWH->base_addr();
		pMapInfo->num = newNum;
		logprint(LOG_DACSHM_BASE,"mapKwhPara : Name = %s , Size = %d",groupName,newNum *sizeof(DAC_KWH));
	}
	pMapInfo->routeNum = pGroupInfo->routeNum;
	for(i=0;i<DAC_ROUTE_IN_GROUP;i++)	pMapInfo->routes[i] = pGroupInfo->routes[i];
	//数据
	for(i=0;i<pMapInfo->routeNum;i++)
	{
		routeNo = pMapInfo->routes[i];
		if(routeNo<0 || routeNo>=DAC_ROUTE_NUM)	continue;
		pData = kwhData(routeNo);
		if(!pData && newNum>0)
		{
			pData = (DAC_KWH_DATA *)m_shmKWH->calloc( newNum * sizeof(DAC_KWH_DATA) + 2 * newNum * sizeof( DAC_KWH_CHG_DATA) );
			ACE_OS::sprintf(routeName,"r%d",routeNo);
			m_shmKWH->bind( routeName , pData );
			m_ptrKWH->data[routeNo] = (size_t)pData - (size_t)m_shmKWH->base_addr();
			m_ptrKWH->chgData[routeNo] = (size_t)(pData+newNum) - (size_t)m_shmKWH->base_addr();
			logprint(LOG_DACSHM_BASE,"mapKwhData : Name = %s , Size = %d",routeName,newNum*sizeof(DAC_KWH_DATA)+newNum*sizeof(DAC_KWH_CHG_DATA));
		}
	}//end for
}
//-----------------------------------------------------------------------------
DAC_YC	*CShareMemory::ycPara(hInt32 groupNo)
{
	if(groupNo<0 || groupNo>=m_ptrCom->info.system.groupNum)	return	NULL;
	size_t	offset = m_ptrYC->para[groupNo];
	if(offset == 0)	return	NULL;
	return	(DAC_YC *)((char *)m_shmYC->base_addr()+offset);
}

DAC_YX	*CShareMemory::yxPara(hInt32 groupNo)
{
	if(groupNo<0 || groupNo>=m_ptrCom->info.system.groupNum)	return	NULL;
	size_t	offset = m_ptrYX->para[groupNo];
	if(offset == 0)	return	NULL;
	return	(DAC_YX *)((char *)m_shmYX->base_addr()+offset);
}

DAC_KWH	*CShareMemory::kwhPara(hInt32 groupNo)
{
	if(groupNo<0 || groupNo>=m_ptrCom->info.system.groupNum)	return	NULL;
	size_t	offset = m_ptrKWH->para[groupNo];
	if(offset == 0)	return	NULL;
	return	(DAC_KWH *)((char *)m_shmKWH->base_addr()+offset);
}

//add by wlq 20150811
DAC_SOE	*CShareMemory::soePara(hInt32 groupNo)
{
	if(groupNo<0 || groupNo>=m_ptrCom->info.system.groupNum)	return	NULL;
	size_t	offset = m_ptrSOE->para[groupNo];
	if(offset == 0)	return	NULL;
	return	(DAC_SOE *)((char *)m_shmSOE->base_addr()+offset);
}
//add end 20150811

const	DAC_YC	*CShareMemory::ycPara(hInt32 groupNo)	const
{
	if(groupNo<0 || groupNo>=m_ptrCom->info.system.groupNum)	return	NULL;
	size_t	offset = m_ptrYC->para[groupNo];
	if(offset == 0)	return	NULL;
	return	(DAC_YC *)((char *)m_shmYC->base_addr()+offset);
}

const	DAC_YX	*CShareMemory::yxPara(hInt32 groupNo)	const
{
	if(groupNo<0 || groupNo>=m_ptrCom->info.system.groupNum)	return	NULL;
	size_t	offset = m_ptrYX->para[groupNo];
	if(offset == 0)	return	NULL;
	return	(DAC_YX *)((char *)m_shmYX->base_addr()+offset);
}

const	DAC_KWH	*CShareMemory::kwhPara(hInt32 groupNo)	const
{
	if(groupNo<0 || groupNo>=m_ptrCom->info.system.groupNum)	return	NULL;
	size_t	offset = m_ptrKWH->para[groupNo];
	if(offset == 0)	return	NULL;
	return	(DAC_KWH *)((char *)m_shmKWH->base_addr()+offset);
}
//add by wlq 20150811
const	DAC_SOE	*CShareMemory::soePara(hInt32 groupNo)	const
{
	if(groupNo<0 || groupNo>=m_ptrCom->info.system.groupNum)	return	NULL;
	size_t	offset = m_ptrSOE->para[groupNo];
	if(offset == 0)	return	NULL;
	return	(DAC_SOE *)((char *)m_shmSOE->base_addr()+offset);
}
//add end 20150811
//-----------------------------------------------------------------------------
DAC_YC_DATA	*CShareMemory::ycData(hInt32 routeNo)
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrYC->data[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_YC_DATA *)((char *)m_shmYC->base_addr()+offset);
}

DAC_YX_DATA	*CShareMemory::yxData(hInt32 routeNo)
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrYX->data[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_YX_DATA *)((char *)m_shmYX->base_addr()+offset);
}

DAC_KWH_DATA	*CShareMemory::kwhData(hInt32 routeNo)
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrKWH->data[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_KWH_DATA *)((char *)m_shmKWH->base_addr()+offset);
}

DAC_SOE_DATA	*CShareMemory::soeData(hInt32 routeNo)
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrSOE->data[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_SOE_DATA *)((char *)m_shmSOE->base_addr()+offset);
}

const	DAC_YC_DATA	*CShareMemory::ycData(hInt32 routeNo)	const
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrYC->data[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_YC_DATA *)((char *)m_shmYC->base_addr()+offset);
}

const	DAC_YX_DATA	*CShareMemory::yxData(hInt32 routeNo)	const
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrYX->data[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_YX_DATA *)((char *)m_shmYX->base_addr()+offset);
}

const	DAC_KWH_DATA	*CShareMemory::kwhData(hInt32 routeNo)	const
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrKWH->data[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_KWH_DATA *)((char *)m_shmKWH->base_addr()+offset);
}

const	DAC_SOE_DATA	*CShareMemory::soeData(hInt32 routeNo)	const
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrSOE->data[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_SOE_DATA *)((char *)m_shmSOE->base_addr()+offset);
}
//-----------------------------------------------------------------------------
DAC_YC_CHG_DATA	*CShareMemory::ycChgData(hInt32 routeNo)
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrYC->chgData[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_YC_CHG_DATA *)((char *)m_shmYC->base_addr()+offset);
}

DAC_YX_CHG_DATA	*CShareMemory::yxChgData(hInt32 routeNo)
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrYX->chgData[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_YX_CHG_DATA *)((char *)m_shmYX->base_addr()+offset);
}

DAC_KWH_CHG_DATA	*CShareMemory::kwhChgData(hInt32 routeNo)
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrKWH->chgData[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_KWH_CHG_DATA *)((char *)m_shmKWH->base_addr()+offset);
}

const	DAC_YC_CHG_DATA	*CShareMemory::ycChgData(hInt32 routeNo)	const
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrYC->chgData[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_YC_CHG_DATA *)((char *)m_shmYC->base_addr()+offset);
}

const	DAC_YX_CHG_DATA	*CShareMemory::yxChgData(hInt32 routeNo)	const
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrYX->chgData[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_YX_CHG_DATA *)((char *)m_shmYX->base_addr()+offset);
}

const	DAC_KWH_CHG_DATA	*CShareMemory::kwhChgData(hInt32 routeNo)	const
{
	if(routeNo<0 || routeNo>=m_ptrCom->info.system.routeNum)	return	NULL;
	size_t	offset = m_ptrKWH->chgData[routeNo];
	if(offset == 0)	return	NULL;
	return	(DAC_KWH_CHG_DATA *)((char *)m_shmKWH->base_addr()+offset);
}



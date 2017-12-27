/*==============================================================================
* 文件名称 : loadpara.cpp
* 模块 : 前置参数加载
* 创建时间 : 2008-06-28 15:55:23
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#include "dac/dacdef.h"
#include "dac/loadpara.h"
#include "ace/OS.h"
#include "sys/blog.h"

using namespace PDT;

CLoadPara::CLoadPara()
{
	m_pRdbOp = new CRdbOp(CRdbOp::PingDirect,false,1000);
	if ( !m_pRdbOp )
		logprint(LOG_LOADPARA_BASE,"CLoadPara::CLoadPara() : can't new CRdbOp object,load parameter failed");
}

CLoadPara::~CLoadPara()
{
	delete m_pRdbOp;
	m_pRdbOp = 0;
}

bool	CLoadPara::loadAll()
{
	loadProtocol();
	loadStation();
	loadChannel();
	loadGroup(false);
	loadRoute();
	
	//loadYc();
	//loadYx();	
	DAC_SYSTEM_INFO* pSystemInfo = m_commInf.systemInfo();
	DAC_GROUP* pGroup = NULL;
	if ( pSystemInfo )
	{
		for (hUInt32 i=0;i<pSystemInfo->groupNum;i++)
		{
			pGroup = m_commInf.group(i);
			if ( pGroup == NULL ) continue;

			loadYc(pGroup->code);
			loadYx(pGroup->code);
		}
	}


	


	loadKwh();
	loadYk();
	loadYt();
	loadYccon();
	loadRecvDev();
	loadSendDev();

	return true;
}

bool	CLoadPara::loadStation()
{
	//CSql* pSql = m_pRdbOp->createSQL();
	//if ( !pSql )
	//{
	//	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadStation() : m_pRdbOp->createSQL() failed");
	//	return false;
	//}
	//pSql->setOperate( CSql::OP_Select );
	//pSql->setTableName("tdac_station");
	//pSql->selectAllField();
	//pSql->whereField("f_stano",0,CSql::CP_GreaterEqual);
	//pSql->orderField( "f_stano");

	int ret;
	CDataset ds;
	//ret = m_pRdbOp->exec( *pSql,ds);

	//if ( ret < 0 )
	//{
	//	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadStation() : m_pRdbOp->exec( *pSql,ds) failed");
	//	pSql->destroy();
	//	return false;
	//}

	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "select * from tdac_station,tsys_userinfo where tdac_station.f_stano =  tsys_userinfo.f_orgid");

	ret = m_pRdbOp->exec(sqlStr, ds);

	if ( ret < 0 )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadStation() : m_pRdbOp->exec( *pSql,ds) failed");
		return false;
	}


	int i,j,retno,okRows=0;
	int maxno = -1;

	for ( i=0,j=0;i<DAC_STATION_NUM;i++ )
	{
		retno = assignStation(i,ds,j);
		if ( retno >= 0 )
		{
			j++;
			okRows++;
			if ( retno > maxno )
				maxno = retno;
		}
	}

	m_dacShm.m_ptrCom->info.system.stationNum = maxno + 1;
	//pSql->destroy();

	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadStation() : success, okRows = %d",okRows);
	return true;
}

bool	CLoadPara::loadChannel()
{
	CSql* pSql = m_pRdbOp->createSQL();
	if ( !pSql )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadChannel() : m_pRdbOp->createSQL() failed");
		return false;
	}
	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("tdac_channel");
	pSql->selectAllField();
	pSql->whereField("f_channo",0,CSql::CP_GreaterEqual);
	pSql->orderField( "f_channo");

	int ret;
	CDataset ds;
	ret = m_pRdbOp->exec( *pSql,ds);

	if ( ret < 0 )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadChannel() : m_pRdbOp->exec( *pSql,ds) failed");
		pSql->destroy();
		return false;
	}

	int i,j,retno,okRows=0;
	int maxno = -1;

	for ( i=0,j=0;i<DAC_CHANNEL_NUM;i++ )
	{
		retno = assignChannel(i,ds,j);
		if ( retno >= 0 )
		{
			j++;
			okRows++;
			if ( retno > maxno )
				maxno = retno;
		}
	}

	m_dacShm.m_ptrCom->info.system.channelNum = maxno + 1;
	pSql->destroy();

	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadChannel() : success, okRows = %d",okRows);
	return true;
}

bool	CLoadPara::loadGroup(bool withData /*=true*/)
{
	CSql* pSql = m_pRdbOp->createSQL();
	if ( !pSql )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadGroup() : m_pRdbOp->createSQL() failed");
		return false;
	}
	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("tdac_group");
	pSql->selectAllField();
	pSql->whereField("f_grpno",0,CSql::CP_GreaterEqual);
	pSql->orderField( "f_grpno");

	int ret;
	CDataset ds;
	ret = m_pRdbOp->exec( *pSql,ds);

	if ( ret < 0 )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadGroup() : m_pRdbOp->exec( *pSql,ds) failed");
		pSql->destroy();
		return false;
	}

	int i,j,retno,okRows=0;
	int maxno = -1;
	size_t rows = ds.rowSize();

	for ( i=0,j=0;i<DAC_GROUP_NUM;i++ )
	{
		retno = assignGroup(i,ds,j,withData);
		if ( retno >= 0 )
		{
			j++;
			okRows++;
			if ( retno > maxno )
				maxno = retno;
		}
	}

	m_dacShm.m_ptrCom->info.system.groupNum = maxno + 1;
	pSql->destroy();

	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadGroup() : success, okRows = %d",okRows);
	return true;
}

bool	CLoadPara::loadRoute()
{
	CSql* pSql = m_pRdbOp->createSQL();
	if ( !pSql )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadRoute() : m_pRdbOp->createSQL() failed");
		return false;
	}
	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("tdac_route");
	pSql->selectAllField();
	pSql->whereField("f_routeno",0,CSql::CP_GreaterEqual);
	pSql->orderField("f_routeno");

	int ret;
	CDataset ds;
	ret = m_pRdbOp->exec( *pSql,ds);

	if ( ret < 0 )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadRoute() : m_pRdbOp->exec( *pSql,ds) failed");
		pSql->destroy();
		return false;
	}

	int retno,okRows=0;
	hUInt32 i,j,k=0;
	int maxno = -1;

	DAC_GROUP_INFO *pGroupInfo;
	for ( i=0;i<DAC_GROUP_NUM;i++ )
	{
		pGroupInfo = &m_dacShm.m_ptrCom->info.group[i];
		pGroupInfo->routeNum = 0;
		for ( j=0;j<DAC_ROUTE_IN_GROUP;j++ )
			pGroupInfo->routes[j] = -1;
	}

	DAC_CHANNEL_INFO *pChannelInfo = NULL;
	for ( i=0;i<DAC_CHANNEL_NUM;i++ )
	{
		pChannelInfo = &m_dacShm.m_ptrCom->info.channel[i];
		pChannelInfo->routeNum = 0;
		for ( j=0;j<DAC_ROUTE_IN_CHANNEL;j++ )
			pChannelInfo->routes[j] = -1;
	}

	for ( i=0,j=0;i<DAC_ROUTE_NUM;i++ )
	{
		retno = assignRoute(i,ds,j);
		if ( retno >= 0 )
		{
			j++;
			okRows++;
			if ( retno > maxno )
				maxno = retno;
		}
	}

	m_dacShm.m_ptrCom->info.system.routeNum = maxno + 1;
	pSql->destroy();

	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadRoute() : success, okRows = %d",okRows);
	return true;
}

bool	CLoadPara::loadProtocol()
{
	CSql* pSql = m_pRdbOp->createSQL();
	if ( !pSql )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadProtocol() : m_pRdbOp->createSQL() failed");
		return false;
	}
	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("tdac_protocol");
	pSql->selectAllField();
	pSql->whereField("f_protono",0,CSql::CP_GreaterEqual);
	pSql->orderField( "f_protono");

	int ret;
	CDataset ds;
	ret = m_pRdbOp->exec( *pSql,ds);

	if ( ret < 0 )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadProtocol() : m_pRdbOp->exec( *pSql,ds) failed");
		pSql->destroy();
		return false;
	}

	int i,j,retno,okRows=0;

	for ( i=0,j=0;i<DAC_PROTOCOL_NUM;i++ )
	{
		retno = assignProtocol(i,ds,j);
		if ( retno >= 0 )
		{
			j++;
			okRows++;
		}
	}

	pSql->destroy();

	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadProtocol() : success, okRows = %d",okRows );
	return true;
}

bool	CLoadPara::loadRecvDev()
{

	int ret;
	CDataset ds;

	char sqlStr[512] = "";
	ACE_OS::sprintf(sqlStr, "select * from tdac_recvdev,tdac_channel where tdac_recvdev.f_rdevaddr = tdac_channel.f_remoteaddr1");

	ret = m_pRdbOp->exec(sqlStr, ds);

	if ( ret < 0 )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadRecvDev() : m_pRdbOp->exec( *pSql,ds) failed");
		return false;
	}

	int i,j,retno,okRows=0;
	int maxno = -1;

	for ( i=0,j=0;i<DAC_RECVDEV_NUM;i++ )
	{
		retno = assignRecvDev(i,ds,j);
		if ( retno >= 0 )
		{
			j++;
			okRows++;
			if ( retno > maxno )
				maxno = retno;
		}
	}

	m_dacShm.m_ptrCom->info.system.recvdevNum = maxno + 1;

	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadStation() : success, okRows = %d",okRows);
	return true;
}


bool	CLoadPara::loadSendDev()
{
	CSql* pSql = m_pRdbOp->createSQL();
	if ( !pSql )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadSendDev() : m_pRdbOp->createSQL() failed");
		return false;
	}
	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("tdac_senddev");
	pSql->selectAllField();
	pSql->whereField("f_sdevid",0,CSql::CP_GreaterEqual);
	pSql->orderField( "f_sdevid");

	int ret;
	CDataset ds;
	ret = m_pRdbOp->exec( *pSql,ds);

	if ( ret < 0 )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadSendDev() : m_pRdbOp->exec( *pSql,ds) failed");
		pSql->destroy();
		return false;
	}

	int i,j,retno,okRows=0;
	int maxno = -1;

	DAC_RECVDEV_INFO *pRdevInfo = NULL;
	for ( i=0;i<DAC_RECVDEV_NUM;i++ )
	{
		pRdevInfo = &m_dacShm.m_ptrCom->info.recvdev[i];
		pRdevInfo->SdevNum = 0;
		for ( j=0;j<DAC_ROUTE_IN_CHANNEL;j++ )
			pRdevInfo->sdevs[j] = -1;
	}

	for ( i=0,j=0;i<DAC_SENDDEV_NUM;i++ )
	{
		retno = assignSendDev(i,ds,j);
		if ( retno >= 0 )
		{
			j++;
			okRows++;
			if ( retno > maxno )
				maxno = retno;
		}
	}


	m_dacShm.m_ptrCom->info.system.senddevNum = maxno + 1;
	//m_dacShm.m_ptrCom->info.system.senddevNum = okRows;
	pSql->destroy();

	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadStation() : success, okRows = %d",okRows);
	return true;
}


bool	CLoadPara::loadYc(const char* group /*= 0*/)
{
	//-------------------------不加载点的参数-------------------------begin
	//暂不加载点参数，只是开辟参数的内存空间
	//如要加载点参数，则需要注释以下代码，打开后续被注释掉的代码
	/*
	if ( group )
	{
		hInt32 groupNo = m_commInf.groupNo( group );
		m_dacShm.mapYc( groupNo );
	}
	else
	{
		for (int i=0;i<DAC_GROUP_NUM;i++ )
			m_dacShm.mapYc( i );
	}

	return true;
	*/
	//-------------------------不加载点的参数-------------------------end

	//-------------------------加载点的参数---------------------------begin
	//以下代码将加载遥测点参数信息
	CSql* pSql = m_pRdbOp->createSQL();
	if ( !pSql )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYc() : m_pRdbOp->createSQL() failed" );
		return false;
	}

	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("tdac_yc");
	pSql->selectAllField();

	if ( group )
	{
		pSql->whereField( "f_grpcode",group);
	}

	int ret;
	CDataset ds;
	ret = m_pRdbOp->exec( *pSql,ds);
	if ( ret < 0 )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYc() : m_pRdbOp->exec( *pSql,ds) failed" );
		pSql->destroy();
		return false;
	}

	int i,groupNo,ycNo,okRows=0;
	size_t rows = ds.rowSize();
	DAC_YC *pYc; 

	if ( group )
	{
		groupNo = m_commInf.groupNo( group );
		m_dacShm.mapYc( groupNo );
	}
	else
	{
		for ( i=0;i<DAC_GROUP_NUM;i++ )
		{
			m_dacShm.mapYc( i );
		}
	}

	for ( i=0;i<rows;i++ )
	{
		groupNo = m_commInf.groupNo(ds.field(i,"f_grpcode").value().toString().c_str());
		ycNo = ds.field(i,"f_no").value().toInt32();

		if ( groupNo < 0 || groupNo >= DAC_GROUP_NUM )
			continue;
		if ( ycNo <0 || ycNo >= m_dacShm.m_ptrCom->para.group[groupNo].ycNum )
			continue;
		pYc = m_dacShm.ycPara(groupNo);
		if ( !pYc )
			continue;
		pYc =  pYc + ycNo;

		pYc->valid = ds.field(i,"f_useflag").value().toInt8();
		ACE_OS::strncpy(pYc->code,ds.field(i,"f_code").value().toString().c_str(),DAC_CODE_LEN);
		ACE_OS::strncpy(pYc->name,ds.field(i,"f_name").value().toString().c_str(),DAC_NAME_LEN);
		pYc->calc = ds.field(i,"f_calc").value().toInt8();
		ACE_OS::strncpy(pYc->formula,ds.field(i,"f_formula").value().toString().c_str(),DAC_CODE_LEN);
		pYc->type = ds.field(i,"f_type").value().toUInt8();
		pYc->srcGroup = m_commInf.groupNo(ds.field(i,"f_srcgrpcode").value().toString().c_str());
		pYc->srcNo = ds.field(i,"f_srcno").value().toInt32();
		pYc->isEvent = (hBool)ds.field(i,"f_isevent").value().toInt8();
		pYc->eventLevel = ds.field(i,"f_eventlevel").value().toInt8();
		pYc->eventType = ds.field(i,"f_evttype").value().toUInt32();
		ACE_OS::strncpy(pYc->unit,ds.field(i,"f_unit").value().toString().c_str(),DAC_UNIT_LEN);
		pYc->isStat = (hBool)ds.field(i,"f_isstat").value().toInt8();
		pYc->coefK = ds.field(i,"f_coefk").value().toFloat();
		pYc->coefB = ds.field(i,"f_coefb").value().toFloat();
		pYc->saveInterval = ds.field(i,"f_saveinterval").value().toInt32();
		pYc->minChgVal = ds.field(i,"f_minchgval").value().toFloat();
		pYc->highLimit = ds.field(i,"f_high_limit").value().toFloat();
		pYc->lowLimit = ds.field(i,"f_low_limit").value().toFloat();
		pYc->hihiLimit = ds.field(i,"f_hihi_limit").value().toFloat();
		pYc->lolowLimit = ds.field(i,"f_lolow_limit").value().toFloat();
		pYc->zeroVal = ds.field(i,"f_zero_val").value().toFloat();
		pYc->maxIllegal = ds.field(i,"f_max_illegal").value().toFloat();
		pYc->minIllegal = ds.field(i,"f_min_illegal").value().toFloat();
		pYc->maxChange = ds.field(i,"f_max_change").value().toFloat();
		pYc->eventDelay = ds.field(i,"f_event_delay").value().toUInt8();
		pYc->isMsgAlarm = (hBool)ds.field(i,"f_ismsgalarm").value().toInt8();
		pYc->msgAlarmLevel = ds.field(i,"f_msgalarmlevel").value().toUInt8();
		pYc->isControl = (hBool)ds.field(i,"f_isctrl").value().toInt8();
		pYc->ctrlType = ds.field(i,"f_ctrltype").value().toUInt8();
		pYc->ctrlNo = ds.field(i,"f_ctrlno").value().toInt32();
		ACE_OS::strncpy(pYc->mrid,ds.field(i,"f_mrid").value().toString().c_str(),DAC_NAME_LEN);

		okRows++;
	}

	pSql->destroy();

	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYc() : success, okRows = %d",okRows );

	return true;	
	//-------------------------加载点的参数---------------------------end
}

bool	CLoadPara::loadYx(const char* group /*= 0*/)
{
	//-------------------------不加载点参数---------------------------begin
	//暂不加载点参数，只是开辟参数的内存空间
	//如要加载点参数，则需要注释以下代码，打开后续被注释掉的代码
	/*
	if ( group )
	{
		hInt32 groupNo = m_commInf.groupNo( group );
		m_fdcshm.mapYx( groupNo );
	}
	else
	{
		for (int i=0;i<DAC_GROUP_NUM;i++ )
			m_fdcshm.mapYx( i );
	}
	
	return true;
	*/
	//-------------------------不加载点参数---------------------------end

	//-------------------------加载点的参数---------------------------begin
	CSql* pSql = m_pRdbOp->createSQL();
	if ( !pSql )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYx() : m_pRdbOp->createSQL() failed" );
		return false;
	}

	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("tdac_yx");
	pSql->selectAllField();

	if ( group )
	{
		pSql->whereField( "f_grpcode",group);
	}

	int ret;
	CDataset ds;
	ret = m_pRdbOp->exec( *pSql,ds);
	if ( ret < 0 )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYx() : m_pRdbOp->exec( *pSql,ds) failed" );
		pSql->destroy();
		return false;
	}

	int i,groupNo,yxNo,okRows=0;
	size_t rows = ds.rowSize();
	DAC_YX *pYx; 

	if ( group )
	{
		groupNo = m_commInf.groupNo( group );
		m_dacShm.mapYx( groupNo );
	}
	else
	{
		for ( i=0;i<DAC_GROUP_NUM;i++ )
			m_dacShm.mapYx( i );
	}

	for ( i=0;i<rows;i++ )
	{
		groupNo = m_commInf.groupNo( ds.field(i,"f_grpcode").value().toString().c_str() );
		yxNo = ds.field(i,"f_no").value().toInt32();

		if ( groupNo < 0 || groupNo >= DAC_GROUP_NUM )
			continue;
		if ( yxNo <0 || yxNo >= m_dacShm.m_ptrCom->para.group[groupNo].yxNum )
			continue;
		pYx = m_dacShm.yxPara(groupNo);
		if ( !pYx )
			continue;
		pYx = pYx + yxNo;

		pYx->valid = ds.field(i,"f_useflag").value().toInt8();
		ACE_OS::strncpy(pYx->code,ds.field(i,"f_code").value().toString().c_str(),DAC_CODE_LEN);
		ACE_OS::strncpy(pYx->name,ds.field(i,"f_name").value().toString().c_str(),DAC_NAME_LEN);
		pYx->calc = ds.field(i,"f_calc").value().toInt8();
		ACE_OS::strncpy(pYx->formula,ds.field(i,"f_formula").value().toString().c_str(),DAC_CODE_LEN);
		pYx->srcGroup = m_commInf.groupNo(ds.field(i,"f_srcgrpcode").value().toString().c_str());
		pYx->srcNo = ds.field(i,"f_srcno").value().toInt32();

		pYx->type = ds.field(i,"f_type").value().toUInt8();
		pYx->normal = ds.field(i,"f_normal").value().toInt8();
		pYx->isEvent = (hBool)ds.field(i,"f_isevent").value().toInt8();
		pYx->eventLevel = ds.field(i,"f_eventlevel").value().toInt8();
		pYx->eventType = ds.field(i,"f_evttype").value().toUInt32();
		pYx->isStat = (hBool)ds.field(i,"f_isstat").value().toInt8();
		pYx->reverse = (hBool)ds.field(i,"f_reverse").value().toInt8();
		pYx->isMsgAlarm = (hBool)ds.field(i,"f_ismsgalarm").value().toInt8();
		pYx->msgAlarmLevel = ds.field(i,"f_msgalarmlevel").value().toUInt8();
		pYx->isInSgz = ds.field(i,"f_isinsgz").value().toUInt8();
		pYx->isControl = (hBool)ds.field(i,"f_isctrl").value().toInt8();
		pYx->ctrlType = ds.field(i,"f_ctrltype").value().toUInt8();
		pYx->ctrlNo = ds.field(i,"f_ctrlno").value().toInt32();
		ACE_OS::strncpy(pYx->mrid,ds.field(i,"f_mrid").value().toString().c_str(),DAC_NAME_LEN);

		okRows++;
	}

	pSql->destroy();

	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYx() : success, okRows = %d",okRows );

	return true;	
	//-------------------------加载点的参数---------------------------end
}

bool	CLoadPara::loadKwh(const char* group /*= 0*/)
{
	//-------------------------不加载点参数---------------------------begin
	//暂不加载点参数，只是开辟参数的内存空间
	//如要加载点参数，则需要注释以下代码，打开后续被注释掉的代码
	/*
	if ( group )
	{
		hInt32 groupNo = m_commInf.groupNo( group );
		m_fdcshm.mapKwh( groupNo );
	}
	else
	{
		for (int i=0;i<DAC_GROUP_NUM;i++ )
			m_fdcshm.mapKwh( i );
	}

	return true;
	*/
	//-------------------------不加载点参数---------------------------end

	//-------------------------加载点参数----------------------------begin
	CSql* pSql = m_pRdbOp->createSQL();
	if ( !pSql )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadKwh() : m_pRdbOp->createSQL() failed" );
		return false;
	}

	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("tdac_kwh");
	pSql->selectAllField();

	if ( group )
	{
		pSql->whereField( "f_grpcode",group);
	}

	int ret;
	CDataset ds;
	ret = m_pRdbOp->exec( *pSql,ds);
	if ( ret < 0 )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadKwh() : m_pRdbOp->exec( *pSql,ds) failed" );
		pSql->destroy();
		return false;
	}

	int i,groupNo,kwhNo,okRows=0;
	size_t rows = ds.rowSize();
	DAC_KWH *pKwh; 

	if ( group )
	{
		groupNo = m_commInf.groupNo( group );
		m_dacShm.mapKwh( groupNo );
	}
	else
	{
		for ( i=0;i<DAC_GROUP_NUM;i++ )
			m_dacShm.mapKwh( i );
	}

	for ( i=0;i<rows;i++ )
	{
		groupNo = m_commInf.groupNo( ds.field(i,"f_grpcode").value().toString().c_str() );
		kwhNo = ds.field(i,"f_no").value().toInt32();

		if ( groupNo < 0 || groupNo >= DAC_GROUP_NUM )
			continue;
		if ( kwhNo <0 || kwhNo >= m_dacShm.m_ptrCom->para.group[groupNo].kwhNum )
			continue;
		pKwh = m_dacShm.kwhPara(groupNo);
		if ( !pKwh )
			continue;
		pKwh = pKwh  + kwhNo;

		pKwh->valid = ds.field(i,"f_useflag").value().toInt8();
		ACE_OS::strncpy(pKwh->code,ds.field(i,"f_code").value().toString().c_str(),DAC_CODE_LEN);
		//ACE_OS::strncpy(pKwh->alias,ds.field(i,"f_alias").value().toString().c_str(),DAC_CODE_LEN);
		ACE_OS::strncpy(pKwh->name,ds.field(i,"f_name").value().toString().c_str(),DAC_NAME_LEN);
		pKwh->srcGroup = m_commInf.groupNo(ds.field(i,"f_srcgrpcode").value().toString().c_str());
		pKwh->srcNo = ds.field(i,"f_srcno").value().toInt32();
		pKwh->coefK = ds.field(i,"f_coefk").value().toFloat();
		pKwh->coefB = ds.field(i,"f_coefb").value().toFloat();

		okRows++;
	}

	pSql->destroy();

	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadKwh() : success, okRows = %d",okRows );

	return true;
	//-------------------------加载点参数----------------------------end
}

bool	CLoadPara::loadYk()
{
	CSql* pSql = m_pRdbOp->createSQL();
	if ( !pSql )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYk() : m_pRdbOp->createSQL() failed");
		return false;
	}
	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("tdac_yk");
	pSql->selectAllField();

	int ret;
	CDataset ds;
	ret = m_pRdbOp->exec( *pSql,ds);

	if ( ret < 0 )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYk() : m_pRdbOp->exec( *pSql,ds) failed");
		pSql->destroy();
		return false;
	}

	//清空遥控数据
	DAC_YK *pYk = m_dacShm.m_ptrCom->para.yk;
	ACE_OS::memset(pYk,0,sizeof(DAC_YK)*DAC_YK_NUM);

	size_t rows = ds.rowSize();
	for (int i=0;i<rows;i++,pYk++)
	{
		pYk->valid  = true;
		ACE_OS::strncpy(pYk->routeCode,ds.field(i,"f_routecode").value().toString().c_str(),DAC_CODE_LEN );
		ACE_OS::strncpy(pYk->code,ds.field(i,"f_code").value().toString().c_str(),DAC_CODE_LEN );
		pYk->no = ds.field(i,"f_no").value().toInt32();
		ACE_OS::strncpy(pYk->name,ds.field(i,"f_name").value().toString().c_str(),DAC_NAME_LEN );
		pYk->ykno = ds.field(i,"f_ykno").value().toInt32();
		pYk->yktype = ds.field(i,"f_yktype").value().toInt32();
		ACE_OS::strncpy(pYk->mrid,ds.field(i,"f_mrid").value().toString().c_str(),DAC_NAME_LEN );
	}

	pSql->destroy();

	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYk() : success, okRows = %d",rows);
	return true;
}

bool	CLoadPara::loadYt()
{
	CSql* pSql = m_pRdbOp->createSQL();
	if ( !pSql )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYt() : m_pRdbOp->createSQL() failed");
		return false;
	}
	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("tdac_yt");
	pSql->selectAllField();

	int ret;
	CDataset ds;
	ret = m_pRdbOp->exec( *pSql,ds);

	if ( ret < 0 )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYt() : m_pRdbOp->exec( *pSql,ds) failed");
		pSql->destroy();
		return false;
	}

	//清空遥调数据
	DAC_YT *pYt = m_dacShm.m_ptrCom->para.yt;
	ACE_OS::memset(pYt,0,sizeof(DAC_YT)*DAC_YT_NUM);

	size_t rows = ds.rowSize();
	for (int i=0;i<rows;i++,pYt++)
	{
		pYt->valid = true;
		ACE_OS::strncpy(pYt->routeCode,ds.field(i,"f_routecode").value().toString().c_str(),DAC_CODE_LEN );
		ACE_OS::strncpy(pYt->code,ds.field(i,"f_code").value().toString().c_str(),DAC_CODE_LEN );
		pYt->no = ds.field(i,"f_no").value().toInt32();
		ACE_OS::strncpy(pYt->name,ds.field(i,"f_name").value().toString().c_str(),DAC_NAME_LEN );
		pYt->ytno = ds.field(i,"f_ytno").value().toInt32();
		ACE_OS::strncpy(pYt->mrid,ds.field(i,"f_mrid").value().toString().c_str(),DAC_NAME_LEN );
	}

	pSql->destroy();

	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYt() : success, okRows = %d",rows);
	return true;
}

hInt32 CLoadPara::assignStation(hInt32 no,const CDataset& ds,hInt32 rowIdx)
{
	int retno = -1, realNo = -1;
	if ( rowIdx >=0 && rowIdx < ds.rowSize() )
		realNo = ds.field(rowIdx,"f_stano").value().toInt32();

	DAC_STATION oldStation,*pStation = &m_dacShm.m_ptrCom->para.station[no];

	memcpy(&oldStation,pStation,sizeof(DAC_STATION));
	//edit by yaoff on 20171207
	if ( realNo < 0 )
	//if ( no != realNo )
	//edit by yaoff end.
		memset(pStation,0,sizeof(DAC_STATION));
	else
	{
		pStation->valid = true;
		ACE_OS::strncpy(pStation->code,ds.field(rowIdx,"f_stacode").value().toString().c_str(),DAC_CODE_LEN );
		ACE_OS::strncpy(pStation->name,ds.field(rowIdx,"f_staname").value().toString().c_str(),DAC_NAME_LEN );
		pStation->level = ds.field(rowIdx,"f_stalevel").value().toInt32();
		ACE_OS::strncpy(pStation->address,ds.field(rowIdx,"f_staaddress").value().toString().c_str(),DAC_NAME_LEN );
		ACE_OS::strncpy(pStation->sgzCode,ds.field(rowIdx,"f_sgzcode").value().toString().c_str(),DAC_NAME_LEN );
		ACE_OS::strncpy(pStation->mrid,ds.field(rowIdx,"f_mrid").value().toString().c_str(),DAC_NAME_LEN );
		pStation->cmpyid = ds.field(rowIdx,"f_cpyid").value().toInt32();

		//edit by yaoff on 20171207
		retno = rowIdx;
		//retno = realNo;
		//edit by yaoff end.
	}

	if ( memcmp(&oldStation,pStation,sizeof(DAC_STATION)) != 0 )
	{
		//here,do something when need do it;
	}

	return retno;
}

hInt32 CLoadPara::assignRecvDev(hInt32 no,const CDataset& ds,hInt32 rowIdx)
{
	int retno = -1, realNo = -1;
	if ( rowIdx >=0 && rowIdx < ds.rowSize() )
		realNo = ds.field(rowIdx,"f_rdevid").value().toInt32();

	DAC_RECVDEV *pRdev = &m_dacShm.m_ptrCom->para.recvdev[no];

	//edit by yaoff on 20171207
	if ( realNo < 0 )
		//if ( no != realNo )
		//edit by yaoff end.
		memset(pRdev,0,sizeof(DAC_RECVDEV));
	else
	{
		pRdev->valid = true;
		pRdev->recvno = no;
		ACE_OS::strncpy(pRdev->code,ds.field(rowIdx,"f_rdevcode").value().toString().c_str(),DAC_CODE_LEN );
		ACE_OS::strncpy(pRdev->name,ds.field(rowIdx,"f_rdevname").value().toString().c_str(),DAC_NAME_LEN );
		ACE_OS::strncpy(pRdev->chancode,ds.field(rowIdx,"f_chancode").value().toString().c_str(),DAC_CODE_LEN );
		pRdev->frequency = ds.field(rowIdx,"f_band").value().toInt32();
		pRdev->isInit = (hBool)ds.field(rowIdx,"f_isInit").value().toInt8();

		//edit by yaoff on 20171207
		retno = rowIdx;
		//retno = realNo;
		//edit by yaoff end.

	}

	return retno;
}


hInt32 CLoadPara::assignSendDev(hInt32 no,const CDataset& ds,hInt32 rowIdx)
{
	int retno = -1, realNo = -1;
	if ( rowIdx >=0 && rowIdx < ds.rowSize() )
		realNo = ds.field(rowIdx,"f_sdevid").value().toInt32();

	DAC_SENDDEV *pSdev = &m_dacShm.m_ptrCom->para.senddev[no];

//edit by yaoff on 20171207
	if ( realNo < 0 )
	//if ( no != realNo )
//edit by yaoff end.
		memset(pSdev,0,sizeof(DAC_SENDDEV));
	else
	{

		hInt32	rdevNo;
		DAC_RECVDEV_INFO* pRdevInfo = NULL;

		rdevNo = m_commInf.recvdevNo( ds.field(rowIdx,"f_rdevcode").value().toString().c_str() );

		if ( rdevNo < 0 || rdevNo >= DAC_RECVDEV_NUM )
			rdevNo = -1;
		else
		{
			pRdevInfo = &m_dacShm.m_ptrCom->info.recvdev[rdevNo];
			if ( pRdevInfo->SdevNum < DAC_ROUTE_IN_CHANNEL )
			{
				pRdevInfo->sdevs[pRdevInfo->SdevNum] = no;
				pRdevInfo->SdevNum++;
			}
		}

		pSdev->valid = true;
		pSdev->seeno =  no;
		ACE_OS::strncpy(pSdev->code,ds.field(rowIdx,"f_sdevcode").value().toString().c_str(),DAC_CODE_LEN );
		ACE_OS::strncpy(pSdev->name,ds.field(rowIdx,"f_sdevname").value().toString().c_str(),DAC_NAME_LEN );
		ACE_OS::strncpy(pSdev->upseecode,ds.field(rowIdx,"f_upseecode").value().toString().c_str(),DAC_NAME_LEN );
		ACE_OS::strncpy(pSdev->rdevcode,ds.field(rowIdx,"f_rdevcode").value().toString().c_str(),DAC_NAME_LEN );
		ACE_OS::strncpy(pSdev->grpcode,ds.field(rowIdx,"f_grpcode").value().toString().c_str(),DAC_CODE_LEN );
		pSdev->frequency = ds.field(rowIdx,"f_band").value().toInt32();
		pSdev->rtu = ds.field(rowIdx,"f_rtu").value().toInt32();
		pSdev->isInit = (hBool)ds.field(rowIdx,"f_isInit").value().toInt8();
		ACE_OS::strncpy(pSdev->cmac,ds.field(rowIdx,"f_mac").value().toString().c_str(),DAC_MACADDRESS_LEN);
		

	//edit by yaoff on 20171207
		retno = rowIdx;
		//retno = realNo;
	//edit by yaoff end.
		
	}

	return retno;
}


hInt32 CLoadPara::assignChannel(hInt32 no,const CDataset& ds,hInt32 rowIdx)
{
	int retno = -1, realNo = -1;
	if ( rowIdx >=0 && rowIdx < ds.rowSize() )
		realNo = ds.field(rowIdx,"f_channo").value().toInt32();

	DAC_CHANNEL oldChannel,*pChannel = &m_dacShm.m_ptrCom->para.channel[no];
	DAC_CHANNEL_INFO* pChannelInfo = &m_dacShm.m_ptrCom->info.channel[no];

	memcpy(&oldChannel,pChannel,sizeof(DAC_CHANNEL));
	//edit by yaoff on 20171207
	if ( realNo < 0 )
	//if ( no != realNo )
	//edit by yaoff end.
		memset(pChannel,0,sizeof(DAC_CHANNEL));
	else
	{
		pChannel->valid = ds.field(rowIdx,"f_useflag").value().toInt8();
		ACE_OS::strncpy(pChannel->code,ds.field(rowIdx,"f_chancode").value().toString().c_str(),DAC_CODE_LEN );
		ACE_OS::strncpy(pChannel->name,ds.field(rowIdx,"f_channame").value().toString().c_str(),DAC_NAME_LEN );
		pChannel->type = ds.field(rowIdx,"f_type").value().toInt8();
		pChannel->datatype = ds.field(rowIdx,"f_datatype").value().toInt8();
		ACE_OS::strncpy(pChannel->localAddr,ds.field(rowIdx,"f_localaddr").value().toString().c_str(),DAC_ADDR_LEN );
		ACE_OS::strncpy(pChannel->remoteAddr[0],ds.field(rowIdx,"f_remoteaddr1").value().toString().c_str(),DAC_ADDR_LEN );
		ACE_OS::strncpy(pChannel->remoteAddr[1],ds.field(rowIdx,"f_remoteaddr2").value().toString().c_str(),DAC_ADDR_LEN );	
		pChannel->timeout = ds.field(rowIdx,"f_timeout").value().toInt32();
		pChannel->switchTimeOut = ds.field(rowIdx,"f_switch_timeout").value().toInt32();
		ACE_OS::strncpy(pChannel->eiiSerialNo,ds.field(rowIdx,"f_eii_serialno").value().toString().c_str(),DAC_EIISERIAL_LEN);
		ACE_OS::strncpy(pChannel->eiiPara,ds.field(rowIdx,"f_eii_para").value().toString().c_str(),DAC_EIIPARA_LEN);
		//wfp add at 20101230
		pChannel->isEvent = ds.field(rowIdx,"f_isevent").value().toInt8();
		pChannel->eventLevel = ds.field(rowIdx,"f_eventLevel").value().toInt8();
		pChannel->isMsgAlarm = ds.field(rowIdx,"f_ismsgalarm").value().toInt8();
		pChannel->msgAlarmLevel = ds.field(rowIdx,"f_msgalarmlevel").value().toInt8();
		pChannel->initState = ds.field(rowIdx,"f_initstate").value().toInt8();

		//edit by yaoff on 20171207
		retno = rowIdx;
		//retno = realNo;
		//edit by yaoff end.
	}

	if ( memcmp(&oldChannel,pChannel,sizeof(DAC_CHANNEL)) != 0 )
	{
		//here,do something when need do it;

		if ( pChannel->valid != oldChannel.valid || pChannel->type != oldChannel.type )
		{
			pChannelInfo->reset = CHANNEL_TASK_ALL;
		}

		if ( strcmp(pChannel->localAddr,oldChannel.localAddr) != 0 )
			pChannelInfo->reset = CHANNEL_TASK_ALL;

		for( int i=0;i<DAC_ADDR_NUM;i++ )
		{
			if ( strcmp(pChannel->remoteAddr[i],oldChannel.remoteAddr[i] ) != 0 )
			{
				pChannelInfo->reset = CHANNEL_TASK_ALL;
				break;
			}
		}

		if ( strcmp(pChannel->eiiSerialNo,oldChannel.eiiSerialNo) != 0 
			 || strcmp(pChannel->eiiPara,oldChannel.eiiPara) != 0 
			 || pChannel->datatype != oldChannel.datatype )
			pChannelInfo->reset |= CHANNEL_TASK_USER;

	}

	return retno;
}
hInt32 CLoadPara::assignGroup(hInt32 no,const CDataset& ds,hInt32 rowIdx,bool withData /*=true*/)
{
	int retno = -1, realNo = -1;
	if ( rowIdx >=0 && rowIdx < ds.rowSize() )
		realNo = ds.field(rowIdx,"f_grpno").value().toInt32();

	DAC_GROUP oldGroup;
	DAC_GROUP *pGroup = &m_dacShm.m_ptrCom->para.group[no];
	DAC_GROUP_INFO* pGroupInfo = &m_dacShm.m_ptrCom->info.group[no];

	memcpy(&oldGroup,pGroup,sizeof(DAC_GROUP));
	//edit by yaoff on 20171207
	if ( realNo < 0 )
	//if ( no != realNo )
	//edit by yaoff end.
	{
		memset(pGroup,0,sizeof(DAC_GROUP));
		pGroup->station = -1;	//wfp add at 20100523
	}
	else
	{
		pGroup->valid = ds.field(rowIdx,"f_useflag").value().toInt8();
		ACE_OS::strncpy(pGroup->code,ds.field(rowIdx,"f_devcode").value().toString().c_str(),DAC_CODE_LEN );
		ACE_OS::strncpy(pGroup->name,ds.field(rowIdx,"f_devname").value().toString().c_str(),DAC_NAME_LEN );
		pGroup->station = m_commInf.stationNo(ds.field(rowIdx,"f_stacode").value().toString().c_str());
		pGroup->type = ds.field(rowIdx,"f_type").value().toInt8();
		pGroup->ycNum = ds.field(rowIdx,"f_ycnum").value().toInt32();
		pGroup->yxNum = ds.field(rowIdx,"f_yxnum").value().toInt32();
		pGroup->kwhNum = ds.field(rowIdx,"f_kwhnum").value().toInt32();
		pGroup->deadTime = ds.field(rowIdx,"f_deadtime").value().toUInt32();
		//wfp add at 20101230
		pGroup->isEvent = ds.field(rowIdx,"f_isevent").value().toInt8();
		pGroup->eventLevel = ds.field(rowIdx,"f_eventLevel").value().toInt8();
		pGroup->isMsgAlarm = ds.field(rowIdx,"f_ismsgalarm").value().toInt8();
		pGroup->msgAlarmLevel = ds.field(rowIdx,"f_msgalarmlevel").value().toInt8();
		ACE_OS::strncpy(pGroup->mrid,ds.field(rowIdx,"f_mrid").value().toString().c_str(),DAC_NAME_LEN );
		pGroup->initState = ds.field(rowIdx,"f_initstate").value().toInt8();

		//edit by yaoff on 20171207
		retno = rowIdx;
		//retno = realNo;
		//edit by yaoff end.
	}

	if ( memcmp(&oldGroup,pGroup,sizeof(DAC_GROUP)) != 0 )
	{
		if ( withData && strcmp( oldGroup.code,pGroup->code) != 0 )
		{
			m_dacShm.mapYc( no );
			loadYc(pGroup->code);
			loadYx(pGroup->code);
			loadKwh(pGroup->code);
		}
		//here,do something when need do it;
	}

	return retno;
}

hInt32 CLoadPara::assignRoute(hInt32 no,const CDataset& ds,hInt32 rowIdx)
{
	int retno = -1, realNo = -1,oldNo = -1;
	if ( rowIdx >=0 && rowIdx < ds.rowSize() )
		realNo = ds.field(rowIdx,"f_routeno").value().toInt32();

	DAC_ROUTE oldRoute;
	DAC_ROUTE *pRoute = &m_dacShm.m_ptrCom->para.route[no];
	DAC_ROUTE_INFO* pRouteInfo = &m_dacShm.m_ptrCom->info.route[no];

	memcpy(&oldRoute,pRoute,sizeof(DAC_ROUTE));
	//edit by yaoff on 20171207
	if ( realNo < 0 )
	//if ( no != realNo )
	//edit by yaoff end.
	{
		memset(pRoute,0,sizeof(DAC_ROUTE));
		pRoute->group = -1;
		pRoute->channel  = -1;
	}
	else
	{
		hInt32	groupNo,channelNo;
		DAC_GROUP_INFO*	pGroupInfo = NULL;
		DAC_CHANNEL_INFO* pChannelInfo = NULL;

		groupNo = m_commInf.groupNo( ds.field(rowIdx,"f_grpcode").value().toString().c_str() );
		channelNo = m_commInf.channelNo( ds.field(rowIdx,"f_chancode").value().toString().c_str() );
		if ( groupNo < 0 || groupNo >= DAC_GROUP_NUM )
			groupNo = -1;
		else 
		{
			pGroupInfo = &m_dacShm.m_ptrCom->info.group[groupNo];
			if ( pGroupInfo->routeNum < DAC_ROUTE_IN_GROUP )
			{
				pGroupInfo->routes[pGroupInfo->routeNum] = no;
				pGroupInfo->routeNum++;
				pGroupInfo->mainRoute = pGroupInfo->routes[0];
			}
		}
		if ( channelNo < 0 || channelNo >= DAC_CHANNEL_NUM )
			channelNo = -1;
		else
		{
			pChannelInfo = &m_dacShm.m_ptrCom->info.channel[channelNo];
			if ( pChannelInfo->routeNum < DAC_ROUTE_IN_CHANNEL )
			{
				pChannelInfo->routes[pChannelInfo->routeNum] = no;
				pChannelInfo->routeNum++;
			}
		}

		pRoute->valid = ds.field(rowIdx,"f_useflag").value().toInt8();
		ACE_OS::strncpy(pRoute->code,ds.field(rowIdx,"f_routecode").value().toString().c_str(),DAC_CODE_LEN );
		ACE_OS::strncpy(pRoute->name,ds.field(rowIdx,"f_routename").value().toString().c_str(),DAC_NAME_LEN );
		pRoute->type = ds.field(rowIdx,"f_type").value().toInt8();
		pRoute->group = groupNo;
		pRoute->channel = channelNo;
		pRoute->localAddr = ds.field(rowIdx,"f_localaddr").value().toInt32();
		pRoute->remoteAddr = ds.field(rowIdx,"f_remoteaddr").value().toInt32();
		pRoute->protocol = m_commInf.protocolNo( ds.field(rowIdx,"f_protocode").value().toString().c_str());
		ACE_OS::strncpy(pRoute->featureCode,ds.field(rowIdx,"f_featurecode").value().toString().c_str(),DAC_CODE_LEN );
		pRoute->scanInterval.all_data= ds.field(rowIdx,"f_all_cycle").value().toUInt32();
		pRoute->scanInterval.all_yc= ds.field(rowIdx,"f_allyc_cycle").value().toUInt32();
		pRoute->scanInterval.all_yx = ds.field(rowIdx,"f_allyx_cycle").value().toUInt32();
		pRoute->scanInterval.change_data = ds.field(rowIdx,"f_chg_cycle").value().toUInt32();
		pRoute->scanInterval.change_yc = ds.field(rowIdx,"f_chgyc_cycle").value().toUInt32();
		pRoute->scanInterval.change_yx = ds.field(rowIdx,"f_chgyx_cycle").value().toUInt32();
		
		pRoute->scanInterval.timesync = ds.field(rowIdx,"f_timecycle").value().toInt32();
		//wfp add
		pRoute->scanInterval.change_kwh = pRoute->scanInterval.change_yx;
		pRoute->scanInterval.soe = pRoute->scanInterval.change_data;

		pRoute->isReadOnly = ds.field(rowIdx,"f_readonly").value().toInt8();

		pRoute->stopLimit = ds.field(rowIdx,"f_stoplimit").value().toInt32();
		pRoute->errRateLimit = ds.field(rowIdx,"f_errratelimit").value().toInt32();
		pRoute->errTimeLimit = ds.field(rowIdx,"f_errtimelimit").value().toInt32();
		pRoute->cmdTimeout = ds.field(rowIdx,"f_cmdtimeout").value().toInt32();
		pRoute->cmdRetryInterval = ds.field(rowIdx,"f_cmdretryintv").value().toInt32();
		pRoute->cmdRetryCnt = ds.field(rowIdx,"f_cmdretrycnt").value().toInt32();
		//wfp add at 20101230
		pRoute->isEvent = ds.field(rowIdx,"f_isevent").value().toInt8();
		pRoute->eventLevel = ds.field(rowIdx,"f_eventLevel").value().toInt8();
		pRoute->isMsgAlarm = ds.field(rowIdx,"f_ismsgalarm").value().toInt8();
		pRoute->msgAlarmLevel = ds.field(rowIdx,"f_msgalarmlevel").value().toInt8();
		pRoute->initState = ds.field(rowIdx,"f_initstate").value().toInt8();

		//edit by yaoff on 20171207
		retno = rowIdx;
		//retno = realNo;
		//edit by yaoff end.
	}

	if ( memcmp(&oldRoute,pRoute,sizeof(DAC_ROUTE)) != 0 )
	{
		//if ( pRoute->protocol != oldRoute.protocol )	//wfp masked at 20100406
		pRouteInfo->delProtocol = true;
		
		pRouteInfo->initFlag = true;
	}

	return retno;
}

hInt32 CLoadPara::assignProtocol(hInt32 no,const CDataset& ds,hInt32 rowIdx)
{
	int retno = -1, realNo = -1;
	if ( rowIdx >=0 && rowIdx < ds.rowSize() )
		realNo = ds.field(rowIdx,"f_protono").value().toInt32();

	DAC_PROTOCOL oldProtocol;
	DAC_PROTOCOL *pProtocol = &m_dacShm.m_ptrCom->para.protocol[no];

	memcpy(&oldProtocol,pProtocol,sizeof(DAC_PROTOCOL));
	if ( no != realNo )
		memset(pProtocol,0,sizeof(DAC_PROTOCOL));
	else
	{
		pProtocol->valid = true;
		ACE_OS::strncpy(pProtocol->code,ds.field(rowIdx,"f_protocode").value().toString().c_str(),DAC_CODE_LEN );
		ACE_OS::strncpy(pProtocol->name,ds.field(rowIdx,"f_protoname").value().toString().c_str(),DAC_NAME_LEN );
		ACE_OS::strncpy(pProtocol->libName,ds.field(rowIdx,"f_libname").value().toString().c_str(),DAC_SNAME_LEN );
		ACE_OS::strncpy(pProtocol->featureTable,ds.field(rowIdx,"f_featuretable").value().toString().c_str(),DAC_SNAME_LEN );

		retno = realNo;
	}

	if ( memcmp(&oldProtocol,pProtocol,sizeof(DAC_PROTOCOL)) != 0 )
	{
		//here,do something when need do it;
	}

	return retno;
}


bool	CLoadPara::loadYccon()
{
	CSql* pSql = m_pRdbOp->createSQL();
	if ( !pSql )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYccon() : m_pRdbOp->createSQL() failed");
		return false;
	}
	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("tdac_summondata");
	pSql->selectAllField();

	int ret;
	CDataset ds;
	ret = m_pRdbOp->exec( *pSql,ds);

	if ( ret < 0 )
	{
		logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYccon() : m_pRdbOp->exec( *pSql,ds) failed");
		pSql->destroy();
		return false;
	}

	//清空遥调数据
	DAC_YCCON *pYccon = m_dacShm.m_ptrCom->para.yccon;
	ACE_OS::memset(pYccon,0,sizeof(DAC_YCCON)*DAC_YK_NUM);

	size_t rows = ds.rowSize();
	for (int i=0;i<rows;i++,pYccon++)
	{
		if (i >= DAC_YK_NUM)
		{
			break;
		}
		pYccon->valid = true;
		pYccon->ntype = ds.field(i,"f_type").value().toUInt16();
		ACE_OS::strncpy(pYccon->code,ds.field(i,"f_code").value().toString().c_str(),DAC_CODE_LEN );
		ACE_OS::strncpy(pYccon->name,ds.field(i,"f_name").value().toString().c_str(),DAC_NAME_LEN );
		pYccon->ndatatype = ds.field(i,"f_datatype").value().toUInt16();
	}

	pSql->destroy();

	logprint(LOG_LOADPARA_BASE,"CLoadPara::loadYccon() : success, okRows = %d",rows);
	return true;
}



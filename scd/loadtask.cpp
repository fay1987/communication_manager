#include "loadtask.h"
#include "dac/dacdef.h"
#include <ace/OS.h>
#include "rdbop/rdbop.h"
#include "scddef.h"
#include "sys/blog.h"
#include "config.h"

using namespace PDT;

CLoadTask::CLoadTask()
{
	ACE_OS::memset(m_isBatteryDevice,0,sizeof(m_isBatteryDevice));
}

CLoadTask::~CLoadTask()
{
}

bool CLoadTask::init()
{
	//加载参数
	//if ( !loadDevice() )
	//{
	//	return false;
	//}

	loadFormula();		//wlq change at 20160608
	//loadBeginKwh();

	return true;
}

int CLoadTask::start()
{
	logprint(LOG_SCD_BASE,"thread:%d , loadTask启动!",ACE_Thread::self());
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

void CLoadTask::stop()
{
	if (thr_count() > 0) thr_mgr()->cancel_task(this);
}

hInt32 CLoadTask::svc()
{
	DAC_SYSTEM_INFO* pSystemInfo = m_commInf.systemInfo();
	ACE_Thread_Manager *mgr = this->thr_mgr ();
	ACE_Time_Value tv(0,10000);

	//loadFormula();		//wfp add at 20100930

	while ( 1 )
	{
		if (mgr->testcancel (mgr->thr_self ()))
			break;

		ACE_OS::sleep(tv);

		//if(	pSystemInfo->loadParaFlag & LOADPARA_DEVICE 
		//	||pSystemInfo->loadParaFlag & LOADPARA_DEVICETYPE )
		//{
		//	loadDevice();
		//	pSystemInfo->loadParaFlag &= ~LOADPARA_DEVICE;
		//	pSystemInfo->loadParaFlag &= ~LOADPARA_DEVICETYPE;
		//}

		//可动态加载计算公式表，但该方式并不稳妥，可能捕获不到loadParaFlag变化
		if (  PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_GROUP)
			||PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_YC)
			||PDT_BIT_ENABLED(pSystemInfo->loadParaFlag,LOADPARA_YX))
		{
			loadFormula();		//wfp add at 20100930
			ACE_OS::sleep(1);
			continue;
		}


	}//end while

	logprint(LOG_SCD_BASE,"threadId = %d , scd loadtask线程退出!",ACE_Thread::self());

	return 0;
}

//蓄电池参数加载
bool CLoadTask::loadDevice()
{
	if ( !CONFIG::instance()->hasBattery() ) return true;

	CRdbOp* pRdbOp = new CRdbOp(CRdbOp::PingDirect);
	if ( !pRdbOp )
	{
		logprint(LOG_SCD_BASE,"<loadDevice> can't new CRdbOp object,load para failed");
		return false;
	}

	CSql* pSql = pRdbOp->createSQL();
	if ( !pSql )
	{
		logprint(LOG_SCD_BASE,"<loadDevice> pRdbOp->createSQL() failed");
		delete pRdbOp;
		pRdbOp = NULL;
		return false;
	}

	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("tdac_device");
	pSql->selectAllField();
	pSql->whereField("f_devtypecode","xdc");

	CDataset ds;
	if ( pRdbOp->exec( *pSql,ds) < 0 )
	{
		logprint(LOG_SCD_BASE,"<loadDevice> pRdbOp->exec( *pSql,ds) failed");
		pSql->destroy();
		delete pRdbOp;
		pRdbOp = NULL;
		return false;
	}

	hUInt32 okRows = 0;
	size_t rows = ds.rowSize();

	int groupNo = -1;
	for ( hUInt32 i=0;i<rows;i++ )
	{
		groupNo = m_commInf.groupNo(ds.field(i,"f_devcode").value().toString().c_str());

		if ( m_commInf.isGroupValid(groupNo) )
		{
			m_isBatteryDevice[groupNo] = true;
		}
	
		okRows++;
	}

	pSql->destroy();
	delete pRdbOp;
	pRdbOp = NULL;

	logprint(LOG_SCD_BASE,"<loadDevice> success, okRows = %d",okRows);
	return true;
}

bool CLoadTask::isBatteryValid(hInt32 group)
{
	if ( !m_commInf.isGroupValid(group) )  return false;
	if ( !m_isBatteryDevice[group] )	return false;
	return true;
}

int CLoadTask::batteryGroupNum()
{
	int num = 0;
	for (int i=0;i<DAC_GROUP_NUM;i++)
	{
		if ( isBatteryValid(i) )
			++num;
	}
	return num;
}

/*
 * 加载计算公式表
 */
bool CLoadTask::loadFormula()
{
	m_formulaMap.clear();
	SCD_FORMULA_DATA formulaData;
	CString	code;

	CRdbOp* pRdbOp = new CRdbOp(CRdbOp::PingDirect);
	if ( !pRdbOp )
	{
		logprint(LOG_SCD_BASE,"<loadFormula> can't new CRdbOp object,load para failed");
		return false;
	}

	CSql* pSql = pRdbOp->createSQL();
	if ( !pSql )
	{
		logprint(LOG_SCD_BASE,"<loadFormula> pRdbOp->createSQL() failed");
		delete pRdbOp;
		pRdbOp = NULL;
		return false;
	}

	pSql->setOperate( CSql::OP_Select );
	pSql->setTableName("tscd_formula");
	pSql->selectAllField();

	CDataset ds;
	if ( pRdbOp->exec( *pSql,ds) < 0 )
	{
		logprint(LOG_SCD_BASE,"<loadFormula> pRdbOp->exec( *pSql,ds) failed");
		pSql->destroy();
		delete pRdbOp;
		pRdbOp = NULL;
		return false;
	}

	hUInt32 okRows = 0;
	size_t rows = ds.rowSize();

	for ( hUInt32 i=0;i<rows;i++ )
	{
		code = ds.field(i,"f_code").value().toString();
		formulaData.name = ds.field(i,"f_name").value().toString();
		formulaData.type = ds.field(i,"f_type").value().toUInt8();
		formulaData.content = ds.field(i,"f_content").value().toString();
		m_formulaMap[code] = formulaData;

		okRows++;
	}

	pSql->destroy();
	delete pRdbOp;
	pRdbOp = NULL;

	logprint(LOG_SCD_BASE,"<loadFormula> success, okRows = %d",okRows);
	return true;
}

bool CLoadTask::formula(const char* code,SCD_FORMULA_DATA& data)
{
	if ( code == NULL ) return false;

	FORMULA_MAP::iterator iter = m_formulaMap.find( CString(code) );
	if ( iter != m_formulaMap.end() )
	{
		data.name = (*iter).second.name;
		data.type = (*iter).second.type;
		data.content = (*iter).second.content;
		return true;
	}

	return false;
}


/*
 * 加载计算电量初始值表
 */
bool CLoadTask::loadBeginKwh()
{
	m_beginkwhMap.clear();
	SCD_BEGINKWH_DATA beginkwhData;
	CString	code;

	CRdbOp* pRdbOp = new CRdbOp(CRdbOp::PingDirect);
	if ( !pRdbOp )
	{
		logprint(LOG_SCD_BASE,"<loadBeginkwh> can't new CRdbOp object,load para failed");
		return false;
	}

	CString sqlstr = CString("select * from tscd_beginkwh order by f_grpcode ASC , f_code ASC ");

	CDataset ds;
	if( pRdbOp->exec(sqlstr.c_str() , ds) < 0)
	{
		logprint(LOG_SCD_BASE,"pRdbOp->exec() failed,sqlStr = %s",sqlstr.c_str());
		delete pRdbOp;
		pRdbOp = NULL;
		return false;
	}

	hUInt32 okRows = 0;
	size_t rows = ds.rowSize();

	for ( hUInt32 i=0;i<rows;i++ )
	{
		code = ds.field(i,"f_grpcode").value().toString() + "/" + ds.field(i,"f_code").value().toString();
		/*BEGINKWH_MAP::iterator iter = m_beginkwhMap.find( CString(code) );
		if ( iter != m_beginkwhMap.end() )
		{
			if( ds.field(i,"f_type").value().toUInt8() == 1 )
				(*iter).second.dayvalue = ds.field(i,"f_value").value().toFloat();
			else if ( ds.field(i,"f_type").value().toUInt8() == 2 )
				(*iter).second.monthvalue = ds.field(i,"f_value").value().toFloat();
			else if ( ds.field(i,"f_type").value().toUInt8() == 3 )
				(*iter).second.yearvalue = ds.field(i,"f_value").value().toFloat();
			else ;
		}
		else
		{
			if( ds.field(i,"f_type").value().toUInt8() == 1 )
				beginkwhData.dayvalue = ds.field(i,"f_value").value().toFloat();
			else if ( ds.field(i,"f_type").value().toUInt8() == 2 )
				beginkwhData.monthvalue = ds.field(i,"f_value").value().toFloat();
			else if ( ds.field(i,"f_type").value().toUInt8() == 3 )
				beginkwhData.yearvalue = ds.field(i,"f_value").value().toFloat();
			else ;
			m_beginkwhMap[code] = beginkwhData;
		}*/
		beginkwhData.dayvalue = ds.field(i,"f_dvalue").value().toFloat();
		beginkwhData.monthvalue = ds.field(i,"f_mvalue").value().toFloat();
		beginkwhData.yearvalue = ds.field(i,"f_yvalue").value().toFloat();
		m_beginkwhMap[code] = beginkwhData;
		okRows++;
	}
	delete pRdbOp;
	pRdbOp = NULL;

	logprint(LOG_SCD_BASE,"<loadBeginkwh> success, okRows = %d",okRows);
	return true;
}

/*
 * 更新计算电量初始值表
 */
bool CLoadTask::updateBeginKwh()
{
	//CString	code;

	CRdbOp* pRdbOp = new CRdbOp(CRdbOp::PingDirect);
	if ( !pRdbOp )
	{
		logprint(LOG_SCD_BASE,"<updateBeginKwh> can't new CRdbOp object,load para failed");
		return false;
	}

	CString sqlstr = CString("delete from tscd_beginkwh");

	CDataset ds;
	if( pRdbOp->exec(sqlstr.c_str() , ds) < 0)
	{
		logprint(LOG_SCD_BASE,"pRdbOp->exec() failed,sqlStr = %s",sqlstr.c_str());
		delete pRdbOp;
		pRdbOp = NULL;
		return false;
	}

	PDT::CSql*				m_pSql;
	m_pSql = pRdbOp->createSQL();
	if ( m_pSql == NULL ) 
	{
		logprint(LOG_SCD_BASE,"<updateBeginKwh> pRdbOp->createSQL() failed");
		delete pRdbOp;
		pRdbOp = NULL;
		return false;
	}

	BEGINKWH_MAP::iterator iter = m_beginkwhMap.begin();

	while (iter != m_beginkwhMap.end()) 
	{
		m_pSql->setOperate(CSql::OP_Insert);
		m_pSql->setTableName("tscd_beginkwh");

		int startPos = 0; 
		int position = (*iter).first.find('/');
		CString		tmpString;
		if ( position != CString::npos )
			tmpString = (*iter).first.substr(startPos,position-startPos);
		m_pSql->insertField("f_grpcode", tmpString.c_str());

		position += 1;
		startPos = position;
		tmpString = (*iter).first.substr(startPos,(*iter).first.length()-startPos);
		m_pSql->insertField("f_code",tmpString.c_str());

		//m_pSql->insertField("f_grpcode", CString((*iter).first).trimLeft('/').c_str());
		//m_pSql->insertField("f_code",CString((*iter).first).trimRight('/').c_str());
		m_pSql->insertField("f_dvalue",(*iter).second.dayvalue);
		m_pSql->insertField("f_mvalue",(*iter).second.monthvalue);
		m_pSql->insertField("f_yvalue",(*iter).second.yearvalue);

		if ( pRdbOp->exec(*m_pSql) == -1 )
		{
			break;
		}
		++iter;
	}

	delete pRdbOp;
	pRdbOp = NULL;
	delete m_pSql;
	m_pSql = NULL;

	logprint(LOG_SCD_BASE,"<updateBeginKwh> success, okRows = %d",m_beginkwhMap.size());
	return true;
}

bool CLoadTask::beginkwh(const char* code,SCD_BEGINKWH_DATA& data)
{
	if ( code == NULL ) return false;

	BEGINKWH_MAP::iterator iter = m_beginkwhMap.find( CString(code) );
	if ( iter != m_beginkwhMap.end() )
	{
		data.dayvalue = (*iter).second.dayvalue;
		data.monthvalue = (*iter).second.monthvalue;
		data.yearvalue = (*iter).second.yearvalue;
		return true;
	}

	return false;
}

bool CLoadTask::setBeginkwh(const char* code,SCD_BEGINKWH_DATA data)
{
	if ( code == NULL ) return false;

	BEGINKWH_MAP::iterator iter = m_beginkwhMap.find( CString(code) );
	if ( iter != m_beginkwhMap.end() )
	{
		(*iter).second.dayvalue = data.dayvalue;
		(*iter).second.monthvalue = data.monthvalue;
		(*iter).second.yearvalue = data.yearvalue;
		return true;
	}
	else
		m_beginkwhMap[CString(code)] = data;

	return false;
}


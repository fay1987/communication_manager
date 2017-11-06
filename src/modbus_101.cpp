
#include "modbus_101.h"

#include "dac/toolbase.h"
#include "sys/blog.h"
#include "dac/datainf.h"
#include <bitset>

#include "rdbop/rdbop.h"
#include "rdb/field.h"
#include "rdb/fieldinfo.h"
#include "iec61850/iedinf.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include "dac/dacctrl.h"
#include <ace/OS.h>
#include <QDir>

using namespace PDT;

//float sgval[6][3]={{1.0,12.5,3.0},{3.0,14.5,2.0},{2.0,15.0,5.0},{1.5,10.0,2.5},{1.2,10.4,3.5},{1.7,11.5,3.8}};
#define  POINTNO_ERRRATE	304
//录波数据有Ia Ib Ic Ua Ub Uc 六个通道  每个通道数据分五个批次请求
#define  RECORD_CHANEL_NUM	6
#define  RECORD_CMD_NUM		5


CProto_Modbus_101::CProto_Modbus_101()
{
	m_open = false;
	m_grpno = -1;
	m_nSOENum = -1;
	m_nRecordsNum = -1;
	m_bOk = false;
	memset(m_filePathName,0,sizeof(m_filePathName));
	m_nflag = 0;
	m_pcmd = new PMC_Feature;
}

CProto_Modbus_101::~CProto_Modbus_101()
{
	delete m_pcmd;
	m_pcmd = NULL;
	close();
}

bool CProto_Modbus_101::isOpen() const
{
	return m_open;
}

bool CProto_Modbus_101::open()
{
	//初始化变量
	memset( m_polling_feature,0,sizeof(PMC_Feature) * PMC_FEATURE_RECORD_MAXNUM );
	memset( &m_current_send_cmd, 0, sizeof(PMC_Current_Send_Command) );
	//memset( m_buf,0, CTRL_COMMON_LEN);
	clearRecvMsg();
	//置规约初始状态为成功
	m_protocolEvent = Succeeded;

	//获取路径参数信息并初始化定时器参数和规约重试次数
	DAC_ROUTE	*m_proute = m_commInf.route(m_route);
	if (m_proute == NULL)
	{
		logprint(LOG_PROTOCOL + m_route, "加载路径信息出错！");
		return false;
	}

	//超时判断时间
	m_rtimeout_id = m_timeSet.add( m_proute->cmdTimeout * 100 );
	//通道轮询时间间隔，借用了路径表中的变化数据扫描间隔
	m_scaninter_id = m_timeSet.add( m_proute->scanInterval.change_data * 100 );
	//超时重发次数
	m_retryCount = m_proute->cmdRetryCnt;
	//是否监听模式
	m_islisting = m_proute->isReadOnly;
	//站址
	m_remoteAddr = m_proute->remoteAddr;

	m_grpno = m_proute->group ;
	//读取规约特征表
	readFeature( m_proute );

	QMap<hInt32,hInt32>::iterator iter = m_mapTimeout_id.begin();  
	while(iter != m_mapTimeout_id.end())  
	{  
		m_timeSet.start(iter.value());
		++iter;  
	}  

	//启动通道轮询定时器
	m_timeSet.start(m_scaninter_id);

	//是否第一次发送命令
	m_firstSend = true;
	//默认第1条为规约建立链接后第一次发出的命令
	m_current_send_cmd.currFeatureptr = m_polling_feature;
	m_currPollCmdno = -1;

	//所有参数初始化完毕，置规约打开状态为TRUE
	m_open = true;

	m_commInf.routeInfo(m_route)->switchFlag = true;

	//初始化MMS内存，主要是读指针位置
	/*if ( !initScl() )
	{
		printf("initScl()...faild!!!\n");
		return false;
	}
	printf("initScl()...ok\n");*/

	//m_ykReadPos = getYkWritePos();

	logprint(LOG_PROTOCOL + m_route,"timeOut:%d,timeOUT ID: %d ",m_proute->cmdTimeout,m_rtimeout_id);

	return true ;
}

bool CProto_Modbus_101::readFeature( DAC_ROUTE	*m_proute )
{
	if( m_proute == NULL ) return false;
	m_routeVector.clear();
	m_mapTimeout_id.clear();

	//从特征表加载参数
	DAC_PROTOCOL* pProtocol = m_commInf.protocol(m_proute->protocol);
	if ( pProtocol == NULL )
	{
		logprint(LOG_PROTOCOL + m_route,"获取规约参数失败");
		return false;
	}

	if ( ACE_OS::strlen(pProtocol->featureTable) <= 0 )
	{
		logprint(LOG_PROTOCOL + m_route,"特征表没有配置,请先配置!");
		return false;
	}


	CRdbOp *prdbop = new CRdbOp(CRdbOp::PingDirect);
	if( !prdbop )
	{
		logprint(LOG_PROTOCOL + m_route, "can't new CRdbOp object!");
		return false;
	}

	CSql *pSql = prdbop->createSQL();
	if( !pSql )
	{
		logprint(LOG_PROTOCOL + m_route, "RdbOp->createSQL() failed!");
		delete prdbop;
		prdbop = NULL;
		return false;
	}

	pSql->setOperate( CSql::OP_Select);
	pSql->setTableName(pProtocol->featureTable);
	pSql->selectAllField();
	pSql->whereField("f_routecode", m_proute->code);
	pSql->whereLogic(CSql::LG_And);
	pSql->whereField("f_useflag", 1, CSql::CP_Equal);
	pSql->orderField("f_cmdno",CSql::S_Asc);

	CDataset ds;
	if( prdbop->exec(*pSql , ds) < 0)
	{
		logprint(LOG_PROTOCOL + m_route,"pRdbOp->exec( *pSql,ds) failed");
		pSql->destroy();
		delete prdbop;
		prdbop = NULL;
		return false;
	}

	//hUInt32 okRows = 0;
	size_t rows = ds.rowSize();

	int recordnum = (rows > PMC_FEATURE_RECORD_MAXNUM) ? PMC_FEATURE_RECORD_MAXNUM : rows ;

	for(hInt32 i = 0 ; i < recordnum; i++ )
	{
		m_polling_feature[i].isUsed = true;

		CField field = ds.field(i, "f_cmdno");//命令号
		m_polling_feature[i].cmdNo = field.value().toInt32();

		field = ds.field(i,"f_cmddesc");//命令描述
		ACE_OS::strncpy(m_polling_feature[i].cmdDesc, field.value().toString().c_str(), PMC_DESCRIBE_STRING_LENGTH);

		field = ds.field(i , "f_isack");//命令是否需要回复
		m_polling_feature[i].isAck = field.value().toUInt8();
		
		field = ds.field(i, "f_fc");//功能码
		m_polling_feature[i].fc = QString(field.value().toString().c_str()).toUInt(0,16);

		field = ds.field(i, "f_fc2");//功能码2
		m_polling_feature[i].fc2 = QString(field.value().toString().c_str()).toUInt(0,16);

		field = ds.field(i, "f_fclength");//功能码位数
		m_polling_feature[i].fcLength = field.value().toUInt8();

		field = ds.field(i, "f_frmtype");//帧类型
		m_polling_feature[i].frmType = field.value().toUInt8();

		field = ds.field(i, "f_addrh");//寄存器地址高位
		m_polling_feature[i].addrH = QString(field.value().toString().c_str()).toUInt(0,16);

		field = ds.field(i, "f_addrl");//寄存器地址低位
		m_polling_feature[i].addrL = QString(field.value().toString().c_str()).toUInt(0,16);

		field = ds.field(i, "f_lengthh");//数据长度高位
		m_polling_feature[i].lengthH = QString(field.value().toString().c_str()).toUInt(0,16);

		field = ds.field(i, "f_lengthl");//数据长度低位
		m_polling_feature[i].lengthL = QString(field.value().toString().c_str()).toUInt(0,16);

		field = ds.field(i, "f_routeno");//本帧数据路径号
		m_polling_feature[i].routeNo = field.value().toInt32();

		field = ds.field(i, "f_startno");//本帧数据测点起始号
		m_polling_feature[i].startNo = field.value().toInt32();

		field = ds.field(i, "f_nextcmdno");//下一个循环命令号
		m_polling_feature[i].nextCmdNo = field.value().toInt32();

		field = ds.field(i, "f_checkfunc");//校验方式
		m_polling_feature[i].checkFunc = field.value().toUInt8();

		if( m_polling_feature[i].frmType == Control || m_polling_feature[i].frmType == SetValue )
		{
			m_routeVector.append(m_polling_feature[i].routeNo);
		}

		if (m_polling_feature[i].frmType == Analog || m_polling_feature[i].frmType == Discrete)
		{
			if (m_polling_feature[i].fcLength > 0)
			{
				hInt32 rtimeout_id = m_timeSet.add( m_polling_feature[i].fcLength * 1000 );
				m_mapTimeout_id.insert(m_polling_feature[i].cmdNo, rtimeout_id);
			}
		}
	}

	pSql->destroy();
	delete prdbop;
	prdbop = NULL;
	if(rows == 0)
	{
		logprint(LOG_PROTOCOL + m_route, "规约特征表中没有配置对应的参数！");
		return false;
	}
	return true;
}

void CProto_Modbus_101::addFeatureYKYT()
{
	for(int i = 0 ; i < PMC_FEATURE_RECORD_MAXNUM ; i ++)
	{
		if(m_polling_feature[i].isUsed) continue;

		m_polling_feature[i].cmdNo = i + 1;
		ACE_OS::strcpy(m_polling_feature[i].cmdDesc, "遥控命令");
		m_polling_feature[i].fc = 0x06;
		m_polling_feature[i].checkFunc = 0;
		m_polling_feature[i].isAck = 1;
		m_polling_feature[i].isUsed = 1;
		m_polling_feature[i].frmType = Control;
		m_polling_feature[i].routeNo = m_route;
		m_polling_feature[i].startNo = -1;
		break;
	}

	for(int i = 0 ; i < PMC_FEATURE_RECORD_MAXNUM ; i ++)
	{
		if(m_polling_feature[i].isUsed) continue;

		m_polling_feature[i].cmdNo = i + 1;
		ACE_OS::strcpy(m_polling_feature[i].cmdDesc, "设置命令");
		m_polling_feature[i].fc = 0x06;
		m_polling_feature[i].checkFunc = 0;
		m_polling_feature[i].isAck = 1;
		m_polling_feature[i].isUsed = 1;
		m_polling_feature[i].frmType = SetValue;
		m_polling_feature[i].routeNo = m_route;
		m_polling_feature[i].startNo = -1;
		break;
	}
}

void CProto_Modbus_101::createSTMatrix()
{
	return ;
}

void CProto_Modbus_101::close()
{
	//清除计时器
	m_timeSet.remove(m_rtimeout_id);
	m_timeSet.remove(m_scaninter_id);

	m_routeVector.clear();
	m_mapTimeout_id.clear();

	m_open = false;
}


void CProto_Modbus_101::mainTx()
{
	//如果是监听模式，该规约不发送任何下发报文
	if( m_islisting ) return ;

	if(1/*m_timeSet.time_out(m_scaninter_id)*/)//时间周期到
	{
		if( !isSending() )
		{
			if(checkCurrentCommand())
			{
				if(!m_current_send_cmd.currFeatureptr->isUsed)
				{
					return;
				}
				makeCommand(m_current_send_cmd.currFeatureptr);	//根据当前命令特征信息组建当前发送的报文
				setCmdinfoToBuf();	//把发送命令发送到内存
				clearRecvMsg();		//同时清空接收缓冲区

				/*如果是无回应报文，则直接置数m_protocolEvent = Succeeded*/
				if( m_current_send_cmd.currFeatureptr->isAck )
				{
					m_timeSet.start( m_rtimeout_id );
					reportCmdSend(true);
					m_protocolEvent = Waitting; 
					m_bOk = false;
				}
				else m_protocolEvent = Succeeded;				
			}
		}
	}
}

bool CProto_Modbus_101::checkCurrentCommand()
{
	//规约下发后还没有接收到任何报文，应该继续等待报文
	if ( m_protocolEvent == Waitting ) return false;

	//如果是超时或者是校验错误，在不是控制命令（或不需要重发的命令）和连发在cmdRetryCnt以内的前提下，则重发当前报文，否则继续进行下一个命令
	if ( m_protocolEvent == TimeOut || m_protocolEvent == CheckError )
	{	
		if( m_current_send_cmd.currFeatureptr->frmType == Control || m_current_send_cmd.currFeatureptr->frmType == SetValue )
		{
			m_protocolEvent = RetryCountOver;
		}
		else if( ++m_current_send_cmd.retryCount < m_retryCount ) 
		{
			return true;
		}
		else 
		{
			m_protocolEvent = RetryCountOver;
		}
	}

	if (m_protocolEvent == Succeeded || m_protocolEvent == RetryCountOver)
	{
		findNextCommand();
		m_current_send_cmd.retryCount = 0;
		return true;
	}

	return false;
}

void CProto_Modbus_101::findNextCommand()
{
	//如果有注册命令发送，则直接发送注册命令
	/*if ( m_pchannel->datatype == DATATYPE_EII )
	{
		if ( !m_reg_ok ) 
		{
			make_REG();
			return;
		}
	}*/

	if (m_nSOENum > 0)
	{
		getSOEData();
	}
	else if (m_record_feature.size() > 0)
	{
		getRecordsData();
	}
	else if (m_vecPara.size() > 0)
	{
		sendUdp();
	}
	//检查是否有控制命令
	else if(hasAnyManualCommand()) 
	{
		logprint(LOG_PROTOCOL + m_route, "有人工控制命令插入，命令号为");
	}
	//检查是否有定时触发命令
	else if(hasAnyOnTimerCommand())
	{
		logprint(LOG_PROTOCOL + m_route, "有定时命令，命令号为");
	}
	//正常轮询到下一条命令
	else
	{
		if( m_firstSend )
		{
			//默认第1条为规约建立链接后第一次发出的命令
			m_current_send_cmd.currFeatureptr = m_polling_feature;
			m_currPollCmdno = m_current_send_cmd.currFeatureptr->cmdNo;
			m_firstSend = false;
		}
		else
		{
			//根据规约特征表中的配置寻找该帧的下一帧该发送的报文

			PMC_Feature_ptr p = findFeatureByCmdno( m_current_send_cmd.currFeatureptr->nextCmdNo );
			while(1)
			{
				if (p == NULL)		//找不到,默认指向第一条
				{
					p = m_polling_feature;
					//m_current_send_cmd.currFeatureptr = p;
					//m_currPollCmdno = p->cmdNo;
					break;
				}
				else if (p->frmType == Control || p->frmType == SetValue) //找到为控制或设置指令，这默认指向第一条
				{
					p = m_polling_feature;
					break;
				}
				else //找到了 也不是控制或设置指令 判断是否为定时
				{

					if (m_mapTimeout_id.contains(p->cmdNo))
					{
						 hInt32 rtime_id = m_mapTimeout_id.value(p->cmdNo);
						 if (m_timeSet.time_out(rtime_id))
						 {
							 m_timeSet.start(rtime_id);
							 break;
						 }
						 else
						 {
							 p = findFeatureByCmdno( p->nextCmdNo );
						 }
					}
					else
					{
						break;
					}

				}
			}
			 

			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;
			////如果找不到，或者找到为控制或设置指令，这默认指向第一条
			//if( p != NULL ) 
			//{
			//	if(p->frmType == Control || p->frmType == SetValue) p = m_polling_feature;
			//	m_current_send_cmd.currFeatureptr = p;
			//	m_currPollCmdno = p->cmdNo;
			//}
			//else
			//{
			//	 p = m_polling_feature;
			//	 m_current_send_cmd.currFeatureptr = p;
			//	 m_currPollCmdno = p->cmdNo;
			//}
		}
	}

	logprint(LOG_PROTOCOL + m_route, "循环到下一个命令，命令号为%s[%d]",
		m_current_send_cmd.currFeatureptr->cmdDesc,m_current_send_cmd.currFeatureptr->cmdNo);
}

PMC_Feature_ptr	CProto_Modbus_101::findFeatureByCmdno(hInt32	cmdNo)
{
	if( cmdNo < 0 ) return NULL;

	PMC_Feature_ptr p = m_polling_feature;
	int	 i;

	for( i = 0 ; i < PMC_FEATURE_RECORD_MAXNUM ; i ++ , p ++ )
	{
		if(p->cmdNo == cmdNo) break;
	}

	if( i == PMC_FEATURE_RECORD_MAXNUM ) return NULL;
	else return p;
}

PMC_Feature_ptr	CProto_Modbus_101::findFeatureControl()
{
	
	PMC_Feature_ptr p = m_polling_feature;
	int	 i;

	for( i = 0 ; i < PMC_FEATURE_RECORD_MAXNUM ; i ++ , p ++ )
	{
		if(!p->isUsed) continue;
		if(p->fc == 0x06 && p->frmType == Control) break;
	}

	if( i == PMC_FEATURE_RECORD_MAXNUM ) return NULL;
	else return p;
}

PMC_Feature_ptr	CProto_Modbus_101::findFeatureControl(int no)
{
	
	PMC_Feature_ptr p = m_polling_feature;
	int	 i;

	for( i = 0 ; i < PMC_FEATURE_RECORD_MAXNUM ; i ++ , p ++ )
	{
		if(!p->isUsed) continue;
		if(p->startNo == no && p->frmType == Control )
		{
			break;
		}
	}

	if( i == PMC_FEATURE_RECORD_MAXNUM ) return NULL;
	else return p;
}

PMC_Feature_ptr	CProto_Modbus_101::findFeatureControl(int no,hUChar actionType)
{
	
	PMC_Feature_ptr p = m_polling_feature;
	int	 i;

	for( i = 0 ; i < PMC_FEATURE_RECORD_MAXNUM ; i ++ , p ++ )
	{
		if(!p->isUsed) continue;
		if(p->startNo == no && p->frmType == Control && p->fcLength == actionType)
		{
			break;
		}
	}

	if( i == PMC_FEATURE_RECORD_MAXNUM ) return NULL;
	else return p;
}

PMC_Feature_ptr	CProto_Modbus_101::findFeatureSetValue()
{
	PMC_Feature_ptr p = m_polling_feature;
	int	 i;

	for( i = 0 ; i < PMC_FEATURE_RECORD_MAXNUM ; i ++ , p ++ )
	{
		if(!p->isUsed) continue;
		if(p->fc == 0x06 && p->frmType == SetValue) break;
	}

	if( i == PMC_FEATURE_RECORD_MAXNUM ) return NULL;
	else return p;
}

PMC_Feature_ptr	CProto_Modbus_101::findFeatureSyncTime()
{
	PMC_Feature_ptr p = m_polling_feature;
	int	 i;

	for( i = 0 ; i < PMC_FEATURE_RECORD_MAXNUM ; i ++ , p ++ )
	{
		if(!p->isUsed) continue;
		if(p->fc == 0x7F && p->frmType == SyncTime) break;
	}

	if( i == PMC_FEATURE_RECORD_MAXNUM ) return NULL;
	else return p;
}

PMC_Feature_ptr	CProto_Modbus_101::findFeatureSetValue(int no)
{
	PMC_Feature_ptr p = m_polling_feature;
	int	 i;

	for( i = 0 ; i < PMC_FEATURE_RECORD_MAXNUM ; i ++ , p ++ )
	{
		if(!p->isUsed) continue;
		if(p->startNo == no && p->frmType == SetValue) break;
	}

	if( i == PMC_FEATURE_RECORD_MAXNUM ) return NULL;
	else return p;
}

PMC_Feature_ptr	CProto_Modbus_101::findFeatureConstantValue(hUChar	fc)
{
	PMC_Feature_ptr p = m_polling_feature;
	int	 i;

	for( i = 0 ; i < PMC_FEATURE_RECORD_MAXNUM ; i ++ , p ++ )
	{
		if(!p->isUsed) continue;
		if(p->fc == fc && p->frmType == ConstantValue) break;
	}

	if( i == PMC_FEATURE_RECORD_MAXNUM ) return NULL;
	else return p;
}


PMC_Feature_ptr	CProto_Modbus_101::findFeatureSetValue(int no,hUChar actionType)
{
	PMC_Feature_ptr p = m_polling_feature;
	int	 i;

	for( i = 0 ; i < PMC_FEATURE_RECORD_MAXNUM ; i ++ , p ++ )
	{
		if(!p->isUsed) continue;
		if(p->startNo == no && p->frmType == SetValue && p->fcLength == actionType) break;
	}

	if( i == PMC_FEATURE_RECORD_MAXNUM ) return NULL;
	else return p;
}

bool CProto_Modbus_101::hasAnyManualCommand()
{
	//规约建立以后，第一帧不发送控制命令
	if( m_firstSend ) return false;

	bool ret = false;
	char	tmpbuf[DAC_CTRL_LEN];

	//if(m_bflag)
	//{
	//	PDT::CString strCode = "grp23";
	//	ctrl_pro_common common;
	//	ACE_OS::strncpy(common.groupCode, strCode.c_str(), DAC_NAME_LEN);  //赋值commom编码
	//	common.length = CTRL_COMMON_LEN;


	//	unsigned char buf[512];
	//	hInt32 headLen,dataLen;

	//	headLen = sizeof( ctrl_head );
	//	dataLen = sizeof( ctrl_pro_common );
	//	ctrl_head*	pHead = (ctrl_head*)buf;
	//	pHead->type = CTRL_PRO_CONSTANTVALUE;
  

	//
	//	PDT::CString str = "07;00;0A;1A;01;00;00;00";

	//	ctrl_pro_constantvalue* pYk = ( ctrl_pro_constantvalue* )( buf + headLen/* + headLen*/);
	//	pYk->groupNo = 22;

	//	pYk->ctrlType = CONSTANTVALUE_WRITE;
	//	pYk->length = str.length();
	//	ACE_OS::memcpy(pYk->cmdBuf,str.c_str(),pYk->length);
	//	
	//	//pYk->ctrlType = CONSTANTVALUE_READ;
	//	//pYk->length = 0

	//	ACE_OS::memcpy(common.cmdBuf,buf,CTRL_COMMON_LEN);

	//	CDacCtrl m_dacCtrl; 
	//	if ( !m_dacCtrl.commonCmd( &common ) ) 
	//	{
	//		return 1;
	//	}
	//
	//	m_bflag = false;
	//}
	//} 

	


	//unsigned char buf[512];
	//hInt32 headLen,dataLen;

	//headLen = sizeof( ctrl_head );
	//dataLen = sizeof( ctrl_pro_common );
	//ctrl_head*	pHead1 = (ctrl_head*)buf;
	//pHead1->type = CTRL_PRO_YCCOMMAND;
	//ctrl_pro_calldata* pData = ( ctrl_pro_calldata* )( buf + headLen/* + headLen*/);
	//pData->groupNo = 11;
	//pData->ctrlNo = 1;

	//PDT::CString strCode = "grp12";
	//ctrl_pro_common common;
	//ACE_OS::strncpy(common.groupCode, strCode.c_str(), DAC_NAME_LEN);  //赋值commom编码
	//common.length = CTRL_COMMON_LEN;
	//ACE_OS::memcpy(common.cmdBuf,buf,CTRL_COMMON_LEN);
	//CDacCtrl m_dacCtrl; 
	//if ( !m_dacCtrl.commonCmd( &common ) ) 
	//{
	//	return 1;
	//}
	
	m_nflag ++;

	if (m_nflag == 1 && m_route == 0)
	{
		unsigned char buf[512];
		hInt32 headLen,dataLen;

		headLen = sizeof( ctrl_head );
		dataLen = sizeof( ctrl_pro_common );
		ctrl_head*	pHead1 = (ctrl_head*)buf;
		pHead1->type = CTRL_PRO_YCCOMMAND;
		ctrl_pro_calldata* pData = ( ctrl_pro_calldata* )( buf + headLen/* + headLen*/);
		//pData->groupNo = 0;
		pData->ctrlNo = 1;

		PDT::CString strCode = "grp0";
		ctrl_pro_common common;
		ACE_OS::strncpy(common.groupCode, strCode.c_str(), DAC_NAME_LEN);  //赋值commom编码
		common.length = CTRL_COMMON_LEN;
		ACE_OS::memcpy(common.cmdBuf,buf,CTRL_COMMON_LEN);
		CDacCtrl m_dacCtrl; 
		if ( !m_dacCtrl.commonCmd( &common ) ) 
		{
			return 1;
		}

		//unsigned char buf[512];
		//hInt32 headLen,dataLen;

		//headLen = sizeof( ctrl_head );
		//dataLen = sizeof( ctrl_pro_common );
		//ctrl_head*	pHead = (ctrl_head*)buf;
		//pHead->type = CTRL_PRO_CALLRECORDSDATA;//CTRL_PRO_CALLSOEDATA;	

		//PDT::CString strCode = "grp12";
		//ctrl_pro_common common;
		//ACE_OS::strncpy(common.groupCode, strCode.c_str(), DAC_NAME_LEN);  //赋值commom编码
		//common.length = CTRL_COMMON_LEN;
		//ACE_OS::memcpy(common.cmdBuf,buf,CTRL_COMMON_LEN);
		//CDacCtrl m_dacCtrl; 
		//if ( !m_dacCtrl.commonCmd( &common ) ) 
		//{
		//	return 1;
		//}
	}


#ifdef	SUPPORT_MUTL_ROUTE

	for (int i = 0; i < m_routeVector.size(); ++i ) 
	{
		if(!getCtrlCmdByRoute(m_routeVector.at(i),tmpbuf,DAC_CTRL_LEN)) continue;
		else break;
	}

	if( i == m_routeVector.size() ) return ret;
#else
	if(!getCtrlCmd(tmpbuf,DAC_CTRL_LEN)) return ret;
#endif

	ctrl_head* pHead = (ctrl_head*)tmpbuf;

	//pHead->type = CTRL_PRO_CALLSOEDATA;
	//ctrl_pro_common* pCommon = (ctrl_pro_common*)(tmpbuf+sizeof(ctrl_head));

	//CTRL_PRO_YK						=	208,		//遥控
	//	CTRL_PRO_YT						=	209,		//遥调
	//	CTRL_PRO_SETPOINT_YC			=	210,		//设点遥测
	//	CTRL_PRO_SETPOINT_YX			=	211,		//设点遥信
	//	CTRL_PRO_SETPOINT_KWH			=	212,		//设点电度
	//	CTRL_PRO_DIRECT_OPERATE			=	213,		//直接操作
	if(pHead->type == CTRL_PRO_YK)
	{
		ctrl_pro_yk* pYk = (ctrl_pro_yk*)(tmpbuf + sizeof(ctrl_head));
		switch(pYk->ctrlType)
		{
		case YK_ACTION_SELECT:
		case YK_ACTION_SELECT_V:
			{
				ret = make_yk_select( pYk );
				/*ctrl_pro_yk_ack  ack;
				memset(&ack,0,sizeof(ctrl_pro_yk_ack));
				ack.ctrlNo = pYk->ctrlNo;
				ack.funcCode = pYk->ctrlType;
				ack.groupNo = m_grpno;
				ack.ackState = pYk->ctrlState;				
				setCtrlCmdAck((char*)(&ack),sizeof(ctrl_pro_yk_ack));	*/			
				logprint(LOG_PROTOCOL  + m_route, "SCL_YK_ACTION_SELECT 0控制选择)");
			}
			break;
		case YK_ACTION_EXECUTE:
			{
				ret = make_yk_execute( pYk );
				logprint(LOG_PROTOCOL  + m_route, "SCL_YK_ACTION_EXECUTE 0控制执行)");
			}
			break;
		case YK_ACTION_CANCEL:
			{
				ctrl_pro_yk_ack  ack;
				memset(&ack,0,sizeof(ctrl_pro_yk_ack));
				ack.ctrlNo = pYk->ctrlNo;
				ack.funcCode = pYk->ctrlType;
				ack.ackState = pYk->ctrlState;	
			#ifdef SUPPORT_MUTL_ROUTE
				PMC_Feature_ptr p = findFeatureControl(pYk->ctrlNo,1);
				if( p == NULL )
				{
					p = findFeatureControl(pYk->ctrlNo);
					if( p == NULL ) p = findFeatureControl( -1 , 0);
					if( p == NULL )
					{
						addFeatureYKYT();
						p = findFeatureControl();
					}
				}
				if( p != NULL )
				{
					ack.groupNo = p->routeNo;	//m_grpno; 
				}	
				setCtrlCmdAckEx((char*)(&ack),sizeof(ctrl_pro_yk_ack));	
			#else
				ack.groupNo = m_grpno; 
				setCtrlCmdAck((char*)(&ack),sizeof(ctrl_pro_yk_ack));	
			#endif
							
							
				logprint(LOG_PROTOCOL  + m_route, "SCL_YK_ACTION_CANCEL 0控制撤销)");
			}
		}		
	}
	else if(pHead->type == CTRL_PRO_YT)
	{
		ctrl_pro_setpointex* pSet = (ctrl_pro_setpointex*)(tmpbuf + sizeof(ctrl_head));
		switch(pSet->ctrlType)
		{
		case	YK_ACTION_SELECT:
		case	YK_ACTION_SELECT_V:
			ret = make_select_parameter(pSet);
			logprint(LOG_PROTOCOL  + m_route, "SCL_SELECT_POINT 遥调选择");
			break;
		case	YK_ACTION_EXECUTE:
			ret = make_set_parameter(pSet);
			break;
			logprint(LOG_PROTOCOL  + m_route, "SCL_SET_POINT 遥调执行");
		}
	}
	else if (pHead->type == CTRL_PRO_CONSTANTVALUE)
	{
		ctrl_pro_constantvalue* pCommon = (ctrl_pro_constantvalue*)(tmpbuf+sizeof(ctrl_head));
		switch(pCommon->ctrlType)
		{
		case CONSTANTVALUE_READ:
			 ret = make_constantValue_read(pCommon);
			break;
		case CONSTANTVALUE_WRITE:
			 ret = make_constantValue_write(pCommon);
			break;
		}
	}
	else if (pHead->type == CTRL_PRO_CALLSOEDATA)
	{
		m_isExclusive = true;
		ret = getSOEData();
	}
	else if (pHead->type == CTRL_PRO_CALLRECORDSDATA)
	{
		m_isExclusive = true;
		ret = getRecordsData();
	}
	else if (pHead->type == CTRL_PRO_YCCOMMAND)
	{
		ctrl_pro_calldata* pCommon = (ctrl_pro_calldata*)(tmpbuf+sizeof(ctrl_head));
		ret = sendYcCommand(pCommon);
	}
	else if (pHead->type == CTRL_PRO_CALLELEC)
	{
		ctrl_pro_calldata* pCommon = (ctrl_pro_calldata*)(tmpbuf + sizeof(ctrl_head));
		//ret = sendElecCommand(pCommon);
	}
	else if (pHead->type == CTRL_PRO_UDP)
	{
		ret = sendUdp();
	}
	else if (pHead->type == CTRL_PRO_EDITPRAR)
	{
		ctrl_pro_constantvalue* pCommon = (ctrl_pro_constantvalue*)(tmpbuf+sizeof(ctrl_head));
		ret = editpara(pCommon);
	}
	else{;}


	return ret;
}

bool CProto_Modbus_101::make_yk_select(void* pyk)
{
	if( pyk == NULL ) return false;

	ctrl_pro_yk*	pYk = (ctrl_pro_yk*)pyk;

	//根据规约特征表中的配置寻找该帧的下一帧该发送的报文
	PMC_Feature_ptr p = findFeatureControl(pYk->ctrlNo,1);
	if( p == NULL )
	{
		p = findFeatureControl(pYk->ctrlNo);
		if( p == NULL ) p = findFeatureControl( -1 , 0);
		if( p == NULL )
		{
			addFeatureYKYT();
			p = findFeatureControl();
		}
		if( p != NULL )
		{
			if(p->fc == 0x10)
			{
#ifdef	SUPPORT_MUTL_ROUTE
				//m_current_send_cmd.cmd[0] = p->routeNo;
				DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
				m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
				m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
				m_current_send_cmd.cmd[1] = p->fc;
				hUInt16 tmp_addr = pYk->ctrlNo;
				p->addrH = HIBYTE(tmp_addr);
				p->addrL = LOBYTE(tmp_addr);
				m_current_send_cmd.cmd[2] = p->addrH;
				m_current_send_cmd.cmd[3] = p->addrL;
				m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
				m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;
				m_current_send_cmd.cmd[6] = 0x02;/*p->lengthL*/;
				m_current_send_cmd.cmd[7] = 0x00;
				if( pYk->ctrlState == YK_ON )m_current_send_cmd.cmd[8] = p->lengthH;
				else	m_current_send_cmd.cmd[8] = p->lengthL;

				hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 9);
				m_current_send_cmd.cmd[9] = HIBYTE(crc);
				m_current_send_cmd.cmd[10] = LOBYTE(crc);
				m_current_send_cmd.length = 11;

				ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
				p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
				memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
				p->ykptr.ctrlNo = pYk->ctrlNo;
				p->ykptr.ctrlType = pYk->ctrlType;
				m_current_send_cmd.currFeatureptr = p;
				m_currPollCmdno = p->cmdNo;
				return true;
			}
			else
			{
				p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;

				
#ifdef	SUPPORT_MUTL_ROUTE
				//m_current_send_cmd.cmd[0] = p->routeNo;
				DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
				m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
				m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
				m_current_send_cmd.cmd[1] = p->fc;
				hUInt16 tmp_addr = pYk->ctrlNo;
				m_current_send_cmd.cmd[2] = HIBYTE(tmp_addr);
				m_current_send_cmd.cmd[3] = LOBYTE(tmp_addr);
				if( pYk->ctrlState == YK_ON )
				{
					m_current_send_cmd.cmd[4] = p->addrH;
					m_current_send_cmd.cmd[5] = p->lengthH;/*p->lengthL*/; 
				}
				else
				{
					m_current_send_cmd.cmd[4] = p->addrL;
					m_current_send_cmd.cmd[5] = p->lengthL;/*p->lengthL*/; 
				}

				hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
				m_current_send_cmd.cmd[6] = HIBYTE(crc);
				m_current_send_cmd.cmd[7] = LOBYTE(crc);
				m_current_send_cmd.length = 8;

				memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
				p->ykptr.ctrlNo = pYk->ctrlNo;
				p->ykptr.ctrlType = pYk->ctrlType;
				m_current_send_cmd.currFeatureptr = p;
				m_currPollCmdno = p->cmdNo;
				ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);

				return true;
			}
		}
	}
	else
	{
		if(p->fc == 0x10)
		{
#ifdef	SUPPORT_MUTL_ROUTE
			//m_current_send_cmd.cmd[0] = p->routeNo;
			DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
			m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
			m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
			m_current_send_cmd.cmd[1] = p->fc;
			m_current_send_cmd.cmd[2] = p->addrH;
			m_current_send_cmd.cmd[3] = p->addrL;
			m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
			m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;
			m_current_send_cmd.cmd[6] = 0x02;/*p->lengthL*/;
			m_current_send_cmd.cmd[7] = 0x00;
			if( pYk->ctrlState == YK_ON )m_current_send_cmd.cmd[8] = p->lengthH;
			else	m_current_send_cmd.cmd[8] = p->lengthL;

			hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 9);
			m_current_send_cmd.cmd[9] = HIBYTE(crc);
			m_current_send_cmd.cmd[10] = LOBYTE(crc);
			m_current_send_cmd.length = 11;

			p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
			memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
			p->ykptr.ctrlNo = pYk->ctrlNo;
			p->ykptr.ctrlType = pYk->ctrlType;
			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;
			ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
			return true;
		}
		else
		{
			p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;

#ifdef	SUPPORT_MUTL_ROUTE
			//m_current_send_cmd.cmd[0] = p->routeNo;
			DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
			m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
			m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
			m_current_send_cmd.cmd[1] = p->fc;
			m_current_send_cmd.cmd[2] = p->addrH;
			m_current_send_cmd.cmd[3] = p->addrL;
			switch( p->fc2 )
			{
			case	0x10:
				if( pYk->ctrlState == YK_ON )
				{
					m_current_send_cmd.cmd[4] = p->lengthH;
					m_current_send_cmd.cmd[5] = 0x00;/*p->lengthL*/; 
				}
				else
				{
					m_current_send_cmd.cmd[4] = p->lengthL;
					m_current_send_cmd.cmd[5] = 0x00;/*p->lengthL*/; 
				}
				break;
			case	0x11:
				if( pYk->ctrlState == YK_ON )
				{
					m_current_send_cmd.cmd[4] = p->lengthH;
					m_current_send_cmd.cmd[5] = p->lengthH;;/*p->lengthL*/; 
				}
				else
				{
					m_current_send_cmd.cmd[4] = p->lengthL;
					m_current_send_cmd.cmd[5] = p->lengthL;/*p->lengthL*/; 
				}
				break;
			case	0x00:
			default:
				if( pYk->ctrlState == YK_ON )
				{
					m_current_send_cmd.cmd[4] = 0x00;
					m_current_send_cmd.cmd[5] = p->lengthH;;/*p->lengthL*/; 
				}
				else
				{
					m_current_send_cmd.cmd[4] = 0x00;
					m_current_send_cmd.cmd[5] = p->lengthL;/*p->lengthL*/; 
				}
				break;
			}

			hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
			m_current_send_cmd.cmd[6] = HIBYTE(crc);
			m_current_send_cmd.cmd[7] = LOBYTE(crc);
			m_current_send_cmd.length = 8;

			memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
			p->ykptr.ctrlNo = pYk->ctrlNo;
			p->ykptr.ctrlType = pYk->ctrlType;
			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;
			ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);

			return true;
		}
	}

	return false;	
}

bool CProto_Modbus_101::make_yk_cancel(void* pyk)
{
	if( pyk == NULL ) return false;

	return false;	
}

bool CProto_Modbus_101::make_yk_execute(void* pyk)
{
	if( pyk == NULL ) return false;

	ctrl_pro_yk*	pYk = (ctrl_pro_yk*)pyk;

	//根据规约特征表中的配置寻找该帧的下一帧该发送的报文
	PMC_Feature_ptr p = findFeatureControl(pYk->ctrlNo,2);
	if( p == NULL )
	{
		p = findFeatureControl(pYk->ctrlNo);
		if( p == NULL ) p = findFeatureControl( -1 , 0);
		if( p == NULL )
		{
			addFeatureYKYT();
			p = findFeatureControl();
		}
		if( p != NULL )
		{
			if(p->fc == 0x10)
			{
#ifdef	SUPPORT_MUTL_ROUTE
				//m_current_send_cmd.cmd[0] = p->routeNo;
				DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
				m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
				m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
				m_current_send_cmd.cmd[1] = p->fc;
				hUInt16 tmp_addr = pYk->ctrlNo;
				p->addrH = HIBYTE(tmp_addr);
				p->addrL = LOBYTE(tmp_addr);
				m_current_send_cmd.cmd[2] = p->addrH;
				m_current_send_cmd.cmd[3] = p->addrL;
				m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
				m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;
				m_current_send_cmd.cmd[6] = 0x02;/*p->lengthL*/;
				if(p->fc2 == 0xB0)
				{
					m_current_send_cmd.cmd[8] = 0x00;
					if( pYk->ctrlState == YK_ON )m_current_send_cmd.cmd[7] = p->lengthH;
					else	m_current_send_cmd.cmd[7] = p->lengthL;
				}
				else
				{		
					m_current_send_cmd.cmd[7] = 0x00;
					if( pYk->ctrlState == YK_ON )m_current_send_cmd.cmd[8] = p->lengthH;
					else	m_current_send_cmd.cmd[8] = p->lengthL;
				}

				hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 9);
				m_current_send_cmd.cmd[9] = HIBYTE(crc);
				m_current_send_cmd.cmd[10] = LOBYTE(crc);
				m_current_send_cmd.length = 11;

				ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
				p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
				//p->routeNo = pYk->grpno;
				memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
				p->ykptr.ctrlNo = pYk->ctrlNo;
				p->ykptr.ctrlType = pYk->ctrlType;
				m_current_send_cmd.currFeatureptr = p;
				m_currPollCmdno = p->cmdNo;
				return true;
			}
			else
			{
				p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;

				
#ifdef	SUPPORT_MUTL_ROUTE
				//m_current_send_cmd.cmd[0] = p->routeNo;
				DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
				m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
				m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
				m_current_send_cmd.cmd[1] = p->fc;
				hUInt16 tmp_addr = pYk->ctrlNo;
				m_current_send_cmd.cmd[2] = HIBYTE(tmp_addr);
				m_current_send_cmd.cmd[3] = LOBYTE(tmp_addr);
				if( pYk->ctrlState == YK_ON )
				{
					m_current_send_cmd.cmd[4] = p->addrH;
					m_current_send_cmd.cmd[5] = p->lengthH;/*p->lengthL*/; 
				}
				else
				{
					m_current_send_cmd.cmd[4] = p->addrL;
					m_current_send_cmd.cmd[5] = p->lengthL;/*p->lengthL*/; 
				}

				hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
				m_current_send_cmd.cmd[6] = HIBYTE(crc);
				m_current_send_cmd.cmd[7] = LOBYTE(crc);
				m_current_send_cmd.length = 8;

				//p->routeNo = pYk->grpno;
				//p->ykptr = pyk;
				memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
				p->ykptr.ctrlNo = pYk->ctrlNo;
				p->ykptr.ctrlType = pYk->ctrlType;
				m_current_send_cmd.currFeatureptr = p;
				m_currPollCmdno = p->cmdNo;
				ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);

				return true;
			}
		}
	}
	else
	{
		if(p->fc == 0x10)
		{
#ifdef	SUPPORT_MUTL_ROUTE
			//m_current_send_cmd.cmd[0] = p->routeNo;
			DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
			m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
			m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
			m_current_send_cmd.cmd[1] = p->fc;
			m_current_send_cmd.cmd[2] = p->addrH;
			m_current_send_cmd.cmd[3] = p->addrL;
			m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
			m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;
			m_current_send_cmd.cmd[6] = 0x02;/*p->lengthL*/;
			if(p->fc2 == 0xB0)
			{
				m_current_send_cmd.cmd[8] = 0x00;
				if( pYk->ctrlState == YK_ON )m_current_send_cmd.cmd[7] = p->lengthH;
				else	m_current_send_cmd.cmd[7] = p->lengthL;
			}
			else
			{
				m_current_send_cmd.cmd[7] = 0x00;
				if( pYk->ctrlState == YK_ON )m_current_send_cmd.cmd[8] = p->lengthH;
				else	m_current_send_cmd.cmd[8] = p->lengthL;
			}

			hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 9);
			m_current_send_cmd.cmd[9] = HIBYTE(crc);
			m_current_send_cmd.cmd[10] = LOBYTE(crc);
			m_current_send_cmd.length = 11;

			p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
			//p->routeNo = pYk->grpno;
			//p->ykptr = pyk;
			memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
			p->ykptr.ctrlNo = pYk->ctrlNo;
			p->ykptr.ctrlType = pYk->ctrlType;
			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;
			ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
			return true;
		}
		else
		{
			p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;

			//p->routeNo = pYk->grpno;
#ifdef	SUPPORT_MUTL_ROUTE
			//m_current_send_cmd.cmd[0] = p->routeNo;
			DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
			m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
			m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
			m_current_send_cmd.cmd[1] = p->fc;
			/*hUInt16 tmp_addr = pYk->no;
			p->addrH = HIBYTE(tmp_addr);
			p->addrL = LOBYTE(tmp_addr);*/
			m_current_send_cmd.cmd[2] = p->addrH;
			m_current_send_cmd.cmd[3] = p->addrL;
			switch( p->fc2 )
			{
			case	0x10:
				if( pYk->ctrlState == YK_ON )
				{
					m_current_send_cmd.cmd[4] = p->lengthH;
					m_current_send_cmd.cmd[5] = 0x00;/*p->lengthL*/; 
				}
				else
				{
					m_current_send_cmd.cmd[4] = p->lengthL;
					m_current_send_cmd.cmd[5] = 0x00;/*p->lengthL*/; 
				}
				break;
			case	0x11:
				if( pYk->ctrlState == YK_ON )
				{
					m_current_send_cmd.cmd[4] = p->lengthH;
					m_current_send_cmd.cmd[5] = p->lengthH;;/*p->lengthL*/; 
				}
				else
				{
					m_current_send_cmd.cmd[4] = p->lengthL;
					m_current_send_cmd.cmd[5] = p->lengthL;/*p->lengthL*/; 
				}
				break;
			case	0x00:
			default:
				if( pYk->ctrlState == YK_ON )
				{
					m_current_send_cmd.cmd[4] = 0x00;
					m_current_send_cmd.cmd[5] = p->lengthH;;/*p->lengthL*/; 
				}
			//	else if( pYk->ctrlIntValue == 2 )
			//	{
			//		m_current_send_cmd.cmd[4] = HIBYTE((int)pYk->ctrlFloatValue

			//	}
				else
				{
					m_current_send_cmd.cmd[4] = 0x00;
					m_current_send_cmd.cmd[5] = p->lengthL;/*p->lengthL*/; 
				}
				break;
			}


			hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
			m_current_send_cmd.cmd[6] = HIBYTE(crc);
			m_current_send_cmd.cmd[7] = LOBYTE(crc);
			m_current_send_cmd.length = 8;

			//p->ykptr = pyk;
			memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
			p->ykptr.ctrlNo = pYk->ctrlNo;
			p->ykptr.ctrlType = pYk->ctrlType;
			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;
			ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);

			return true;
		}
	}

	return false;
}

bool CProto_Modbus_101::make_select_parameter(void* pyk)
{
	if( pyk == NULL ) return false;

	ctrl_pro_setpointex* pSet = (ctrl_pro_setpointex*)pyk;

	//根据规约特征表中的配置寻找该帧的下一帧该发送的报文
	PMC_Feature_ptr p = findFeatureSetValue(pSet->ctrlNo,1);
	if(p == NULL) p = findFeatureSetValue(pSet->ctrlNo);
	if( p != NULL ) 
	{
		if(p->fc == 0x10)
		{
#ifdef	SUPPORT_MUTL_ROUTE
			//m_current_send_cmd.cmd[0] = p->routeNo;
			DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
			m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
			m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
			m_current_send_cmd.cmd[1] = p->fc;
			m_current_send_cmd.cmd[2] = p->addrH;
			m_current_send_cmd.cmd[3] = p->addrL;
			//m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
			//m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;

			hInt16 tmp_16 = pSet->floatValue;
			hInt32 tmp_32 = pSet->floatValue;
			hInt8  tmp_8 = pSet->floatValue;

			switch(p->fc2)
			{
				case	0x20:
				case	0x21:
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[7] = HIBYTE(tmp_16);
					m_current_send_cmd.cmd[8] = LOBYTE(tmp_16);
					m_current_send_cmd.length = 11;
					break;
				case	0x40:
				case	0x41:
					//hInt32 tmp_32 = pYk->ctrlFloatValue;
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x04;/*p->lengthL*/;
					m_current_send_cmd.cmd[7] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[8] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[9] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[10] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 13;
					break;
				case	0x42:
					tmp_32 = *(int*)&(pSet->floatValue);
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x04;/*p->lengthL*/;
					m_current_send_cmd.cmd[7] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[8] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[9] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[10] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 13;
					break;
				case	0x10:
				case	0xA0:
				case	0x11:
				case	0xA1:
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x01;/*p->lengthL*/;
					m_current_send_cmd.cmd[7] = tmp_8;
					m_current_send_cmd.length = 10;
					break;
				case	0xB0:				
				case	0xB1:
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[8] = HIBYTE(tmp_16);
					m_current_send_cmd.cmd[7] = LOBYTE(tmp_16);
					m_current_send_cmd.length = 11;
					break;
				case	0xD0:				
				case	0xD1:
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x04;/*p->lengthL*/;
					m_current_send_cmd.cmd[10] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[9] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[8] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[7] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 13;
				case	0xD2:
					tmp_32 = *(int*)&(pSet->floatValue);
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x04;/*p->lengthL*/;
					m_current_send_cmd.cmd[10] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[9] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[8] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[7] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 13;
					break;
				case	0xD3:
					tmp_32 = *(int*)&(pSet->floatValue);
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x04;/*p->lengthL*/;
					m_current_send_cmd.cmd[9] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[10] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[7] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[8] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 13;
					break;
				default:
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[7] = HIBYTE(tmp_16);
					m_current_send_cmd.cmd[8] = LOBYTE(tmp_16);
					m_current_send_cmd.length = 11;
					break;
			}

			hUInt16 crc;

			switch(p->checkFunc)
			{
			case CRC16:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, m_current_send_cmd.length - 2);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 2] = HIBYTE(crc);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 1] = LOBYTE(crc);
				//m_current_send_cmd.length = 8;
				break;
			case CDT:
				//break;
			case SUM:
				crc = CToolFunctions::checkSum(m_current_send_cmd.cmd, 0, m_current_send_cmd.length - 2);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 2] = HIBYTE(crc);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 1] = LOBYTE(crc);
				//m_current_send_cmd.length = 8;
				break;
			default:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, m_current_send_cmd.length - 2);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 2] = HIBYTE(crc);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 1] = LOBYTE(crc);
				//m_current_send_cmd.length = 8;
				break;
			}

			ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
			
			p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
			//p->routeNo = pYk->grpno;
			p->ykptr = *(ctrl_pro_yk*)pyk;
			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;

			return true;
		}
		else if (p->fc == 0x05)
		{
#ifdef	SUPPORT_MUTL_ROUTE
			DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
			m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
			m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
			m_current_send_cmd.cmd[1] = p->fc;
			m_current_send_cmd.cmd[2] = p->addrH;
			m_current_send_cmd.cmd[3] = p->addrL;
			m_current_send_cmd.cmd[4] = p->lengthH;
			m_current_send_cmd.cmd[5] = p->lengthL;

			hUInt16 crc;
			switch(p->checkFunc)
			{
			case CRC16:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
				break;
			case CDT:
				//break;
			case SUM:
				crc = CToolFunctions::checkSum(m_current_send_cmd.cmd, 0, 6);
				break;
			default:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
				break;
			}

			m_current_send_cmd.cmd[6] = HIBYTE(crc);
			m_current_send_cmd.cmd[7] = LOBYTE(crc);
			m_current_send_cmd.length = 8;


			ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
			p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
			memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
			p->ykptr.ctrlNo = pSet->ctrlNo;
			p->ykptr.ctrlType = pSet->ctrlType;
			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;
			return true;
		}
		else if (p->fc == 0x16)
		{
#ifdef	SUPPORT_MUTL_ROUTE
			DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
			m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
			m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
			m_current_send_cmd.cmd[1] = p->fc;
			m_current_send_cmd.cmd[2] = p->addrH;
			m_current_send_cmd.cmd[3] = p->addrL;
			m_current_send_cmd.cmd[4] = p->lengthH;
			m_current_send_cmd.cmd[5] = p->lengthL;

			hUInt16 crc;
			switch(p->checkFunc)
			{
			case CRC16:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
				break;
			case CDT:
				//break;
			case SUM:
				crc = CToolFunctions::checkSum(m_current_send_cmd.cmd, 0, 6);
				break;
			default:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
				break;
			}

			m_current_send_cmd.cmd[6] = HIBYTE(crc);
			m_current_send_cmd.cmd[7] = LOBYTE(crc);
			m_current_send_cmd.length = 8;

			ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
			p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
			memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
			p->ykptr.ctrlNo = pSet->ctrlNo;
			p->ykptr.ctrlType = pSet->ctrlType;
			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;
			return true;
		}
		else
		{
#ifdef	SUPPORT_MUTL_ROUTE
			//m_current_send_cmd.cmd[0] = p->routeNo;
			DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
			m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
			m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
			m_current_send_cmd.cmd[1] = p->fc;
			m_current_send_cmd.cmd[2] = p->addrH;
			m_current_send_cmd.cmd[3] = p->addrL;

			hInt16 tmp_16 = pSet->floatValue;
			hInt32 tmp_32 = pSet->floatValue;
			hInt8  tmp_8 = pSet->floatValue;

			switch(p->fc2)
			{
				case	0x20:
				case	0x21:
					m_current_send_cmd.cmd[4] = HIBYTE(tmp_16);
					m_current_send_cmd.cmd[5] = LOBYTE(tmp_16);
					m_current_send_cmd.length = 8;
					break;
				case	0x40:
				case	0x41:
					m_current_send_cmd.cmd[4] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[5] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[6] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[7] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 10;
					break;
				case	0x42:
					tmp_32 = *(int*)&(pSet->floatValue);
					m_current_send_cmd.cmd[4] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[5] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[6] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[7] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 10;
					break;
				case	0x10:
				case	0xA0:
				case	0x11:
				case	0xA1:
					m_current_send_cmd.cmd[4] = tmp_8;
					m_current_send_cmd.length = 7;
					break;
				case	0xB0:				
				case	0xB1:
					m_current_send_cmd.cmd[4] = HIBYTE(tmp_16);
					m_current_send_cmd.cmd[5] = LOBYTE(tmp_16);
					m_current_send_cmd.length = 8;
					break;
				case	0xD0:				
				case	0xD1:
					m_current_send_cmd.cmd[7] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[6] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[5] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[4] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 10;
				case	0xD2:
					tmp_32 = *(int*)&(pSet->floatValue);
					m_current_send_cmd.cmd[7] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[6] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[5] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[4] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 10;
					break;
				case	0xD3:
					tmp_32 = *(int*)&(pSet->floatValue);
					m_current_send_cmd.cmd[6] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[7] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[4] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[5] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 10;
					break;
				default:
					m_current_send_cmd.cmd[4] = HIBYTE(tmp_16);
					m_current_send_cmd.cmd[5] = LOBYTE(tmp_16);
					m_current_send_cmd.length = 8;
					break;
			}

			hUInt16 crc;

			switch(p->checkFunc)
			{
			case CRC16:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, m_current_send_cmd.length - 2);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 2] = HIBYTE(crc);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 1] = LOBYTE(crc);
				//m_current_send_cmd.length = 8;
				break;
			case CDT:
				//break;
			case SUM:
				crc = CToolFunctions::checkSum(m_current_send_cmd.cmd, 0, m_current_send_cmd.length - 2);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 2] = HIBYTE(crc);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 1] = LOBYTE(crc);
				//m_current_send_cmd.length = 8;
				break;
			default:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, m_current_send_cmd.length - 2);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 2] = HIBYTE(crc);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 1] = LOBYTE(crc);
				//m_current_send_cmd.length = 8;
				break;
			}

			ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
			
			p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
			memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
			p->ykptr.ctrlNo = pSet->ctrlNo;
			p->ykptr.ctrlType = pSet->ctrlType;
			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;

			return true;
		}
	}

	return false;
}


bool CProto_Modbus_101::make_set_parameter(void* pyk)
{
	if( pyk == NULL ) return false;

	ctrl_pro_setpointex* pSet = (ctrl_pro_setpointex*)pyk;

	//根据规约特征表中的配置寻找该帧的下一帧该发送的报文
	PMC_Feature_ptr p = findFeatureSetValue(pSet->ctrlNo,2);
	if(p == NULL) p = findFeatureSetValue(pSet->ctrlNo);
	if( p != NULL ) 
	{
		if(p->fc == 0x10)
		{
#ifdef	SUPPORT_MUTL_ROUTE
			//m_current_send_cmd.cmd[0] = p->routeNo;
			DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
			m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
			m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
			m_current_send_cmd.cmd[1] = p->fc;
			m_current_send_cmd.cmd[2] = p->addrH;
			m_current_send_cmd.cmd[3] = p->addrL;
			//m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
			//m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;
			switch(p->lengthH)
			{
			case 0x01:
				pSet->floatValue = pSet->floatValue * 0.1;
				break;
			case 0x02:
				pSet->floatValue = pSet->floatValue * 0.01;
				break;
			case 0x03:
				pSet->floatValue = pSet->floatValue * 0.001;
				break;
			case 0x10:
				pSet->floatValue = pSet->floatValue * 10;
				break;
			case 0x20:
				pSet->floatValue = pSet->floatValue * 100;
				break;
			case 0x30:
				pSet->floatValue = pSet->floatValue * 1000;
				break;
			default:
				break;
			}

			hInt16 tmp_16 = pSet->floatValue;
			hInt32 tmp_32 = pSet->floatValue;
			hInt8  tmp_8 = pSet->floatValue;

			switch(p->fc2)
			{
				case	0x20:
				case	0x21:
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[7] = HIBYTE(tmp_16);
					m_current_send_cmd.cmd[8] = LOBYTE(tmp_16);
					m_current_send_cmd.length = 11;
					break;
				case	0x40:
				case	0x41:
					//hInt32 tmp_32 = pYk->ctrlFloatValue;
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x04;/*p->lengthL*/;
					m_current_send_cmd.cmd[7] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[8] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[9] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[10] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 13;
					break;
				case	0x42:
					tmp_32 = *(int*)&(pSet->floatValue);
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x04;/*p->lengthL*/;
					m_current_send_cmd.cmd[7] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[8] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[9] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[10] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 13;
					break;
				case	0x10:
				case	0xA0:
				case	0x11:
				case	0xA1:
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x01;/*p->lengthL*/;
					m_current_send_cmd.cmd[7] = tmp_8;
					m_current_send_cmd.length = 10;
					break;
				case	0xB0:				
				case	0xB1:
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[8] = HIBYTE(tmp_16);
					m_current_send_cmd.cmd[7] = LOBYTE(tmp_16);
					m_current_send_cmd.length = 11;
					break;
				case	0xD0:				
				case	0xD1:
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x04;/*p->lengthL*/;
					m_current_send_cmd.cmd[10] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[9] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[8] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[7] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 13;
				case	0xD2:
					tmp_32 = *(int*)&(pSet->floatValue);
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x04;/*p->lengthL*/;
					m_current_send_cmd.cmd[10] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[9] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[8] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[7] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 13;
					break;
				case	0xD3:
					tmp_32 = *(int*)&(pSet->floatValue);
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x04;/*p->lengthL*/;
					m_current_send_cmd.cmd[9] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[10] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[7] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[8] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 13;
					break;
				default:
					m_current_send_cmd.cmd[4] = 0x00;/*p->lengthH*/;
					m_current_send_cmd.cmd[5] = 0x01;/*p->lengthL*/;
					m_current_send_cmd.cmd[6] = 0x02;/*p->lengthL*/;
					m_current_send_cmd.cmd[7] = HIBYTE(tmp_16);
					m_current_send_cmd.cmd[8] = LOBYTE(tmp_16);
					m_current_send_cmd.length = 11;
					break;
			}

			hUInt16 crc;

			switch(p->checkFunc)
			{
			case CRC16:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, m_current_send_cmd.length - 2);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 2] = HIBYTE(crc);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 1] = LOBYTE(crc);
				//m_current_send_cmd.length = 8;
				break;
			case CDT:
				//break;
			case SUM:
				crc = CToolFunctions::checkSum(m_current_send_cmd.cmd, 0, m_current_send_cmd.length - 2);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 2] = HIBYTE(crc);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 1] = LOBYTE(crc);
				//m_current_send_cmd.length = 8;
				break;
			default:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, m_current_send_cmd.length - 2);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 2] = HIBYTE(crc);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 1] = LOBYTE(crc);
				//m_current_send_cmd.length = 8;
				break;
			}

			ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
			
			p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
			//p->routeNo = pYk->grpno;
			p->ykptr = *(ctrl_pro_yk*)pyk;
			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;

			return true;
		}
		else if (p->fc == 0x05)
		{
#ifdef	SUPPORT_MUTL_ROUTE
			DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
			m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
			m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
			m_current_send_cmd.cmd[1] = p->fc;
			m_current_send_cmd.cmd[2] = p->addrH;
			m_current_send_cmd.cmd[3] = p->addrL;
			m_current_send_cmd.cmd[4] = p->lengthH;
			m_current_send_cmd.cmd[5] = p->lengthL;

			hUInt16 crc;
			switch(p->checkFunc)
			{
			case CRC16:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
				break;
			case CDT:
				//break;
			case SUM:
				crc = CToolFunctions::checkSum(m_current_send_cmd.cmd, 0, 6);
				break;
			default:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
				break;
			}

			m_current_send_cmd.cmd[6] = HIBYTE(crc);
			m_current_send_cmd.cmd[7] = LOBYTE(crc);
			m_current_send_cmd.length = 8;

			ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
			p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
			memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
			p->ykptr.ctrlNo = pSet->ctrlNo;
			p->ykptr.ctrlType = pSet->ctrlType;
			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;
			return true;
		}
		else if (p->fc == 0x16)
		{
#ifdef	SUPPORT_MUTL_ROUTE
			DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
			m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
			m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
			m_current_send_cmd.cmd[1] = p->fc;
			m_current_send_cmd.cmd[2] = p->addrH;
			m_current_send_cmd.cmd[3] = p->addrL;
			switch(p->lengthH)
			{
			case 0x01:
				pSet->floatValue = pSet->floatValue * 0.1;
				break;
			case 0x02:
				pSet->floatValue = pSet->floatValue * 0.01;
				break;
			case 0x03:
				pSet->floatValue = pSet->floatValue * 0.001;
				break;
			case 0x10:
				pSet->floatValue = pSet->floatValue * 10;
				break;
			case 0x20:
				pSet->floatValue = pSet->floatValue * 100;
				break;
			case 0x30:
				pSet->floatValue = pSet->floatValue * 1000;
				break;
			default:
				break;
			}

			hInt16 tmp_16 = pSet->floatValue;
			//hInt32 tmp_32 = pSet->floatValue;
			//hInt8  tmp_8 = pSet->floatValue;
			m_current_send_cmd.cmd[4] = HIBYTE(tmp_16);
			m_current_send_cmd.cmd[5] =LOBYTE(tmp_16);
			
			hUInt16 crc;
			switch(p->checkFunc)
			{
			case CRC16:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
				break;
			case CDT:
				//break;
			case SUM:
				crc = CToolFunctions::checkSum(m_current_send_cmd.cmd, 0, 6);
				break;
			default:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
				break;
			}

			m_current_send_cmd.cmd[6] = HIBYTE(crc);
			m_current_send_cmd.cmd[7] = LOBYTE(crc);
			m_current_send_cmd.length = 8;

			ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
			p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
			memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
			p->ykptr.ctrlNo = pSet->ctrlNo;
			p->ykptr.ctrlType = pSet->ctrlType;
			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;
			return true;
		}
		//else if (p->fc == F3)
		//{
		//	int nLength = 0;
		//	m_current_send_cmd.cmd[0] = p->fc;
		//	m_current_send_cmd.cmd[1] = p->addrH;
		//	m_current_send_cmd.cmd[2] = p->addrL;
		//	m_current_send_cmd.cmd[3] = p->fc2;
		//	// 传送原因
		//	m_current_send_cmd.cmd[4] = 0x0D;
		//	m_current_send_cmd.cmd[5] = 0x00;
		//	// 公共地址
		//	m_current_send_cmd.cmd[6] = 0x01;
		//	m_current_send_cmd.cmd[7] = 0x00;
		//	// 信息体地址
		//	m_current_send_cmd.cmd[8] = 0x02;
		//	m_current_send_cmd.cmd[9] = 0x69;
		//	// 文件名
		//	m_current_send_cmd.cmd[10] = 0x00;
		//	m_current_send_cmd.cmd[11] = 0x00;
		//	// 节名
		//	m_current_send_cmd.cmd[12] = 0x00;
		//	
		//	if(p->fc2 == 0x7A)	//读
		//	{
		//		// SCQ(选择和调用限定词)
		//		m_current_send_cmd.cmd[13] = 0x00;
		//	}
		//	else if (p->fc2 ==0x7D)	//写
		//	{
		//		// 节长度
		//		m_current_send_cmd.cmd[13] = 0x07;
		//		//数据1 ··· 数据n
		//		m_current_send_cmd.cmd[14] = 0x07;
		//		m_current_send_cmd.cmd[15] = 0x07;
		//		m_current_send_cmd.cmd[16] = 0x07;
		//		m_current_send_cmd.cmd[17] = 0x07;
		//		m_current_send_cmd.cmd[18] = 0x07;
		//		m_current_send_cmd.cmd[19] = 0x07;
		//		m_current_send_cmd.cmd[20] = 0x07;
		//	}
		//	
		//	
		//	m_current_send_cmd.cmd[14] = 0x01;
		//	m_current_send_cmd.cmd[7] = 0x00;
		//	
		//	hInt16 tmp_16 = pSet->floatValue;
		//	//hInt32 tmp_32 = pSet->floatValue;
		//	//hInt8  tmp_8 = pSet->floatValue;
		//	m_current_send_cmd.cmd[4] = HIBYTE(tmp_16);
		//	m_current_send_cmd.cmd[5] =LOBYTE(tmp_16);

		//	hUInt16 crc;
		//	switch(p->checkFunc)
		//	{
		//	case CRC16:
		//		crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
		//		break;
		//	case CDT:
		//		//break;
		//	case SUM:
		//		crc = CToolFunctions::checkSum(m_current_send_cmd.cmd, 0, 6);
		//		break;
		//	default:
		//		crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
		//		break;
		//	}

		//	m_current_send_cmd.cmd[6] = HIBYTE(crc);
		//	m_current_send_cmd.cmd[7] = LOBYTE(crc);
		//	m_current_send_cmd.length = 8;

		//	ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
		//	p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
		//	memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
		//	p->ykptr.ctrlNo = pSet->ctrlNo;
		//	p->ykptr.ctrlType = pSet->ctrlType;
		//	m_current_send_cmd.currFeatureptr = p;
		//	m_currPollCmdno = p->cmdNo;
		//	return true;
		//}
		else
		{
			#ifdef	SUPPORT_MUTL_ROUTE
			//m_current_send_cmd.cmd[0] = p->routeNo;
			DAC_ROUTE	*pRoute = m_commInf.route(p->routeNo);
			m_current_send_cmd.cmd[0] = pRoute->remoteAddr;
#else
			m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
			m_current_send_cmd.cmd[1] = p->fc;
			m_current_send_cmd.cmd[2] = p->addrH;
			m_current_send_cmd.cmd[3] = p->addrL;

			switch(p->lengthH)
			{
			case 0x01:
				pSet->floatValue = pSet->floatValue * 0.1;
				break;
			case 0x02:
				pSet->floatValue = pSet->floatValue * 0.01;
				break;
			case 0x03:
				pSet->floatValue = pSet->floatValue * 0.001;
				break;
			case 0x10:
				pSet->floatValue = pSet->floatValue * 10;
				break;
			case 0x20:
				pSet->floatValue = pSet->floatValue * 100;
				break;
			case 0x30:
				pSet->floatValue = pSet->floatValue * 1000;
				break;
			default:
				break;
			}

			hInt16 tmp_16 = pSet->floatValue;
			hInt32 tmp_32 = pSet->floatValue;
			hInt8  tmp_8 = pSet->floatValue;

			switch(p->fc2)
			{
				case	0x20:
				case	0x21:
					m_current_send_cmd.cmd[4] = HIBYTE(tmp_16);
					m_current_send_cmd.cmd[5] = LOBYTE(tmp_16);
					m_current_send_cmd.length = 8;
					break;
				case	0x40:
				case	0x41:
					m_current_send_cmd.cmd[4] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[5] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[6] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[7] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 10;
					break;
				case	0x42:
					tmp_32 = *(int*)&(pSet->floatValue);
					m_current_send_cmd.cmd[4] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[5] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[6] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[7] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 10;
					break;
				case	0x10:
				case	0xA0:
				case	0x11:
				case	0xA1:
					m_current_send_cmd.cmd[4] = tmp_8;
					m_current_send_cmd.length = 7;
					break;
				case	0xB0:				
				case	0xB1:
					m_current_send_cmd.cmd[4] = HIBYTE(tmp_16);
					m_current_send_cmd.cmd[5] = LOBYTE(tmp_16);
					m_current_send_cmd.length = 8;
					break;
				case	0xD0:				
				case	0xD1:
					m_current_send_cmd.cmd[7] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[6] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[5] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[4] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 10;
				case	0xD2:
					tmp_32 = *(int*)&(pSet->floatValue);
					m_current_send_cmd.cmd[7] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[6] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[5] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[4] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 10;
					break;
				case	0xD3:
					tmp_32 = *(int*)&(pSet->floatValue);
					m_current_send_cmd.cmd[6] = HIBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[7] = LOBYTE(HIWORD(tmp_32));
					m_current_send_cmd.cmd[4] = HIBYTE(LOWORD(tmp_32));
					m_current_send_cmd.cmd[5] = LOBYTE(LOWORD(tmp_32));
					m_current_send_cmd.length = 10;
					break;
				default:
					m_current_send_cmd.cmd[4] = HIBYTE(tmp_16);
					m_current_send_cmd.cmd[5] = LOBYTE(tmp_16);
					m_current_send_cmd.length = 8;
					break;
			}

			hUInt16 crc;

			switch(p->checkFunc)
			{
			case CRC16:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, m_current_send_cmd.length - 2);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 2] = HIBYTE(crc);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 1] = LOBYTE(crc);
				//m_current_send_cmd.length = 8;
				break;
			case CDT:
				//break;
			case SUM:
				crc = CToolFunctions::checkSum(m_current_send_cmd.cmd, 0, m_current_send_cmd.length - 2);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 2] = HIBYTE(crc);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 1] = LOBYTE(crc);
				//m_current_send_cmd.length = 8;
				break;
			default:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, m_current_send_cmd.length - 2);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 2] = HIBYTE(crc);
				m_current_send_cmd.cmd[m_current_send_cmd.length - 1] = LOBYTE(crc);
				//m_current_send_cmd.length = 8;
				break;
			}

			ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
			
			p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
			//p->routeNo = pYk->grpno;
			memcpy(&p->ykptr , pyk,sizeof(ctrl_pro_yk));
			p->ykptr.ctrlNo = pSet->ctrlNo;
			p->ykptr.ctrlType = pSet->ctrlType;
			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;

			return true;
		}
	}

	return false;
}



bool PDT::CProto_Modbus_101::make_constantValue_read( ctrl_pro_constantvalue* pCom )
{
	if( pCom == NULL ) return false;
	//PMC_Feature_ptr p = findFeatureConstantValue(CONSTANTVALUE_READ);
	//if(p == NULL) return false;


	//根据规约特征表中的配置寻找该帧的下一帧该发送的报文
	int nLength = 0;
	m_current_send_cmd.cmd[nLength++] = 0x71;	//
	m_current_send_cmd.cmd[nLength++] = 0x00;	//
	m_current_send_cmd.cmd[nLength++] = 0x00;	//
	m_current_send_cmd.cmd[nLength++] = 0x71;	//

	m_current_send_cmd.cmd[nLength++] = 0xF3;	//
	m_current_send_cmd.cmd[nLength++] = 0x01;	// 子站站址低位
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 子站站址高位
	m_current_send_cmd.cmd[nLength++] = 0x7A;			// 类型标识
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 结构限定词
	m_current_send_cmd.cmd[nLength++] = 0x0D;			// 传送原因
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 
	m_current_send_cmd.cmd[nLength++] = 0x01;			// 公共地址
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 
	m_current_send_cmd.cmd[nLength++] = 0x02;			// 信息体地址
	m_current_send_cmd.cmd[nLength++] = 0x69;			// 
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 文件名
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 节名
	m_current_send_cmd.cmd[nLength++] = 0x00;			// SCQ

	m_current_send_cmd.cmd[1] = nLength -4;
	m_current_send_cmd.cmd[2] = nLength -4;

	hUInt16 crc = CToolFunctions::checkSum(m_current_send_cmd.cmd, 4, nLength);
	m_current_send_cmd.cmd[nLength++] = crc;
	m_current_send_cmd.cmd[nLength++] = 0x16;
	m_current_send_cmd.length = nLength;
	
	p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
	m_current_send_cmd.currFeatureptr = p;
	m_currPollCmdno = p->cmdNo;

	ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);

	return true;
}

bool PDT::CProto_Modbus_101::make_constantValue_write( ctrl_pro_constantvalue* pCom )
{
	if( pCom == NULL ) return false;
	//PMC_Feature_ptr p = findFeatureConstantValue(CONSTANTVALUE_WRITE);
	//if(p == NULL) return false;

	//根据规约特征表中的配置寻找该帧的下一帧该发送的报文
	int nLength = 0;
	m_current_send_cmd.cmd[nLength++] = 0x71;	//
	m_current_send_cmd.cmd[nLength++] = 0x00;	//
	m_current_send_cmd.cmd[nLength++] = 0x00;	//
	m_current_send_cmd.cmd[nLength++] = 0x71;	//

	m_current_send_cmd.cmd[nLength++] = 0xF3;	//
	m_current_send_cmd.cmd[nLength++] = 0x01;	// 子站站址低位
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 子站站址高位
	m_current_send_cmd.cmd[nLength++] = 0x7D;			// 类型标识
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 结构限定词
	m_current_send_cmd.cmd[nLength++] = 0x0D;			// 传送原因
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 
	m_current_send_cmd.cmd[nLength++] = 0x01;			// 公共地址
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 
	m_current_send_cmd.cmd[nLength++] = p->addrL;			// 信息体地址
	m_current_send_cmd.cmd[nLength++] = p->addrH;			// 
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 文件名
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 
	m_current_send_cmd.cmd[nLength++] = 0x00;			// 节名

	//char str[] = "07;00;0A;1A;01;00;00;00";
	char *buf = pCom->cmdBuf;
 
	char *token = strtok(buf,";");
	QString qstr;
	bool bOk;
	while(token != NULL)
	{
		qstr = QString(token);
		m_current_send_cmd.cmd[nLength++] = qstr.toInt(&bOk, 16);
		token = strtok(NULL, ";");
	}

	m_current_send_cmd.cmd[1] = nLength -4;
	m_current_send_cmd.cmd[2] = nLength -4;

	hUInt16 crc = CToolFunctions::checkSum(m_current_send_cmd.cmd, 4, nLength);
	m_current_send_cmd.cmd[nLength++] = crc;
	m_current_send_cmd.cmd[nLength++] = 0x16;
	m_current_send_cmd.length = nLength;

	p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
	m_current_send_cmd.currFeatureptr = p;
	m_currPollCmdno = p->cmdNo;

	ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);

	return true;
}

bool CProto_Modbus_101::sendTime()
{
	//根据规约特征表中的配置寻找该帧的下一帧该发送的报文
	PMC_Feature_ptr p = findFeatureSyncTime();
	if( p != NULL ) 
	{
		if(p->fc == 0x7F)
		{	
			int i = 0;
#ifdef	SUPPORT_MUTL_ROUTE
			DAC_ROUTE	*m_proute = m_commInf.route(p->routeNo);
			if (!m_proute)
			{
				return false;
			}
			m_current_send_cmd.cmd[i++] = m_proute->remoteAddr;
#else
			m_current_send_cmd.cmd[i++] = m_remoteAddr;
#endif

			m_current_send_cmd.cmd[i++] = p->fc;
			m_current_send_cmd.cmd[i++] = p->addrH;
			m_current_send_cmd.cmd[i++] = p->addrL;
			m_current_send_cmd.cmd[i++] = 0x00;
			m_current_send_cmd.cmd[i++] = 0x07;
			m_current_send_cmd.cmd[i++] = 0x0E;

			CDateTime dateTime = CDateTime::currentDateTime();
			m_current_send_cmd.cmd[i++] = HiByte(dateTime.year());
			m_current_send_cmd.cmd[i++] = LoByte(dateTime.year());
			m_current_send_cmd.cmd[i++] = HiByte(dateTime.month());
			m_current_send_cmd.cmd[i++] = LoByte(dateTime.month());
			m_current_send_cmd.cmd[i++] = HiByte(dateTime.day());
			m_current_send_cmd.cmd[i++] = LoByte(dateTime.day());
			m_current_send_cmd.cmd[i++] = HiByte(dateTime.hour());
			m_current_send_cmd.cmd[i++] = LoByte(dateTime.hour());
			m_current_send_cmd.cmd[i++] = HiByte(dateTime.minute());
			m_current_send_cmd.cmd[i++] = LoByte(dateTime.minute());
			m_current_send_cmd.cmd[i++] = HiByte(dateTime.second());
			m_current_send_cmd.cmd[i++] = LoByte(dateTime.second());
			m_current_send_cmd.cmd[i++] = HiByte(dateTime.microsec());
			m_current_send_cmd.cmd[i++] = LoByte(dateTime.microsec());

			hUInt16 crc;

			switch(p->checkFunc)
			{
			case CRC16:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, i);
				break;
			case CDT:
				//break;
			case SUM:
				crc = CToolFunctions::checkSum(m_current_send_cmd.cmd, 0, i);
				break;
			default:
				crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, i);
				break;
			}

			m_current_send_cmd.cmd[i++] = HIBYTE(crc);
			m_current_send_cmd.cmd[i++] = LOBYTE(crc);
			m_current_send_cmd.length = i;



			ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
			
			p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;			
			m_current_send_cmd.currFeatureptr = p;
			m_currPollCmdno = p->cmdNo;

			return true;
		}
	}

	return false;
}

bool CProto_Modbus_101::hasAnyOnTimerCommand()
{
	//规约建立以后，第一帧不发送定时命令
	if( m_firstSend ) return false;

#ifdef	SUPPORT_MUTL_ROUTE
	PMC_Feature_ptr         p = m_current_send_cmd.currFeatureptr;
	if(!p) return false;
	DAC_ROUTE_INFO*			pRouteInfo = m_commInf.routeInfo(p->routeNo);
#else
	DAC_ROUTE_INFO*			pRouteInfo = m_commInf.routeInfo(m_route);
#endif

	if (pRouteInfo && pRouteInfo->cmd.timesync == TRUE )
	{
		pRouteInfo->cmd.timesync = FALSE;
		
		return sendTime();
	}
	
	return false;
}

void CProto_Modbus_101::makeCommand( PMC_Feature_ptr p )
{
	if( p == NULL ) return ;

	if(p->fc == 0x10 || p->fc == 0x0f ) return;
	if(p->frmType  > 1) return;

	//modify by wlq 20110730
	//处理一个规约挂接多个路径问题
#ifdef	SUPPORT_MUTL_ROUTE
	DAC_ROUTE	*m_proute = m_commInf.route(p->routeNo);
	if (!m_proute)
	{
		return;
	}
	m_current_send_cmd.cmd[0] = m_proute->remoteAddr;
#else
	m_current_send_cmd.cmd[0] = m_remoteAddr;
#endif
	m_current_send_cmd.cmd[1] = p->fc;
	m_current_send_cmd.cmd[2] = p->addrH;
	m_current_send_cmd.cmd[3] = p->addrL;
	m_current_send_cmd.cmd[4] = p->lengthH;
	m_current_send_cmd.cmd[5] = p->lengthL;

	//modify by wlq 20110722
	//处理03命令问遥信，长度根据遥信个数要调整,个数/16
	if( p->fc == 0x03 && p->frmType == Discrete )
	{
		hUInt16 my_len = p->lengthH * 256 + p->lengthL;
		my_len = (my_len%16) == 0 ? my_len/16 : my_len/16 + 1 ;
		m_current_send_cmd.cmd[4] = HIBYTE(my_len);
		m_current_send_cmd.cmd[5] = LOBYTE(my_len);
	}

	hUInt16 crc;

	switch(p->checkFunc)
	{
	case CRC16:
		crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
		m_current_send_cmd.cmd[6] = HIBYTE(crc);
		m_current_send_cmd.cmd[7] = LOBYTE(crc);
		m_current_send_cmd.length = 8;
		break;
	case CDT:
		//break;
	case SUM:
		crc = CToolFunctions::checkSum(m_current_send_cmd.cmd, 0, 6);
		m_current_send_cmd.cmd[6] = HIBYTE(crc);
		m_current_send_cmd.cmd[7] = LOBYTE(crc);
		m_current_send_cmd.length = 8;
		break;
	default:
		crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
		m_current_send_cmd.cmd[6] = HIBYTE(crc);
		m_current_send_cmd.cmd[7] = LOBYTE(crc);
		m_current_send_cmd.length = 8;
		break;
	}

	ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
}

void CProto_Modbus_101::setCmdinfoToBuf()
{
	//发送桢数+1
	addSendFrmNum();

	//把命令put到共享内存
	//hUChar buf[PMC_SEND_BUFFER_SIZE];
	//msgDisplay(FRAME_TX, m_current_send_cmd.cmdStr, m_current_send_cmd.cmd, m_current_send_cmd.length);

#ifdef  SUPPORT_MODBUS_TCP
	hUInt16	length = (m_current_send_cmd.length - 2 > 0) ? (m_current_send_cmd.length - 2) : 0;
	memcpy(m_current_send_cmd.cmd + 6,m_current_send_cmd.cmd,length);
	m_current_send_cmd.cmd[0] = 0x00;
	m_current_send_cmd.cmd[1] = 0x00;
	m_current_send_cmd.cmd[2] = 0x00;
	m_current_send_cmd.cmd[3] = 0x00;
	m_current_send_cmd.cmd[4] = HIBYTE(length);
	m_current_send_cmd.cmd[5] = LOBYTE(length);
	m_current_send_cmd.length += 4;
#endif

	put(m_current_send_cmd.cmd, m_current_send_cmd.length);
	msgDisplay(FRAME_TX, m_current_send_cmd.cmdStr, m_current_send_cmd.cmd, m_current_send_cmd.length);
	//logprint(LOG_PROTOCOL + m_route, "发送：%s , 命令长度：%d",m_current_send_cmd.cmdStr, m_current_send_cmd.length);
	CDateTime tNow = CDateTime::currentDateTime();
	logprint(LOG_PROTOCOL+ m_route, "发送： -- %02d:%02d:%02d  %03d,  %s , 命令长度：%d",tNow.hour(), tNow.minute(), tNow.second(), tNow.microsec()/1000, m_current_send_cmd.cmdStr, m_current_send_cmd.length);


}


void CProto_Modbus_101::mainRx()
{
	hUChar buff[PMC_RECV_BUFFER_SIZE];
	hInt32 rcvdataleng = length();//取得接收字节数
	hUInt32 rcvleng;//实际获取的字节

	rcvdataleng = rcvdataleng > ( PMC_RECV_BUFFER_SIZE - m_recv_msg.length )  ? ( PMC_RECV_BUFFER_SIZE - m_recv_msg.length ) : rcvdataleng ; 

	rcvleng = get(buff, rcvdataleng);

	
	if (m_bTimeOut && m_isExclusive)
	{
		if (m_nSOENum <= 0 || m_record_feature.size() == 0)
		{
			m_isExclusive = false;
		}
	}


	if (m_bTimeOut && !m_isExclusive)
	{
		CDateTime tNow = CDateTime::currentDateTime();

		if (!m_bOk)
		{
			logprint(LOG_PROTOCOL+ m_route, "接收： -- %02d:%02d:%02d  %03d,  接收超时",tNow.hour(), tNow.minute(), tNow.second(), tNow.microsec()/1000);
			//addRecvErrFrmNum(); 接收超时不属于错误帧
			m_protocolEvent = TimeOut;
		}
		repotrAckRecv(true);//接收数据
		m_bTimeOut = false;
		logprint(LOG_PROTOCOL + m_route, "切换发送模式 ");

		return;
	}
	else if (m_isExclusive && m_protocolEvent == Waitting && m_timeSet.time_out(m_rtimeout_id))
	{
		
		CDateTime tNow = CDateTime::currentDateTime();
		logprint(LOG_PROTOCOL+ m_route, "接收： -- %02d:%02d:%02d  %03d,  接收超时",tNow.hour(), tNow.minute(), tNow.second(), tNow.microsec()/1000);

		if (!m_bOk)
		{
			//addRecvErrFrmNum();
			m_protocolEvent = TimeOut;
		}

		if (m_nSOENum == 0)
		{
			m_nSOENum = -1;
			m_isExclusive = false;	//下发命令超时，直接跳出继续轮询数据。
		}

		if (m_record_feature.size() == 0)
		{
			m_isExclusive = false;
		}

		//while(m_record_feature.size()>0)
		//{
		//	PMC_Feature*  pFeature  = m_record_feature.first(); 
		//	delete pFeature;
		//	pFeature = NULL;
		//}
		//m_record_feature.clear();

		repotrAckRecv(true);//接收数据
		return;
	}



	//if (m_bTimeOut)
	//{
	//	if (!m_isExclusive || (m_protocolEvent == Waitting && m_timeSet.time_out(m_rtimeout_id)))
	//	{
	//	//if(m_protocolEvent == Waitting)
	//	//{
	//		//if(m_timeSet.time_out(m_rtimeout_id))
	//	//	{
	//			m_isExclusive = false;	//下发命令超时，直接跳出继续轮询数据。
	//			CDateTime tNow = CDateTime::currentDateTime();
	//			logprint(LOG_PROTOCOL+ m_route, "接收： -- %02d:%02d:%02d  %03d,  接收超时",tNow.hour(), tNow.minute(), tNow.second(), tNow.microsec()/1000);

	//			if (!m_bOk)
	//			{
	//				addRecvErrFrmNum();
	//				m_protocolEvent = TimeOut;
	//			}
	//			repotrAckRecv(true);//接收数据
	//			//return;
	//	}
	//	m_bTimeOut = false;
	//	return;
	//}


	if (rcvleng == 0)
	{
		return;
	}

	int nRet = handleData(buff, rcvleng);
	if (nRet == RECEIVE_SUCCESS)
	{
		if(isSending() && m_protocolEvent != Waitting )
		{
			logprint(LOG_PROTOCOL + m_route, "接收正确：本次接收长度：%d ",rcvleng , m_recv_msg.length);
			addRecvOkFrmNum();
			m_bOk = true;
			//repotrAckRecv(true);//接收数据
		}
	}
	else if (nRet == RECEIVE_UNFINISHED)
	{
		CDateTime tNow = CDateTime::currentDateTime();
		logprint(LOG_PROTOCOL+ m_route, "接收： -- %02d:%02d:%02d  %03d,  接收正确但数据未收完：本次接收长度：%d",
			tNow.hour(), tNow.minute(), tNow.second(), tNow.microsec()/1000, rcvleng);

		//logprint(LOG_PROTOCOL + m_route, "接收正确但数据未收完：本次接收长度：%d ",rcvleng);
	}
	else 
	{
		if (nRet == RECEIVE_ERROR_LENGTH)
		{
			logprint(LOG_PROTOCOL + m_route, "接收错误：长度出错");
		}
		else if (nRet == RECEIVE_ERROR_CRC)
		{
			logprint(LOG_PROTOCOL + m_route, "接收错误：校验出错");
		}
		else if (nRet == RECEIVE_ERROR_FC)
		{
			logprint(LOG_PROTOCOL + m_route, "接收错误：功能码出错");	
		}
		else if (nRet == RECEIVE_ERROR_ROUTE)
		{
			logprint(LOG_PROTOCOL + m_route, "接收错误：路径出错");	
		}
		addRecvErrFrmNum();
		//repotrAckRecv(true);
		
	}
}

int CProto_Modbus_101::handleData( hUChar *buff, hUInt16 nLen )
{
	putData(buff, nLen);
	//updateErrRate();
	return  paraDevProtocol();
}

void CProto_Modbus_101::putData( hUChar *buff, hUInt16 leng )
{
	if( leng == 0 ) return;

	if(leng > PMC_RECV_BUFFER_SIZE - m_recv_msg.length)
	{
		logprint(LOG_PROTOCOL  + m_route, "***数据超过接收缓存的长度!清空数据接收区");
		memset(m_recv_msg.msg, 0, PMC_RECV_BUFFER_SIZE);
		memcpy(m_recv_msg.msg, buff, leng);
		m_recv_msg.length = leng;
	}
	else
	{
		memcpy(m_recv_msg.msg + m_recv_msg.length, buff, leng);
		m_recv_msg.length = m_recv_msg.length + leng;
	}
}



int CProto_Modbus_101::paraDevProtocol()
{
	//计算正确帧的实际长度
	hUInt32	frmLen;

	//接收长度小于5，肯定没有完整的报文头，等下次接收超过5长度再处理addr（1）+func（1）+LEN（1）+CRC（2）
	if (m_recv_msg.length < 3)
	{
		return RECEIVE_UNFINISHED;
	}


	//首先寻找报文头（判断首位是否地址码和长度）
	if (!findHeadAndAdjustMsg())
	{
		return RECEIVE_ERROR_ROUTE;
	}
		
	PMC_Feature_ptr pfeature = m_current_send_cmd.currFeatureptr;
	//首先判断是否错误帧
	bool bFlag = false;
	hUChar ch;

#ifdef SUPPORT_MODBUS_TCP
	if(m_recv_msg.msg[7] == (pfeature->fc + 0x80))
	{
		bFlag = true;
		ch = m_recv_msg.msg[8];
	}
#else
	if(m_recv_msg.msg[1] == (pfeature->fc + 0x80))
	{
		bFlag = true;
		ch = m_recv_msg.msg[2];
	}
#endif
	if(bFlag)		// 功能码为 0x80
	{
		switch(ch)
		{
		case 0x01:
			logprint(LOG_PROTOCOL + m_route,"异常功能码: <%d> 无效!",m_current_send_cmd.cmdFunc);
			break;
		case 0x02:
			logprint(LOG_PROTOCOL + m_route,"寄存器地址无效!");
			break;
		case 0x03:
			logprint(LOG_PROTOCOL + m_route,"数值无效!");
			break;
		default:
			logprint(LOG_PROTOCOL + m_route,"未知的异常");
			break;
		}

		if(pfeature->frmType == Control || pfeature->frmType == SetValue) reportCmdExecResult(pfeature, ch);

		m_protocolEvent = Succeeded;

		msgDisplay(FRAME_RX, pfeature->cmdDesc , m_recv_msg.msg, m_recv_msg.length);

#ifdef SUPPORT_MODBUS_TCP
		memcpy( m_recv_msg.msg, m_recv_msg.msg + 9 , m_recv_msg.length - 9 );
		m_recv_msg.length = m_recv_msg.length - 9;
#else
		memcpy( m_recv_msg.msg, m_recv_msg.msg + 3 , m_recv_msg.length - 3 );
		m_recv_msg.length = m_recv_msg.length - 3;
#endif				
		//addRecvOkFrmNum();
		return RECEIVE_SUCCESS;
	}


	//长度校验 校验码校验
#ifdef SUPPORT_MODBUS_TCP
	frmLen = MAKEWORD(m_recv_msg.msg[5],m_recv_msg.msg[4]) + 1;

	if (m_recv_msg.length < frmLen + 5 ) return RECEIVE_UNFINISHED;
	if (m_recv_msg.length > 8 && m_recv_msg.msg[8] != MAKEWORD(pfeature->lengthL, pfeature->lengthH) * 2)
	{
		memcpy( m_recv_msg.msg, m_recv_msg.msg + frmLen + 5 , m_recv_msg.length - frmLen - 5 );
		m_recv_msg.length = m_recv_msg.length - frmLen - 5;
		return RECEIVE_ERROR_LENGTH;
	}
#else
	//非错误帧，首先计算校验
	if (m_recv_msg.msg[0] == 0x71 && m_recv_msg.msg[3]==0x71)
	{
		frmLen = m_recv_msg.msg[1];
	}
	else if(m_recv_msg.msg[1] == 0x06 || m_recv_msg.msg[1] == 0x05 || m_recv_msg.msg[1] == 0x0F || m_recv_msg.msg[1] == 0x10 
		|| m_recv_msg.msg[1] == 0x16 || m_recv_msg.msg[1] == 0x7F) 
	{
		frmLen = 3;
	}
	else if (pfeature->frmType == RecordsData_Record)
	{
		frmLen = m_recv_msg.msg[2];
		if (m_recv_msg.length < frmLen + 5 ) 
			return RECEIVE_UNFINISHED;
	}
	else 
	{
		frmLen = m_recv_msg.msg[2];
		if (m_recv_msg.length < frmLen + 5 ) 
			return RECEIVE_UNFINISHED;

		if ( /*pfeature->frmType == Analog &&*/ frmLen != MAKEWORD(pfeature->lengthL, pfeature->lengthH) * 2 )
		{
			//if (m_recv_msg.length -5 < frmLen)
			//{
			//	frmLen = m_recv_msg.length -5;
			//}
			//frmLen = m_recv_msg.length-5 <= frmLen ? m_recv_msg.length-5 : frmLen;
			//memcpy( m_recv_msg.msg, m_recv_msg.msg + frmLen + 5 , m_recv_msg.length - frmLen - 5 );
			//m_recv_msg.length = m_recv_msg.length - frmLen - 5;
		
			//memcpy( m_recv_msg.msg, m_recv_msg.msg + m_recv_msg.length , m_recv_msg.length);
			m_recv_msg.length = 0;
			return RECEIVE_ERROR_LENGTH;
		}
	}

	//长度完整以后，先计算校验
	if (m_recv_msg.msg[0] == 0x71 && m_recv_msg.msg[3]==0x71)
	{
		hUInt16 crc = CToolFunctions::checkSum(m_recv_msg.msg, 4, m_recv_msg.length - 2);
		if (LOBYTE(crc) != m_recv_msg.msg[frmLen+4])
		{
			logprint(LOG_PROTOCOL + m_route,"设备数据crc校验错误!");
			m_protocolEvent = CheckError;
			msgDisplay(FRAME_RX_ERROR, "设备数据crc校验错误", m_recv_msg.msg, m_recv_msg.length);
			m_recv_msg.length = 0;
			return RECEIVE_ERROR_CRC;
		}
	}
	else
	{
		hUInt16 crc = CToolFunctions::modbusCRC( m_recv_msg.msg, frmLen + 3 );

		if( HIBYTE(crc)!= m_recv_msg.msg[frmLen + 3] || LOBYTE(crc) != m_recv_msg.msg[frmLen + 4])
		{
			logprint(LOG_PROTOCOL + m_route,"设备数据crc校验错误!");
			m_protocolEvent = CheckError;

			msgDisplay(FRAME_RX_ERROR, "设备数据crc校验错误", m_recv_msg.msg, m_recv_msg.length);
			//addRecvErrFrmNum();

			//丢帧以后退出
			//memcpy(m_recv_msg.msg, m_recv_msg.msg + 1, m_recv_msg.length - 1);
			m_recv_msg.length = 0;
			return RECEIVE_ERROR_CRC;
		}
	}
#endif

	if (pfeature->frmType == ConstantValue)
	{
		//功能码验证
#ifdef SUPPORT_MODBUS_TCP
		hUChar  cfc = m_recv_msg.msg[13];
#else
		hUChar  cfc = m_recv_msg.msg[7];
#endif
		if( cfc != pfeature->fc )
		{
			logprint(LOG_PROTOCOL + m_route,"收到了非本次问询的结果，本次问询功能码:%d 回送功能码:%d ",pfeature->fc, cfc);

			msgDisplay(FRAME_RX, pfeature->cmdDesc , m_recv_msg.msg, m_recv_msg.length);
			memcpy( m_recv_msg.msg, m_recv_msg.msg + frmLen + 6 , m_recv_msg.length - frmLen - 6 );
			m_recv_msg.length = m_recv_msg.length - frmLen - 6;
			//addRecvErrFrmNum();
			return RECEIVE_ERROR_FC;
		}

		resolve_constvalue(pfeature);

		memcpy( m_recv_msg.msg, m_recv_msg.msg + frmLen + 6 , m_recv_msg.length - frmLen - 6 );
		m_recv_msg.length = m_recv_msg.length - frmLen - 6;
		return RECEIVE_SUCCESS;
	}
	else
	{
		//通过较验
		//判断功能码是否是本次问询功能码
		// 
#ifdef SUPPORT_MODBUS_TCP
		hUChar chFc = m_recv_msg.msg[7];
#else
		hUChar chFc = m_recv_msg.msg[1];
#endif

		if( chFc != pfeature->fc )
		{
			logprint(LOG_PROTOCOL + m_route,"收到了非本次问询的结果，本次问询功能码:%d 回送功能码:%d ",pfeature->fc, chFc);

			msgDisplay(FRAME_RX, pfeature->cmdDesc , m_recv_msg.msg, m_recv_msg.length);
			memcpy( m_recv_msg.msg, m_recv_msg.msg + frmLen + 5 , m_recv_msg.length - frmLen - 5 );
			m_recv_msg.length = m_recv_msg.length - frmLen - 5;
			//addRecvErrFrmNum();
			return RECEIVE_ERROR_FC;
		}

		switch(pfeature->frmType)
		{
		case Analog:
			if (pfeature->cmdNo == 0)			//召唤基础数据电压电流功率温度及遥信
			{
				resolve_analogTM(pfeature);		
			}
			else
			{
				resolve_analog(pfeature);
			}
			break;
		case Discrete:
			resolve_discrete(pfeature);
			break;
		case Control:
			resolve_control(pfeature);
			break;
		case SetValue:
			resolve_setvalue(pfeature);
			break;
		case SyncTime:
			break;
		case SOEData:
			resolve_SOEData(pfeature);
			break;
		case ElecCommand:
			resolve_elecdata(pfeature);
			break;
		case HarmonicCmd:
			resolve_harmdata(pfeature);
			break;
		case ParameterCmd:
			resolve_paradata(pfeature);
			break;
		case RecordsData:
		case RecordsData_SOE:
		case RecordsData_YX:
		case RecordsData_Para:
		case RecordsData_Record:
			resolve_RecordsData(pfeature);
			break;
		//case YcCommand:
		//	{
		//		resolve_callYcCommand(pfeature);
		//		if (pfeature->cmdNo == 4)		//装置参数
		//		{
		//			resolve_paradata(pfeature);
		//		}
		//		else
		//		{
		//			resolve_analog(pfeature);
		//		}
		//	}
		//	break;
		default:
			break;
		}

		//updateErrRate();
		m_protocolEvent = Succeeded;

		msgDisplay(FRAME_RX, pfeature->cmdDesc , m_recv_msg.msg, m_recv_msg.length);

		memcpy( m_recv_msg.msg, m_recv_msg.msg + frmLen + 5 , m_recv_msg.length - frmLen - 5 );
		m_recv_msg.length = m_recv_msg.length - frmLen - 5;
		//addRecvOkFrmNum();
		return RECEIVE_SUCCESS;	
	}

	return RECEIVE_SUCCESS;
}

/*寻找同步字头并调整报文，删除先导报文*/ 
bool CProto_Modbus_101::findHeadAndAdjustMsg()
{
	hUChar *buff = m_recv_msg.msg;
	int		i;
	bool	isfind = false;

	PMC_Feature_ptr pfeature = m_current_send_cmd.currFeatureptr;
	if (pfeature->fc == CONSTANTVALUE_READ || pfeature->fc == CONSTANTVALUE_WRITE)
	{
		for ( i = 0 ; i < m_recv_msg.length ; i ++ )
		{
			#ifdef	SUPPORT_MODBUS_TCP
			if( (buff[i] == 0x00) && (buff[i+1] == 0x00) && (buff[i+2] == 0x00) && (buff[i+3] == 0x00) && (buff[i + 6] == m_remoteAddr) ) 
			{			
				isfind = true ;
				break;				
			}
			#else
			if (buff[i] == 0x71 && buff[i+3] == 0x71)
			{
				isfind = true;
				break;
			}
			#endif
		}
	}
	else
	{
		for ( i = 0 ; i < m_recv_msg.length ; i ++ )
		{
#ifdef	SUPPORT_MODBUS_TCP
#ifdef	SUPPORT_MUTL_ROUTE
			DAC_ROUTE	*m_proute = m_commInf.route(m_current_send_cmd.currFeatureptr->routeNo);
			if( (buff[i] == 0x00) && (buff[i+1] == 0x00) && (buff[i+2] == 0x00) && (buff[i+3] == 0x00) && (buff[i + 6] == m_proute->remoteAddr)) 
			{			
				isfind = true ;
				break;				
			}
#else
			if( (buff[i] == 0x00) && (buff[i+1] == 0x00) && (buff[i+2] == 0x00) && (buff[i+3] == 0x00) && (buff[i + 6] == m_remoteAddr) ) 
			{			
				isfind = true ;
				break;				
			}
#endif
#else
#ifdef	SUPPORT_MUTL_ROUTE
			DAC_ROUTE	*m_proute = m_commInf.route(m_current_send_cmd.currFeatureptr->routeNo);
			if(m_proute &&  buff[i] == m_proute->remoteAddr ) 
			{
				isfind = true ;
				break;	
			}
#else
			if( buff[i] == m_remoteAddr ) 
			{
				isfind = true ;
				break;	
			}
#endif
#endif
		}

	}
	
	if( i > 0 )
	{
		memcpy(m_recv_msg.msg, buff + i, m_recv_msg.length - i);
		m_recv_msg.length = m_recv_msg.length - i;
	}

	return isfind;
}

void CProto_Modbus_101::resolve_analog(PMC_Feature_ptr p)
{
	if(p==NULL) return;

#ifdef SUPPORT_MODBUS_TCP
	hUChar *temp = m_recv_msg.msg + 6;
#else
	hUChar *temp = m_recv_msg.msg ;
#endif
	DAC_YC_DATA ycval;
	//bool	fileok = true;
	//QString savfinename = QString("%1//dat//%2.txt").arg( ACE_OS::getenv(SYS_ENV_VAR) ).arg(p->cmdDesc);

	//QFile file(savfinename);
     //if (!file.open(QIODevice::WriteOnly |/* QIODevice::Text |*/ QIODevice::Append))
         //fileok = false;

     //QTextStream out(&file);
	// QDataStream out(&file);
     //out << "The magic number is: " << 49 << "\n";

	//遥测测点序号从0开始
	int ycno = 0;
	int leng = temp[2];//遥测个数
	unsigned short bcd_val=0;
	ycval.quality = 0;
	ycval.updateTime = CDateTime::currentDateTime().toTimeT();
	//if(fileok) 
	//{
	//	//out << QString(CDateTime::currentDateTime().toString(DT_FMT_DEF).c_str()) << "\n";
	//	out << ycval.updateTime << "\n";
	//}

	ycno = p->startNo;
	unsigned short tmp_ycnum = MakeWord( p->lengthL, p->lengthH );
	int	m_route_no = p->routeNo;

	//根据fc2来判断该遥测的format方式，以十六进制来判断，fc2总共16位，前8位表示这个遥测包含字节个数，后8为表示格式化方式
	//前8位支持=1,2,4，高字节在前，=A,B,D，低字节在前，默认按照2字节高字节在前
	//后8位支持=0,1,2,0：unsigned short， 1：short，2：float，23：表示用BCD码解析
	int i;
	switch(p->fc2)
	{
	case	0x20:
		for( i = 0; i < (leng/2) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)MakeWord(temp[4+i*2],temp[3+i*2]);
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0x21:
		for( i = 0; i < (leng/2) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)(short)MakeWord(temp[4+i*2],temp[3+i*2]);
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0x23:
		for( i = 0; i < (leng/2) ; i++)
		{
			if (i >= tmp_ycnum) break;
			CToolFunctions::bcd2bin2(MakeWord(temp[4+i*2],temp[3+i*2]),&bcd_val);
			ycval.val = bcd_val;//(float)(short)MakeWord(temp[4+i*2],temp[3+i*2]);
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0x40:
		for( i = 0; i < (leng/4) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)MakeLong( MakeWord(temp[6+i*4],temp[5+i*4]),MakeWord(temp[4+i*4],temp[3+i*4]));
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0x41:
		for( i = 0; i < (leng/4) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)(int)MakeLong( MakeWord(temp[6+i*4],temp[5+i*4]),MakeWord(temp[4+i*4],temp[3+i*4]));
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0x42:
		for( i = 0; i < (leng/4) ; i++)
		{
			if (i >= tmp_ycnum) break;
			unsigned int	tmp_int = MakeLong( MakeWord(temp[6+i*4],temp[5+i*4]),MakeWord(temp[4+i*4],temp[3+i*4]));
			ycval.val = *(float*)&tmp_int;
			m_dataInf.setYc(m_route_no, ycno, ycval);
			//if(fileok) out << ycval.val /*<< "\t"*/; 
			ycno++;
		}
		//if(fileok) out << "\n"; 
		break;
	case	0x10:
	case	0xA0:
		for( i = 0; i < (leng/1) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)temp[3+i];
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0x11:
	case	0xA1:
		for( i = 0; i < (leng/1) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)(char)temp[3+i];
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0xB0:
		for( i = 0; i < (leng/2) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)MakeWord(temp[3+i*2],temp[4+i*2]);
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0xB1:
		for( i = 0; i < (leng/2) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)(short)MakeWord(temp[3+i*2],temp[4+i*2]);
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0xD0:
		for( i = 0; i < (leng/4) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)MakeLong( MakeWord(temp[3+i*4],temp[4+i*4]),MakeWord(temp[5+i*4],temp[6+i*4]));
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0xD1:
		for( i = 0; i < (leng/4) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)(int)MakeLong( MakeWord(temp[3+i*4],temp[4+i*4]),MakeWord(temp[5+i*4],temp[6+i*4]));
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0xD2:
		for( i = 0; i < (leng/4) ; i++)
		{
			if (i >= tmp_ycnum) break;
			unsigned int	tmp_int = MakeLong( MakeWord(temp[3+i*4],temp[4+i*4]),MakeWord(temp[5+i*4],temp[6+i*4]));
			ycval.val = *(float*)&tmp_int;
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0xD3:
		for( i = 0; i < (leng/4) ; i++)
		{
			if (i >= tmp_ycnum) break;
			unsigned int	tmp_int = MakeLong( MakeWord(temp[4+i*4],temp[3+i*4]),MakeWord(temp[6+i*4],temp[5+i*4]));
			ycval.val = *(float*)&tmp_int;
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0xD4:
		for( i = 0; i < (leng/4) ; i++)
		{
			if (i >= tmp_ycnum) break;
			unsigned int	tmp_int = MakeLong( MakeWord(temp[4+i*4],temp[3+i*4]),MakeWord(temp[6+i*4],temp[5+i*4]));
			ycval.val = tmp_int;
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	case	0xD5:
		for( i = 0; i < (leng/4) ; i++)
		{
			if (i >= tmp_ycnum) break;
			unsigned int	tmp_int = MakeLong( MakeWord(temp[4+i*4],temp[3+i*4]),MakeWord(temp[6+i*4],temp[5+i*4]));
			ycval.val = (float)(int)tmp_int;
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	default:
		for( i = 0; i < (leng/2) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)MakeWord(temp[4+i*2],temp[3+i*2]);
			m_dataInf.setYc(m_route_no, ycno, ycval);
			ycno++;
		}
		break;
	}
	/*if( p->fc2 == 4 )
	{
		for(int i = 0; i < (leng/4) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)( temp[3+i*4]*16777215 + temp[4+i*4]*65535 + temp[5+i*4]*256 + temp[6+i*4]);
			m_dataInf.setYc(m_route, ycno, ycval);
			ycno++;
		}
	}
	else if( p->fc2 == 2 )
	{
		for(int i = 0; i < (leng/2) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)(short)(temp[3+i*2]*256 + temp[4+i*2]);
			m_dataInf.setYc(m_route, ycno, ycval);
			ycno++;
		}
	}
	else
	{
		for(int i = 0; i < (leng/2) ; i++)
		{
			if (i >= tmp_ycnum) break;
			ycval.val = (float)(temp[3+i*2]*256 + temp[4+i*2]);
			m_dataInf.setYc(m_route, ycno, ycval);
			ycno++;
		}
	}*/
}

void CProto_Modbus_101::resolve_discrete(PMC_Feature_ptr p)
{
	if(p==NULL) return;

#ifdef SUPPORT_MODBUS_TCP
	hUChar *temp = m_recv_msg.msg + 6;
#else
	hUChar *temp = m_recv_msg.msg ;
#endif

	DAC_YX_DATA yxval;
	bitset<8> bitemp;

	//遥信测点序号从0开始
	int yxno = 0;
	int leng = temp[2];
	yxval.quality = 0;
	yxval.updateTime = CDateTime::currentDateTime().toTimeT();

	yxno = p->startNo;
	unsigned short tmp_yxnum = MakeWord( p->lengthL, p->lengthH );
	int tmp_yxno = yxno + tmp_yxnum;
	int	m_route_no = p->routeNo;

	if(p->fc == 0x03 || p->fc == 0x04 )
	{
		bitset<16> bite;
		for(int i = 0; i < leng / 2; i++)
		{
			if(p->fc2 == 0xB0)
				bite = (temp[4+i*2]*256 + temp[3+i*2]);
			else
				bite = (temp[3+i*2]*256 + temp[4+i*2]);
			
			for(int j = 0; j < 16 ; j++)
			{
				if (yxno >= tmp_yxno) break;
				yxval.val =  bite.test(j); 
				m_dataInf.setYx(m_route_no, yxno, yxval, true);
#ifdef SUPPORT_XHDL
				if(yxno == 2)
				{
					DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(m_route);
					if ( pRouteInfo != NULL ) 
					{
						if(yxval.val == 0)
						{
							if(pRouteInfo->state != ROUTE_STATE_STANDBY)
								pRouteInfo->state = ROUTE_STATE_STANDBY;
						}
						else if(yxval.val == 1)
						{
							if(pRouteInfo->state != ROUTE_STATE_UP)
								pRouteInfo->state = ROUTE_STATE_UP;
						}
					}
				}
#endif
				yxno++;
			}
		}
	}
	else
	{
		for(int i = 0; i < leng; i++)
		{
			bitemp = temp[3+i];
			for(int j = 0; j < 8 ; j++)
			{
				if (yxno >= tmp_yxno) break;
				yxval.val =  bitemp.test(j); 
				m_dataInf.setYx(m_route_no, yxno, yxval, true);
				yxno++;
			}
		}
	}
}
void CProto_Modbus_101::resolve_control(PMC_Feature_ptr p)
{
#ifdef SUPPORT_MODBUS_TCP
	hUChar *temp = m_recv_msg.msg + 6;
#else
	hUChar *temp = m_recv_msg.msg ;
#endif

	if((temp[1] == p->fc) && (temp[2] == p->addrH) && (temp[3] == p->addrL) /*&& (temp[4] == p->lengthH) && (temp[5] == p->lengthL)*/ )
	{
		reportCmdExecResult(p,0);
	}
	else reportCmdExecResult(p,1);
}
void CProto_Modbus_101::resolve_setvalue(PMC_Feature_ptr p)
{
#ifdef SUPPORT_MODBUS_TCP
	hUChar *temp = m_recv_msg.msg + 6;
#else
	hUChar *temp = m_recv_msg.msg ;
#endif

	if((temp[1] == p->fc) && (temp[2] == p->addrH) && (temp[3] == p->addrL)/* && (temp[4] == p->lengthH) && (temp[5] == p->lengthL)*/ )
	{
		reportCmdExecResult(p,0);
	}
	else reportCmdExecResult(p,1);
}

void CProto_Modbus_101::reportCmdExecResult(PMC_Feature_ptr p,hUChar result)
{
	//ctrl_pro_yk* pYk = (ctrl_pro_yk*)p->ykptr;

	//if(pYk == NULL) return;

	ctrl_pro_yk_ack  ack;
	memset(&ack,0,sizeof(ctrl_pro_yk_ack));
	ack.ctrlNo = p->ykptr.ctrlNo;//pYk->ctrlNo;
	ack.funcCode = p->ykptr.ctrlType;//pYk->ctrlType;
	ack.ackState = result;


	if (p->frmType == Control)
	{

		DAC_YX *yxptr = NULL;
#ifdef SUPPORT_MUTL_ROUTE

		DAC_ROUTE *pRoute = m_commInf.route(p->routeNo, false);
		ack.groupNo = pRoute->group;

		DAC_GROUP *pGroup = m_commInf.group( pRoute->group); 
		for (hUInt32 pointNo = 0;pointNo < pGroup->yxNum ; pointNo++)
		{
			yxptr = m_dataInf.yxPara( pRoute->group,pointNo);  //根据数据组号和遥测号找到遥测表
			if (yxptr == NULL || !yxptr->valid) continue;
			if (yxptr->isControl ==1 && yxptr->ctrlNo == p->ykptr.ctrlNo)
			{
				break;
			}
			yxptr = NULL;
		}
		
		if (yxptr && yxptr->srcGroup > 0 )
		{
			ack.groupNo = yxptr->srcGroup;
		}

		setCtrlCmdAckEx((char*)(&ack),sizeof(ctrl_pro_yk_ack));	


#else
		ack.groupNo = m_grpno; 

		DAC_ROUTE *pRoute = m_commInf.route(m_route);
		DAC_GROUP *pGroup = m_commInf.group( m_grpno); 
		for (hUInt32 pointNo = 0;pointNo < pGroup->yxNum ; pointNo++)
		{
			yxptr = m_dataInf.yxPara( pRoute->group,pointNo);  //根据数据组号和遥信号找到遥信表
			if (yxptr == NULL || !yxptr->valid) continue;
			if (yxptr->isControl == 1 && yxptr->ctrlNo == p->ykptr.ctrlNo)
			{
				break;
			}
			yxptr = NULL;
		}

		if (yxptr && yxptr->srcGroup > 0 )
		{
			ack.groupNo = yxptr->srcGroup;
			setCtrlCmdAckEx((char*)(&ack),sizeof(ctrl_pro_yk_ack));	
		}
		else
		{
			setCtrlCmdAck((char*)(&ack),sizeof(ctrl_pro_yk_ack));
		}	

#endif
	}
	else if (p->frmType == SetValue)
	{
		DAC_YC *ycptr = NULL;

#ifdef SUPPORT_MUTL_ROUTE

		DAC_ROUTE *pRoute = m_commInf.route(p->routeNo, false);
		ack.groupNo = pRoute->group;


		DAC_GROUP *pGroup = m_commInf.group( pRoute->group); 
		for (hUInt32 pointNo = 0;pointNo < pGroup->ycNum ; pointNo++)
		{
			ycptr = m_dataInf.ycPara( pRoute->group,pointNo);  //根据数据组号和遥测号找到遥测表
			if (ycptr == NULL || !ycptr->valid) continue;
			if (ycptr->isControl ==1 && ycptr->ctrlNo == p->ykptr.ctrlNo)
			{
				break;
			}
			ycptr = NULL;
		}

		if (ycptr && ycptr->srcGroup > 0 )
		{
			ack.groupNo = ycptr->srcGroup;
		}
		setCtrlCmdAckEx((char*)(&ack),sizeof(ctrl_pro_yk_ack));	

#else
		ack.groupNo = m_grpno; 

		DAC_ROUTE *pRoute = m_commInf.route(m_route);
		DAC_GROUP *pGroup = m_commInf.group( m_grpno); 
		for (hUInt32 pointNo = 0;pointNo < pGroup->ycNum ; pointNo++)
		{
			ycptr = m_dataInf.ycPara( pRoute->group,pointNo);  //根据数据组号和遥测号找到遥测表
			if (ycptr == NULL || !ycptr->valid) continue;
			if (ycptr->isControl ==1 && ycptr->ctrlNo == p->ykptr.ctrlNo)
			{
				break;
			}
			ycptr = NULL;
		}

		if (ycptr && ycptr->srcGroup > 0 )
		{
			ack.groupNo = ycptr->srcGroup;
			setCtrlCmdAckEx((char*)(&ack),sizeof(ctrl_pro_yk_ack));	
		}
		else
		{
			setCtrlCmdAck((char*)(&ack),sizeof(ctrl_pro_yk_ack));	
		}

#endif
	}

			
				
	logprint(LOG_PROTOCOL  + m_route, "SCL_YK_ACTION_SELECT 0控制执行成功or失败。 result = %d",result);

	return;
}


bool CProto_Modbus_101::SetCVCmdAck(const ctrl_pro_constantvalue_ack* pAck)
{
	if ( !pAck )
		return false;
	if ( !m_commInf.isGroupValid(pAck->groupNo) )
		return false;

	int copyLen = sizeof(ctrl_pro_constantvalue_ack);
	if ( copyLen >= CTRL_COMMON_LEN ) copyLen = CTRL_COMMON_LEN;


	ctrl_pro_common_ack commonAck;
	commonAck.groupNo = pAck->groupNo;
	commonAck.length = copyLen;
	ACE_OS::memcpy(commonAck.ackBuf,pAck,copyLen);
	
	unsigned char buf[512];
	hInt32 headLen,dataLen;

	headLen = sizeof( ctrl_head );
	dataLen = sizeof( ctrl_pro_common_ack);
	ctrl_head*	pHead = (ctrl_head*)buf;
	pHead->type = CTRL_PRO_COMMON_ACK;
	ctrl_pro_common_ack* pData = (ctrl_pro_common_ack*)(buf + headLen);
	ACE_OS::memcpy(pData,&commonAck,dataLen );

	return m_ctrlInf.addAck(CTRL_PRO_COMMON_ACK,buf, headLen + dataLen, commonAck.groupNo);
}



int CProto_Modbus_101::resolve_constvalue(PMC_Feature_ptr pfeature)
{
	if (m_recv_msg.msg[0] == 0x71 && m_recv_msg.msg[3]==0x71)
	{
		// 数据处理
		switch(pfeature->fc)
		{
		case CONSTANTVALUE_READ:	//读
			{
#ifdef SUPPORT_MODBUS_TCP
				hUChar *temp = m_recv_msg.msg + 6;
				int nLen = m_recv_msg.length - 6;
#else
				hUChar *temp = m_recv_msg.msg ;
				int nLen = m_recv_msg.length;
#endif
				_ctrl_pro_constantvalue_ack_  ack;
				memset(&ack,0,sizeof(_ctrl_pro_constantvalue_ack_));
				ack.groupNo = m_grpno;
				//ack.ctrlType = pfeature->fc;

				DAC_YC *ycptr = NULL;
				DAC_ROUTE *pRoute = m_commInf.route(m_route);
				DAC_GROUP *pGroup = m_commInf.group( m_grpno); 
				for (hUInt32 pointNo = 0;pointNo < pGroup->ycNum ; pointNo++)
				{
					ycptr = m_dataInf.ycPara( pRoute->group,pointNo);  //根据数据组号和遥测号找到遥测表
					if (ycptr == NULL || !ycptr->valid)
					{
						ycptr = NULL;
						continue;
					}
					if (ycptr->srcGroup >= 0)
					{
						break;
					}
					ycptr = NULL;
				}

				if (ycptr)
				{
					//ack.groupNo = ycptr->srcGroup;
				}


				if (nLen > CTRL_COMMON_LEN)
				{
					nLen = CTRL_COMMON_LEN;
				}

				if (temp[0] == 0x71 && temp[3] == 0x71 && temp[7] == pfeature->fc)
				{
					if (nLen > 20)
					{
						ACE_OS::memcpy(ack.ackBuf,temp+19,temp[18]);
						ack.length = temp[18];
						ack.ackState = 0;
					}
					else
					{
						ack.length = 0;
						ack.ackState = 1;
					}
				}	
				SetCVCmdAck(&ack);	
			}
			break;
		case CONSTANTVALUE_WRITE:	//写
			{
#ifdef SUPPORT_MODBUS_TCP
				hUChar *temp = m_recv_msg.msg + 6;
#else
				hUChar *temp = m_recv_msg.msg ;
#endif
				_ctrl_pro_constantvalue_ack_  ack;
				memset(&ack,0,sizeof(_ctrl_pro_constantvalue_ack_));
				ack.groupNo = m_grpno;
				//ack.ctrlType = pfeature->fc;
				ack.length = 0;

				DAC_YC *ycptr = NULL;
				DAC_ROUTE *pRoute = m_commInf.route(m_route);
				DAC_GROUP *pGroup = m_commInf.group( m_grpno); 
				for (hUInt32 pointNo = 0;pointNo < pGroup->ycNum ; pointNo++)
				{
					ycptr = m_dataInf.ycPara( pRoute->group,pointNo);  //根据数据组号和遥测号找到遥测表
					if (ycptr == NULL || !ycptr->valid)
					{
						ycptr = NULL;
						continue;
					}
					if (ycptr->srcGroup >= 0)
					{
						break;
					}
					ycptr = NULL;
				}

				if (ycptr)
				{
					//ack.groupNo = ycptr->srcGroup;
				}

				if (temp[0] == 0x71 && temp[3] == 0x71 && temp[7] == pfeature->fc)
				{
					ack.ackState = 0;
				}
				else
				{
					ack.ackState = 1;
				}
				SetCVCmdAck(&ack);	
			}
			break;

		default:
			break;
		}

		m_protocolEvent = Succeeded;

		msgDisplay(FRAME_RX, pfeature->cmdDesc , m_recv_msg.msg, m_recv_msg.length);
	}
 return RECEIVE_ERROR_FC;
}
bool CProto_Modbus_101::getSOEData()
{
	if (m_nSOENum < 0)
	{
		m_current_send_cmd.currFeatureptr = m_pcmd;
		m_current_send_cmd.currFeatureptr->isAck = true;
		m_current_send_cmd.currFeatureptr->isUsed = true;
		m_current_send_cmd.currFeatureptr->fc = 0x03;
		m_current_send_cmd.currFeatureptr->addrH = 0x90;
		m_current_send_cmd.currFeatureptr->addrL = 0x00;
		m_current_send_cmd.currFeatureptr->lengthH = 0x00;
		m_current_send_cmd.currFeatureptr->lengthL = 0x02;
		m_current_send_cmd.currFeatureptr->frmType = SOEData;
		CString strDesc = "获取SOE条数";
		ACE_OS::strncpy(m_current_send_cmd.currFeatureptr->cmdDesc, strDesc.c_str(), PMC_DESCRIBE_STRING_LENGTH);

		int i = 0;

		m_current_send_cmd.cmd[i++] = m_remoteAddr;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->fc;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->addrH;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->addrL;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->lengthH;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->lengthL;

		hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, i);

		m_current_send_cmd.cmd[i++] = HIBYTE(crc);
		m_current_send_cmd.cmd[i++] = LOBYTE(crc);
		m_current_send_cmd.length = i;

		ACE_OS::strncpy(m_current_send_cmd.cmdStr, strDesc.c_str(), PMC_DESCRIBE_STRING_LENGTH);
		
	}
	else
	{
		int Addr = 0x9200 + (m_nSOENum - 1)* 11;
		m_current_send_cmd.currFeatureptr = m_pcmd;
		m_current_send_cmd.currFeatureptr->isAck = true;
		m_current_send_cmd.currFeatureptr->isUsed = true;
		m_current_send_cmd.currFeatureptr->fc = 0x03;
		m_current_send_cmd.currFeatureptr->addrH = HIBYTE(Addr);
		m_current_send_cmd.currFeatureptr->addrL = LOBYTE(Addr);
		m_current_send_cmd.currFeatureptr->lengthH = 0x00;
		m_current_send_cmd.currFeatureptr->lengthL = 0x58;
		m_current_send_cmd.currFeatureptr->frmType = SOEData;
		

		int nNum = m_nSOENum < 8 ?  m_nSOENum : 8;
		m_current_send_cmd.currFeatureptr->fcLength = nNum;

		QString strDesc = QString("获取第 %1 条 -- 第 %2 条SOE数据")
						.arg(m_nSOENum - nNum)
						.arg(m_nSOENum -1);
	
		QByteArray ba = strDesc.toLocal8Bit();  
		ACE_OS::strncpy(m_current_send_cmd.currFeatureptr->cmdDesc, ba.data(), PMC_DESCRIBE_STRING_LENGTH);


		int i = 0;

		m_current_send_cmd.cmd[i++] = m_remoteAddr;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->fc;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->addrH;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->addrL;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->lengthH;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->lengthL;

		hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, i);

		m_current_send_cmd.cmd[i++] = HIBYTE(crc);
		m_current_send_cmd.cmd[i++] = LOBYTE(crc);
		m_current_send_cmd.length = i;

		ACE_OS::strncpy(m_current_send_cmd.cmdStr, ba.data(), PMC_DESCRIBE_STRING_LENGTH);
		m_nSOENum -= nNum;
	}

	return true;
}


void CProto_Modbus_101::resolve_SOEData(PMC_Feature_ptr p)
{
	if(p==NULL) return;

#ifdef SUPPORT_MODBUS_TCP
	hUChar *temp = m_recv_msg.msg + 6;
#else
	hUChar *temp = m_recv_msg.msg ;
#endif
	
	if (m_nSOENum < 0 )		//条数
	{
		m_nSOENum = (short) MakeWord(temp[4],temp[3]);
		m_nSumSOE = m_nSOENum;

		if (m_nSumSOE <= 0)
		{
			m_nSOENum = -1;

			char buf[CTRL_COMMON_LEN];
			memcpy(buf, &m_nSOENum, sizeof(short));
			setCtrlCmdAck(buf, sizeof(short));
		}

	}
	else	//数据
	{
		int leng = p->fcLength;
		for (int i = 0; i < leng; i++)
		{
			ctrl_pro_data_soe sData;
			//sData.nDataNo = m_nSOENum - nNum + i;
			sData.nErrorType = (float)MakeWord(temp[4+i*22],temp[3+i*22]);
			sData.nAction1 = MakeLong( MakeWord(temp[6+i*22],temp[5+i*22]),MakeWord(temp[8+i*22],temp[7+i*22]));
			sData.nAction2 = MakeLong( MakeWord(temp[10+i*22],temp[9+i*22]),MakeWord(temp[12+i*22],temp[11+i*22]));
			sData.nAction3 = MakeLong( MakeWord(temp[14+i*22],temp[13+i*22]),MakeWord(temp[16+i*22],temp[15+i*22]));
			
			CDateTime datetime;

			datetime.second((float)MakeWord(temp[18+i*22],temp[17+i*22]));
			datetime.hour(temp[19+i*22]);
			datetime.minute(temp[20+i*22]);
			datetime.month(temp[21+i*22]);
			datetime.day(temp[22+i*22]);
			datetime.year((float)MakeWord(temp[24+i*22],temp[23+i*22]));

			sData.datetime = datetime.toTimeT();
			
			m_nVector.append(sData);
		}
		
		{
			short cState = m_nSumSOE;
			char buf[CTRL_COMMON_LEN];
			memset(buf, 0, CTRL_COMMON_LEN);

			////add by yaoff for test on [2017/7/24 14:49:39]
			//
			//	char filePathName[256];
			//	FILE* fp = NULL;
			//	ACE_OS::snprintf(filePathName,256,"%s/SOE/SOE.pdt",ACE_OS::getenv (SYS_ENV_VAR));
			//	if( ACE_OS::filesize(filePathName) > BLOG_FILE_LEN )	
			//		fp = ACE_OS::fopen(filePathName,"w+");  
			//	else	
			//		fp = ACE_OS::fopen(filePathName,"a+");
			//
			////add end. [2017/7/24 14:49:39]

			while (m_nVector.size() > 0)
			{
				int nLen = 8;
				int nSize = m_nVector.size();
				if (nSize <= nLen)
				{
					nLen = nSize;
				}
				if (m_nSOENum == 0)
				{
					cState = 300;
				}

				memcpy(buf, &cState, sizeof(short));
				
				//char head[1024];

				int nNo = 0;
				for(int i = nSize-1; i >= nSize-nLen; i--)
				{
					memcpy(buf + sizeof(short) + (nNo++)*sizeof(ctrl_pro_data_soe), &(m_nVector[i]), sizeof(ctrl_pro_data_soe));
					m_nVector.remove(i);
				}
				
				//int copyLen = sizeof(short) + (nNo)*sizeof(ctrl_pro_data_soe);

				////add by yaoff for test on [2017/7/24 14:49:39]
				//{
				//	short cst = 0;
				//	memcpy(&cst, buf, sizeof(short));

				//	QString str = QString("state: %1 ").arg(cst);
				//	int nnn = sizeof(short);
				//	int nf = copyLen/sizeof(ctrl_pro_data_soe);
				//	while(nf > 0)
				//	{
				//		ctrl_pro_data_soe pYk;
				//		memcpy((&pYk), (buf + nnn), sizeof(ctrl_pro_data_soe));
				//		QDateTime tt = QDateTime::fromTime_t(pYk.datetime);
				//		QString str1 = QString("SOE--- ErrorType:%1  Action1:%2  Action2:%3  Action3:%4  datetime: %5\n")
				//			.arg(pYk.nErrorType)
				//			.arg(pYk.nAction1)
				//			.arg(pYk.nAction2)
				//			.arg(pYk.nAction3)
				//			.arg(tt.toString("yy-MM-dd hh:mm:ss"));
				//		str += str1;
				//		nnn += sizeof(ctrl_pro_data_soe);
				//		nf--;
				//	}
				//	ACE_OS::sprintf(head,"%s", str.toStdString().c_str());	
				//	if ( fp != NULL ) 
				//		ACE_OS::fprintf(fp,"%s",head);
				//}
				////add end. [2017/7/24 14:49:39]

				setCtrlCmdAck(buf, CTRL_COMMON_LEN);
			}
			//if ( fp != NULL ) ACE_OS::fclose(fp);

		}

		if (m_nSOENum == 0)
		{
			m_nSOENum = -1;
		}
	}
}


bool CProto_Modbus_101::getRecordsData()
{
	if (m_nRecordsNum < 0)
	{
		// 重新请求录波数据前，需要先删除旧的数据。
		//char cfilePathName[256];
		//ACE_OS::snprintf(cfilePathName,256,"%s/Record/",ACE_OS::getenv (SYS_ENV_VAR));
		QString strPath = QString("%1/cmddata/record/").arg( ACE_OS::getenv(SYS_ENV_VAR) );
		
		QDir d(strPath);
		d.setFilter(QDir::Files);
		int nCount =d.count()-1;

		for (int i = 0;i <= nCount; i++)
		{
			d.remove(d[i]);
		}

		m_current_send_cmd.currFeatureptr = m_pcmd;
		m_current_send_cmd.currFeatureptr->isAck = true;
		m_current_send_cmd.currFeatureptr->isUsed = true;
		m_current_send_cmd.currFeatureptr->fc = 0x03;
		m_current_send_cmd.currFeatureptr->addrH = 0xB0;
		m_current_send_cmd.currFeatureptr->addrL = 0x00;
		m_current_send_cmd.currFeatureptr->lengthH = 0x00;
		m_current_send_cmd.currFeatureptr->lengthL = 0x02;
		m_current_send_cmd.currFeatureptr->frmType = RecordsData;
		QString strDesc = "获取录波条数";
		ACE_OS::strncpy(m_current_send_cmd.currFeatureptr->cmdDesc, strDesc.toStdString().c_str(), PMC_DESCRIBE_STRING_LENGTH);

		int i = 0;

		m_current_send_cmd.cmd[i++] = m_remoteAddr;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->fc;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->addrH;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->addrL;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->lengthH;
		m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->lengthL;

		hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, i);

		m_current_send_cmd.cmd[i++] = HIBYTE(crc);
		m_current_send_cmd.cmd[i++] = LOBYTE(crc);
		m_current_send_cmd.length = i;

		ACE_OS::strncpy(m_current_send_cmd.cmdStr, m_current_send_cmd.currFeatureptr->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);

	}
	else
	{
		if (m_record_feature.size() > 0)
		{
			PMC_Feature*  pFeature  = m_record_feature.first(); 

			m_pcmd->cmdNo = pFeature->cmdNo;
 			m_pcmd->isAck = pFeature->isAck;
 			m_pcmd->isUsed = pFeature->isUsed;
 			m_pcmd->fc = pFeature->fc;
 			m_pcmd->addrH = pFeature->addrH;
 			m_pcmd->addrL = pFeature->addrL;
 			m_pcmd->lengthH = pFeature->lengthH;
 			m_pcmd->lengthL = pFeature->lengthL;
 			m_pcmd->frmType = pFeature->frmType;
			ACE_OS::strncpy(m_pcmd->cmdDesc, pFeature->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);

			m_current_send_cmd.currFeatureptr = m_pcmd;
			ACE_OS::strncpy(m_current_send_cmd.currFeatureptr->cmdDesc,m_pcmd->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);

			int i = 0;

			m_current_send_cmd.cmd[i++] = m_remoteAddr;
			m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->fc;
			m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->addrH;
			m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->addrL;
			m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->lengthH;
			m_current_send_cmd.cmd[i++] = m_current_send_cmd.currFeatureptr->lengthL;

			hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, i);

			m_current_send_cmd.cmd[i++] = HIBYTE(crc);
			m_current_send_cmd.cmd[i++] = LOBYTE(crc);
			m_current_send_cmd.length = i;

			delete pFeature;
			pFeature = NULL;
			m_record_feature.pop_front();
		}

		if (m_record_feature.size() == 0)
		{
			m_nRecordsNum = -1;
		}
		//if (m_record_feature.size() == 0)
		//{
		//	m_nRecordsNum = -1;
		//}
		ACE_OS::strncpy(m_current_send_cmd.cmdStr, m_current_send_cmd.currFeatureptr->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);

	}

	return true;
}


void CProto_Modbus_101::resolve_RecordsData(PMC_Feature_ptr p)
{
	if(p == NULL) return;

#ifdef SUPPORT_MODBUS_TCP
	hUChar *temp = m_recv_msg.msg + 6;
#else
	hUChar *temp = m_recv_msg.msg ;
#endif

	if (p->frmType == RecordsData)
	{
		m_nRecordsNum = (int) MakeWord(temp[4],temp[3]);

		char buf[CTRL_COMMON_LEN];
		memset(buf, 0, CTRL_COMMON_LEN);
		memcpy(buf, &m_nRecordsNum, sizeof(int));
		setCtrlCmdAck(buf, CTRL_COMMON_LEN);

		for (int i = m_record_feature.size()-1; i >=0 ; i--)
		{
			PMC_Feature*  pFeature = m_record_feature.last();
			delete pFeature;
			pFeature = NULL;
			m_record_feature.pop_back();
		}
		//if (m_record_feature.size() == 0)
		{
			hInt32	cmdNo = 0;

			for (int i = 0; i < m_nRecordsNum; i++)
			{
				PMC_Feature*  pFeature1 = new PMC_Feature;
				pFeature1->cmdNo = cmdNo++;
				pFeature1->isAck = true;
				pFeature1->isUsed = true;
				pFeature1->fc = 0x03;
				hUInt16 tmp_addr1 = 0xB200 + i;
				pFeature1->addrH = HIBYTE(tmp_addr1);
				pFeature1->addrL = LOBYTE(tmp_addr1);
				pFeature1->lengthH = 0x00;
				pFeature1->lengthL = 0x0B;
				pFeature1->frmType = RecordsData_SOE;
				QString strDesc = "RecordsData_SOE";
				ACE_OS::strncpy(pFeature1->cmdDesc, strDesc.toStdString().c_str(), PMC_DESCRIBE_STRING_LENGTH);

				m_record_feature.push_back(pFeature1);

				PMC_Feature* pFeature2 = new PMC_Feature;
				pFeature2->isAck = true;
				pFeature2->isUsed = true;
				pFeature2->fc = 0x03;
				hUInt16 tmp_addr2 = 0x8400 + i;
				pFeature2->addrH = HIBYTE(tmp_addr2);
				pFeature2->addrL = LOBYTE(tmp_addr2);
				pFeature2->lengthH = 0x00;
				pFeature2->lengthL = 0x04;
				pFeature2->frmType = RecordsData_YX;
				strDesc = "RecordsData_YX";
				ACE_OS::strncpy(pFeature2->cmdDesc, strDesc.toStdString().c_str(), PMC_DESCRIBE_STRING_LENGTH);

				m_record_feature.push_back(pFeature2);

				PMC_Feature* pFeature3 = new PMC_Feature;
				pFeature3->isAck = true;
				pFeature3->isUsed = true;
				pFeature3->fc = 0x03;
				hUInt16 tmp_addr3 = 0x8600 + i;
				pFeature3->addrH = HIBYTE(tmp_addr3);
				pFeature3->addrL = LOBYTE(tmp_addr3);
				pFeature3->lengthH = 0x00;
				pFeature3->lengthL = 0x35;
				pFeature3->frmType = RecordsData_Para;
				strDesc = "RecordsData_Para";
				ACE_OS::strncpy(pFeature3->cmdDesc, strDesc.toStdString().c_str(), PMC_DESCRIBE_STRING_LENGTH);

				m_record_feature.push_back(pFeature3);

				for (int m = 0; m < RECORD_CHANEL_NUM; m++)
				{
					for (int n = 0; n < RECORD_CMD_NUM; n++)
					{
						PMC_Feature* pFeature4 = new PMC_Feature;
						pFeature4->cmdNo = (m+1)*(n+1) -1;
						pFeature4->isAck = true;
						pFeature4->isUsed = true;
						pFeature4->fc = 0x03;
						hUInt16 tmp_addr4 = 0xC200 + 0x10*m + 0x01*n;
						pFeature4->addrH = HIBYTE(tmp_addr4);
						pFeature4->addrL = LOBYTE(tmp_addr4);
						hUInt16 tmp_lenth = 0x0001*(i+1);
						pFeature4->lengthH = HIBYTE(tmp_lenth);
						pFeature4->lengthL = LOBYTE(tmp_lenth);
						pFeature4->frmType = RecordsData_Record;
						strDesc = QString("RecordsData_Record%1_%2").arg(m).arg(n);
						ACE_OS::strncpy(pFeature4->cmdDesc, strDesc.toStdString().c_str(), PMC_DESCRIBE_STRING_LENGTH);
						m_record_feature.push_back(pFeature4);
					}
				}

			}

		}
		return;
	}

	
	if (p->frmType == RecordsData_SOE)
	{
		ACE_OS::snprintf(m_filePathName,256,"%s/cmddata/record/%d.pdt",ACE_OS::getenv (SYS_ENV_VAR),p->cmdNo);
	}
	
	FILE* fp = NULL;

	if( ACE_OS::filesize(m_filePathName) > BLOG_FILE_LEN )	
		fp = ACE_OS::fopen(m_filePathName,"w+");  
	else	
		fp = ACE_OS::fopen(m_filePathName,"a+");

	if(fp == NULL)
	{
		printf("打开文件 %s 失败\n",m_filePathName);
		return;
	}
	
	char head[256];


	CString strFile;
	switch(p->frmType)
	{
	case RecordsData_SOE:
		{
			ACE_OS::sprintf(head,"{\"groupno\":\"%d\",", m_grpno);
			ACE_OS::fprintf(fp,"%s",head);
			ACE_OS::sprintf(head,"\"SOE\":{\"ErrorType\":\"%d\",\"Action1\":\"%d\",\"Action2\":\"%d\",\"Action3\":\"%d\",\"datetime\":\"%04d-%02d-%02d %02d:%02d:%02d %d\"},",
				(int)MakeWord(temp[4],temp[3]),			//nErrorType
				MakeLong( MakeWord(temp[6],temp[5]),MakeWord(temp[8],temp[7])),		//nAction1
				MakeLong( MakeWord(temp[10],temp[9]),MakeWord(temp[12],temp[11])),	//nAction2
				MakeLong( MakeWord(temp[14],temp[13]),MakeWord(temp[16],temp[15])), //nAction3
				(int)MakeWord(temp[24],temp[23]),	//年
				temp[21],							//月
				temp[22],							//日
				temp[19],							//时
				temp[20],							//分
				(int)MakeWord(temp[18],temp[17])/1000,
				(int)MakeWord(temp[18],temp[17])%1000	);	//秒

			
			ACE_OS::fprintf(fp,"%s",head);
		}
		break;
	case RecordsData_YX:
		{
			ACE_OS::fprintf(fp,"\"YX\":{");


			bitset<16> bite;
			int nIndex = 0;
			
			for(int i = 0; i < 4; i++)
			{
				bite = (temp[3+i*2]*256 + temp[4+i*2]);

				for(int j = 0; j < 16 ; j++)
				{
					DAC_YX* yxptr = m_dataInf.yxPara(m_grpno, nIndex++);
					ACE_OS::fprintf(fp,"\"%s\":\"%d\",", yxptr->name, bite.test(j));
				}
			}
 			
 			ACE_OS::fprintf(fp,"},");

		}
		break;
	case RecordsData_Para:
		{
			ACE_OS::fprintf(fp,"\"PARA\":[\"1\":{");

			bitset<16> bite;
			int nIndex = 64;
			for(int i = 0; i < 1; i++)
			{
				bite = (temp[3+i*2]*256 + temp[4+i*2]);

				for(int j = 0; j < 16 ; j++)
				{
					DAC_YX* yxptr = m_dataInf.yxPara(m_grpno, nIndex++);
					ACE_OS::fprintf(fp,"\"%s\":\"%d\",", yxptr->name, bite.test(j));
				}
			}

			ACE_OS::fprintf(fp,"},\"2\":{");

			int leng = temp[2];
			nIndex = 259;
			for(int i = 0; i < (leng/2) ; i++)
			{
				DAC_YC* ycptr = m_dataInf.ycPara(m_grpno, nIndex++);
				ACE_OS::fprintf(fp,"\"%s\":%.2f,",ycptr->name, (float)MakeWord(temp[4+i*2],temp[3+i*2]));			
			}

			ACE_OS::fprintf(fp,"}],");

		}
		break;
	case RecordsData_Record:
		{
			if (p->cmdNo == 0)
			{
				ACE_OS::fprintf(fp,"\"RECORD\":[");
			}

			int leng = temp[2];
			int cc = temp[3]>>4;
			int nn = temp[3]&0x0F;

			if (nn == 0)
			{
				ACE_OS::fprintf(fp,"{");
			}
			ACE_OS::sprintf(head, "\"%d-%d\": \"", cc, nn);
			ACE_OS::fprintf(fp,"%s",head);
			for(int i = 0; i < ((leng-3)/2) ; i++)
			{
				ACE_OS::fprintf(fp,"%d,",(short)MakeWord(temp[7+i*2],temp[6+i*2]));
			}

			ACE_OS::fprintf(fp,"\",");
			if (nn == 4)
			{
				ACE_OS::fprintf(fp,"},");
				if (cc == 5)
				{
					ACE_OS::fprintf(fp,"]}");
				}
			}
		}
		break;
	default:
		break;
	}

	if ( fp != NULL ) ACE_OS::fclose(fp);
}




void CProto_Modbus_101::resolve_analogTM(PMC_Feature_ptr p)
{
	if(p == NULL) return;

#ifdef SUPPORT_MODBUS_TCP
	hUChar *temp = m_recv_msg.msg + 6;
#else
	hUChar *temp = m_recv_msg.msg ;
#endif

	int ycno = p->startNo;
	DAC_YC_DATA ycval;
	ycval.quality = 0;
	ycval.updateTime = CDateTime::currentDateTime().toTimeT();

	int	nRouteNo = p->routeNo;

	// 电压电流
	for(int i = 0; i < 6 ; i++)
	{
		ycval.val = (float)MakeWord(temp[4+i*2],temp[3+i*2]);
		m_dataInf.setYc(nRouteNo, ycno, ycval);
		ycno++;
	}

	//有功功率 无功功率 
	for(int i = 0; i < 8; i++)
	{
		ycval.val = (float)(short)MakeWord(temp[16+i*2],temp[15+i*2]);
		m_dataInf.setYc(nRouteNo, ycno, ycval);
		ycno++;
	}

	// 电源频率
	ycval.val = (float)MakeWord(temp[32],temp[31]);
	m_dataInf.setYc(nRouteNo, ycno, ycval);
	ycno++;

	//功率因数 温度
	for(int i = 0; i < 7 ; i++)
	{
		ycval.val = (float)(short)MakeWord(temp[34+i*2],temp[33+i*2]);
		m_dataInf.setYc(nRouteNo, ycno, ycval);
		ycno++;
	}


	//遥信
	DAC_YX_DATA yxval;

	int yxno = 0;
	yxval.quality = 0;
	yxval.updateTime = CDateTime::currentDateTime().toTimeT();

	bitset<16> bite;
	for(int i = 0; i < 4; i++)
	{
		bite = (temp[47+i*2]*256 + temp[48+i*2]);

		for(int j = 0; j < 16 ; j++)
		{
			yxval.val =  bite.test(j); 
			m_dataInf.setYx(nRouteNo, yxno, yxval);
			yxno++;
		}
	}

	//总用电量电能
	for(int i = 0; i < 5; i++)
	{
		ycval.val = (float)MakeLong(MakeWord(temp[58+i*4],temp[57+i*4]), MakeWord(temp[56+i*4],temp[55+i*4]));
		m_dataInf.setYc(nRouteNo, ycno, ycval);
		ycno++;
	}
	
	//谐波
	for(int i = 0; i < 6 ; i++)
	{
		ycval.val = (float)MakeWord(temp[76+i*2],temp[75+i*2]);
		m_dataInf.setYc(nRouteNo, ycno, ycval);
		ycno++;
	}

	/*  7月2日前测点
	if(p == NULL) return;

#ifdef SUPPORT_MODBUS_TCP
	hUChar *temp = m_recv_msg.msg + 6;
#else
	hUChar *temp = m_recv_msg.msg ;
#endif

	int ycno = p->startNo;
	
	DAC_YC_DATA ycval;
	ycval.quality = 0;
	ycval.updateTime = CDateTime::currentDateTime().toTimeT();

	int	nRouteNo = p->routeNo;

	// 电压电流
	for(int i = 0; i < 6 ; i++)
	{
		ycval.val = (float)MakeWord(temp[4+i*2],temp[3+i*2]);
		m_dataInf.setYc(nRouteNo, ycno, ycval);
		ycno++;
	}

	//有功功率 无功功率 视在功率
	for(int i = 0; i < 3; i++)
	{
		unsigned int	tmp_int = MakeLong( MakeWord(temp[16+i*10],temp[15+i*10]),MakeWord(temp[18+i*10],temp[17+i*10]));
		ycval.val = (float)(int)tmp_int;
		m_dataInf.setYc(nRouteNo, ycno, ycval);
		ycno++;

		m_dataInf.setYc(nRouteNo, ycno, ycval);
		ycno++;	

		ycval.val = (float)(short)MakeWord(temp[20+i*10],temp[19+i*10]);
		m_dataInf.setYc(nRouteNo, ycno, ycval);
		ycno++;

		ycval.val = (float)(short)MakeWord(temp[22+i*10],temp[21+i*10]);
		m_dataInf.setYc(nRouteNo, ycno, ycval);
		ycno++;

		ycval.val = (float)(short)MakeWord(temp[24+i*10],temp[23+i*10]);
		m_dataInf.setYc(nRouteNo, ycno, ycval);
		ycno++;
	}


	// 电源频率 功率因数 温度
	for(int i = 0; i < 8 ; i++)
	{
		ycval.val = (float)(short)MakeWord(temp[46+i*2],temp[45+i*2]);
		m_dataInf.setYc(nRouteNo, ycno, ycval);
		ycno++;
	}


	//遥信
	DAC_YX_DATA yxval;

	int yxno = 0;
	yxval.quality = 0;
	yxval.updateTime = CDateTime::currentDateTime().toTimeT();

	bitset<16> bite;
	for(int i = 0; i < 4; i++)
	{
		bite = (temp[61+i*2]*256 + temp[62+i*2]);

		for(int j = 0; j < 16 ; j++)
		{
			yxval.val =  bite.test(j); 
			m_dataInf.setYx(nRouteNo, yxno, yxval);
			yxno++;
		}
	}
*/
}

bool PDT::CProto_Modbus_101::sendUdp()
{  
	char buff[1024];
	char fpath[256]; 
	ACE_OS::snprintf(fpath,256,"%s/bin/web/tmp/senddev_para.csv",ACE_OS::getenv (SYS_ENV_VAR));

	FILE* fp = ACE_OS::fopen(fpath,"r+");  

	if(fp == NULL)
	{
		printf("打开文件 %s 失败\n",fpath);
		return FALSE;
	}

	//读数据，加入vector
	QString str;
	QStringList ll;
	bool ok;
	fgets(buff, sizeof(buff), fp);
	while(fgets(buff, sizeof(buff), fp))
	{
		str = buff;
		ll = str.split(",");
		str = ll.at(0);
		if (ll.count() < 3 || str.length()< 12 || str.toULong() <= 0)
		{
			ll.clear();
			continue;
		}
		sendpara para;
		memset(para.cmac, 0, 16);		
		memcpy(para.cmac, str.toStdString().c_str(), str.length());
		para.rtu = ll.at(1).toUInt();
		para.frequency = ll.at(2).toUInt();
		m_vecPara.push_front(para);
		ll.clear();
	}



	//char mac[10];
	int i = 0;
	if (m_vecPara.size() > 0)
	{
		sendpara para = m_vecPara.last();
		//sprintf(mac, "%.10lX",para.lmac);
		str = QString("%1").arg(para.cmac);

		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x10;
		m_current_send_cmd.cmd[i++] = 0x05;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x0D;
		m_current_send_cmd.cmd[i++] = 0x1A;

		m_current_send_cmd.cmd[i++] = HiByte(str.mid(0,4).toUInt(0,16));
		m_current_send_cmd.cmd[i++] = LoByte(str.mid(0,4).toUInt(0,16));
		m_current_send_cmd.cmd[i++] = HiByte(str.mid(4,4).toUInt(0,16));
		m_current_send_cmd.cmd[i++] = LoByte(str.mid(4,4).toUInt(0,16));		
		m_current_send_cmd.cmd[i++] = HiByte(str.mid(8,4).toUInt(0,16));
		m_current_send_cmd.cmd[i++] = LoByte(str.mid(8,4).toUInt(0,16));

		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x02;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = para.frequency;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x1A;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x01;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = para.rtu;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x00;
		m_current_send_cmd.cmd[i++] = 0x00;

		hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, i);

		m_current_send_cmd.cmd[i++] = HIBYTE(crc);
		m_current_send_cmd.cmd[i++] = LOBYTE(crc);
		m_current_send_cmd.length = i;

		m_vecPara.pop_back();
		//ACE_OS::strncpy(m_current_send_cmd.cmdStr, m_current_send_cmd.currFeatureptr->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);

	}


	return TRUE;

}


bool PDT::CProto_Modbus_101::editpara(ctrl_pro_constantvalue* pCom)
{  
	if (pCom == NULL)
	{
		return false;
	}

	int i = 0;

	sendpara para = m_vecPara.last();
	
	QString qstr = QString("%s").arg(pCom->cmdBuf);
	QStringList ll = qstr.split(":");
	bool bOk;
	QString str = ll.at(0).trimmed();

	m_current_send_cmd.cmd[i++] = m_remoteAddr;
	m_current_send_cmd.cmd[i++] = 0x10;
	m_current_send_cmd.cmd[i++] = 0x05;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x0D;
	m_current_send_cmd.cmd[i++] = 0x1A;

	m_current_send_cmd.cmd[i++] = HiByte(str.mid(0,4).toUInt(0,16));
	m_current_send_cmd.cmd[i++] = LoByte(str.mid(0,4).toUInt(0,16));
	m_current_send_cmd.cmd[i++] = HiByte(str.mid(4,4).toUInt(0,16));
	m_current_send_cmd.cmd[i++] = LoByte(str.mid(4,4).toUInt(0,16));		
	m_current_send_cmd.cmd[i++] = HiByte(str.mid(8,4).toUInt(0,16));
	m_current_send_cmd.cmd[i++] = LoByte(str.mid(8,4).toUInt(0,16));



	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x02;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = ll.at(1).toInt(&bOk, 16);
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x1A;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x01;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = ll.at(2).toInt(&bOk, 16);;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x00;
	m_current_send_cmd.cmd[i++] = 0x00;

	hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, i);

	m_current_send_cmd.cmd[i++] = HIBYTE(crc);
	m_current_send_cmd.cmd[i++] = LOBYTE(crc);
	m_current_send_cmd.length = i;

	m_vecPara.pop_back();

	return TRUE;

}


void PDT::CProto_Modbus_101::resolve_elecdata( PMC_Feature_ptr p )
{/*
	if(p == NULL) return;

#ifdef SUPPORT_MODBUS_TCP
	hUChar *temp = m_recv_msg.msg + 6;
#else
	hUChar *temp = m_recv_msg.msg ;
#endif

	int leng = temp[2];//遥测个数
	int ntype = 0;

	char fpath[256]; 
	ACE_OS::snprintf(fpath,256,"%s/cmddata/elec.pdt",ACE_OS::getenv (SYS_ENV_VAR));

	FILE* fp = NULL;

	if( ACE_OS::filesize(fpath) > BLOG_FILE_LEN )	
		fp = ACE_OS::fopen(fpath,"w+");  
	else	
		fp = ACE_OS::fopen(fpath,"a+");

	if(fp == NULL)
	{
		printf("打开文件 %s 失败\n",fpath);
		return;
	}

	PDT::CCommInf	m_commInf;
	DAC_YCCON* ptr = m_commInf.yccon(ntype);

	ACE_OS::fprintf(fp,"{\"time\":\"%u\",\"yc\":{", (hUInt32)ACE_OS::time(0));

	for(int i = 0; i < (leng/4) ; i++)
	{
		if (ptr->ntype != ntype)	break;
		ACE_OS::fprintf(fp,"\"%s\":\"%.2f\"",ptr->name, (float)MakeLong( MakeWord(temp[6+i*4],temp[5+i*4]),MakeWord(temp[4+i*4],temp[3+i*4])));
		if (i!=(leng/4)-1)
		{
			ACE_OS::fprintf(fp,",");
		}
		ptr++;
	}

	ACE_OS::fprintf(fp,"}}",time(NULL));

	if ( fp != NULL ) ACE_OS::fclose(fp);
	*/
}

void PDT::CProto_Modbus_101::resolve_harmdata( PMC_Feature_ptr p )
{/*
	if(p == NULL) return;

#ifdef SUPPORT_MODBUS_TCP
	hUChar *temp = m_recv_msg.msg + 6;
#else
	hUChar *temp = m_recv_msg.msg ;
#endif

	int leng = temp[2];//遥测个数
	int ntype = 1;

	char fpath[256]; 
	ACE_OS::snprintf(fpath,256,"%s/cmddata/harm.pdt",ACE_OS::getenv (SYS_ENV_VAR));

	FILE* fp = NULL;

	if( ACE_OS::filesize(fpath) > BLOG_FILE_LEN )	
		fp = ACE_OS::fopen(fpath,"w+");  
	else	
		fp = ACE_OS::fopen(fpath,"a+");

	if(fp == NULL)
	{
		printf("打开文件 %s 失败\n",fpath);
		return;
	}

	PDT::CCommInf	m_commInf;
	DAC_YCCON* ptr = m_commInf.yccon(ntype);


	ACE_OS::fprintf(fp,"\"%s\":{",p->cmdDesc);

	for(int i = 0; i < (leng/2) ; i++)
	{
		if (ptr->ntype != ntype)	break;
		ACE_OS::fprintf(fp,"\"%s\":\"%.2f\"",ptr->name, (float)MakeWord(temp[4+i*2],temp[3+i*2]));
		if (i!=(leng/4)-1)
		{
			ACE_OS::fprintf(fp,",");
		}
		ptr++;
	}
	ACE_OS::fprintf(fp,"}",time(NULL));

	if ( fp != NULL ) ACE_OS::fclose(fp);
	*/
}





void CProto_Modbus_101::resolve_paradata(PMC_Feature_ptr p)
{/*
	if(p == NULL) return;

#ifdef SUPPORT_MODBUS_TCP
	hUChar *temp = m_recv_msg.msg + 6;
#else
	hUChar *temp = m_recv_msg.msg ;
#endif



	int leng = temp[2];//遥测个数

	char fpath[256]; 
	ACE_OS::snprintf(fpath,256,"%s/cmddata/para.pdt",ACE_OS::getenv (SYS_ENV_VAR));

	FILE* fp = NULL;

	if( ACE_OS::filesize(fpath) > BLOG_FILE_LEN )	
		fp = ACE_OS::fopen(fpath,"w+");  
	else	
		fp = ACE_OS::fopen(fpath,"a+");

	if(fp == NULL)
	{
		printf("打开文件 %s 失败\n",fpath);
		return;
	}
	
	PDT::CCommInf	m_commInf;
	DAC_YCCON* ptr = m_commInf.yccon(2);

	//遥信
	bitset<16> bite;
	ACE_OS::fprintf(fp,"{\"time\":\"%u\",\"yx\":{", (hUInt32)ACE_OS::time(0));

	for(int i = 0; i < 2; i++)
	{
		bite = (temp[3+i*2]*256 + temp[4+i*2]);

		for(int j = 0; j < 16 ; j++)
		{
			if (ptr->ntype != 2)	break;
			
			ACE_OS::fprintf(fp,"\"%s\":\"%d\"",ptr->name, bite.test(j));
			if (i!=1 && j!= 15)
			{
				ACE_OS::fprintf(fp,",");
			}
			ptr++;
		}
	}
	ACE_OS::fprintf(fp,"},\"yc\":{",time(NULL));

	int ll = (leng-2)/2;

	for(int i = 0; i < ll ; i++)
	{
		if (ptr->ntype != 2)	break;
		ACE_OS::fprintf(fp,"\"%s\":\"%.2f\"",ptr->name, (float)MakeWord(temp[8+i*2],temp[7+i*2]));
		if (i!=ll-1)
		{
			ACE_OS::fprintf(fp,",");
		}
		ptr++;
	}
	ACE_OS::fprintf(fp,"}}",time(NULL));

	if ( fp != NULL ) ACE_OS::fclose(fp);
*/
}





void CProto_Modbus_101::updateErrRate()
{
	DAC_ROUTE_INFO* pRouteInfo = m_commInf.routeInfo(m_route);

	DAC_YC_DATA ycval;
	ycval.quality = 0;
	ycval.updateTime = CDateTime::currentDateTime().toTimeT();
	ycval.val = 0;

	if (pRouteInfo->txFrmCnt > 0)
	{
		//pRouteInfo->rxFrmCnt = pRouteInfo->txFrmCnt;
		//pRouteInfo->errFrmCnt = pRouteInfo->rxFrmCnt - pRouteInfo->okFrmCnt;
		hFloat  errRate = (hFloat)pRouteInfo->errFrmCnt/pRouteInfo->rxFrmCnt * 100;
		ycval.val = errRate;
	}	

	m_dataInf.setYc(m_route, POINTNO_ERRRATE, ycval);
}


bool CProto_Modbus_101::sendYcCommand(void* pFeature)
{
	if( pFeature == NULL ) return false;

	ctrl_pro_calldata*	pCom = (ctrl_pro_calldata*)pFeature;

	PMC_Feature_ptr p = m_polling_feature;
	int	 i = 0;
	hUChar cctype= YcCommand;
	QString str;
	/*
	 *elec		 0		电能数据
	 *Harmonic	 1		谐波数据
	 *Parameter	 2		装置参数
	 */
	switch(pCom->ctrlNo)
	{
	case 0:
		cctype = ElecCommand;
		str = "elec";
		break;
	case 1:
		cctype = HarmonicCmd;
		str = "harm";
		m_isExclusive = true;
		break;
	case 2:
		cctype = ParameterCmd;
		str = "para";
		break;
	default:
		break;
	}

	for( i = 0 ; i < PMC_FEATURE_RECORD_MAXNUM ; i ++ , p ++ )
	{
		if(!p->isUsed) continue;
		if(p->frmType == cctype) break;
	}

	if( i == PMC_FEATURE_RECORD_MAXNUM ) return false;
	
	char fpath[256]; 
	ACE_OS::snprintf(fpath,256,"%s/cmddata/%s.pdt",ACE_OS::getenv (SYS_ENV_VAR), str.toAscii().data());


	QFile f;
	f.remove(fpath);

	//if (p->frmType != HarmonicCmd)
	//{
	//	p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
	//}


	m_current_send_cmd.cmd[0] = m_remoteAddr;
	m_current_send_cmd.cmd[1] = p->fc;
	m_current_send_cmd.cmd[2] = p->addrH;
	m_current_send_cmd.cmd[3] = p->addrL;
	m_current_send_cmd.cmd[4] = p->lengthH;
	m_current_send_cmd.cmd[5] = p->lengthL;

	hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
	m_current_send_cmd.cmd[6] = HIBYTE(crc);
	m_current_send_cmd.cmd[7] = LOBYTE(crc);
	m_current_send_cmd.length = 8;

	p->ykptr.ctrlNo = pCom->ctrlNo;
	m_current_send_cmd.currFeatureptr = p;
	m_currPollCmdno = p->cmdNo;
	ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);

	return true;
}


/*

DAC_GROUP	*pGroup;
DAC_YC		*pYc;
CCommInf	m_commInf;
PDT::CDataInf	m_dataInf;
DAC_YC_DATA localData;
for(hUInt32 i=0;i<DAC_GROUP_NUM;i++)
{
	pGroup = m_commInf.group(i);
	if(!pGroup || !(pGroup->valid))	continue;
	for (hUInt32 j = 0; j < pGroup->ycNum; j++)
	{
		pGroup->code;	//组编码
		
		pYc = m_dataInf.ycPara(i, j);
		if (pYc)
		{
			pYc->code; //遥测code
				
		}
		m_dataInf.ycMainRoute(i,j,localData);
		localData.val;		//值
		localData.quality;	  //质量码
		localData.updateTime; //更新时间
	}
}
*/


//bool CProto_Modbus_101::sendElecCommand(void* pFeature)
//{
//	if( pFeature == NULL ) return false;
//
//	ctrl_pro_calldata*	pCom = (ctrl_pro_calldata*)pFeature;
//
//	PMC_Feature_ptr p = m_polling_feature;
//	int	 i;
//
//	for( i = 0 ; i < PMC_FEATURE_RECORD_MAXNUM ; i ++ , p ++ )
//	{
//		if(!p->isUsed) continue;
//		if(p->cmdNo == 1 && p->frmType == ElecCommand) break;
//	}
//
//	if( i == PMC_FEATURE_RECORD_MAXNUM ) return false;
//
//
//	p->nextCmdNo = m_current_send_cmd.currFeatureptr->nextCmdNo;
//
//
//	m_current_send_cmd.cmd[0] = m_remoteAddr;
//	m_current_send_cmd.cmd[1] = p->fc;
//	m_current_send_cmd.cmd[2] = p->addrH;
//	m_current_send_cmd.cmd[3] = p->addrL;
//	m_current_send_cmd.cmd[4] = p->lengthH;
//	m_current_send_cmd.cmd[5] = p->lengthL;
//
//	hUInt16 crc = CToolFunctions::modbusCRC(m_current_send_cmd.cmd, 6);
//	m_current_send_cmd.cmd[6] = HIBYTE(crc);
//	m_current_send_cmd.cmd[7] = LOBYTE(crc);
//	m_current_send_cmd.length = 8;
//
//	p->ykptr.ctrlNo = pCom->ctrlNo;
//	m_current_send_cmd.currFeatureptr = p;
//	m_currPollCmdno = p->cmdNo;
//	ACE_OS::strncpy(m_current_send_cmd.cmdStr, p->cmdDesc, PMC_DESCRIBE_STRING_LENGTH);
//
//	return true;
//}

void CProto_Modbus_101::resolve_callYcCommand(PMC_Feature_ptr p)
{
	ctrl_pro_calldata_ack  ack;
	memset(&ack,0,sizeof(ctrl_pro_calldata_ack));
	//ack.ctrlNo = p->ykptr.ctrlNo;//pYk->ctrlNo;
	ack.ackState = 0;
	ack.groupNo = m_grpno; 


	// 通讯管理机内数据都是通过转发到主站，所以遥控遥调以及命令下发的返回状态都需要返回到转发组去。
	// 需要有测点配置一个转发组的源组序号。

	DAC_YC *ycptr = NULL;
	DAC_ROUTE *pRoute = m_commInf.route(m_route);
	DAC_GROUP *pGroup = m_commInf.group( m_grpno); 
	for (hUInt32 pointNo = 0;pointNo < pGroup->ycNum ; pointNo++)
	{
		ycptr = m_dataInf.ycPara( pRoute->group,pointNo);  //根据数据组号和遥测号找到遥测表
		if (ycptr == NULL || !ycptr->valid)
		{
			ycptr = NULL;
			continue;
		}
		if (ycptr->srcGroup >= 0)
		{
			break;
		}
		ycptr = NULL;
	}

	if (ycptr)
	{
		ack.groupNo = ycptr->srcGroup;
	}
	
	int copyLen = sizeof(ctrl_pro_calldata_ack);
	if ( copyLen >= CTRL_COMMON_LEN ) copyLen = CTRL_COMMON_LEN;

	ctrl_pro_common_ack commonAck;
	commonAck.groupNo = ack.groupNo;

	commonAck.length = copyLen;
	ACE_OS::memcpy(commonAck.ackBuf,(char*)(&ack),copyLen);

	CDacCtrl m_dacCtrl; 
	if ( !m_dacCtrl.commonCmdAck( &commonAck ) ) 
	{
		logprint(LOG_PROTOCOL+m_route,"<group=%s,state=%d> oo遥调返校输出错误oo! 信号无法传递给后台",(int)(ack.groupNo),ack.ackState);
		return ;
	}
}


extern "C" MODBUS_101_EXPORT PDT::CProtocol * CreateProtocol()
{
	return ( new CProto_Modbus_101() );
}


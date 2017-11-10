/*===================================================================
*	模块名称：	csea
*	创建日期:	2013年11月27日 9:42
*	文 件 名: 	csea.h
*	文件路径:	E:\pdt3000\include\event
*	文件类型:	h
	
*	作    者:	wlq
*	版    本:	V2.0
	
*	模块说明:	事项结构基本定义

*	修改版本：	V2.1
*	修改作者：	wlq
*	修改日期：	27/11/2013
*	修改摘要：

====================================================================*/
#ifndef CSEA_H
#define CSEA_H

#include "csea_global.h"

class CSEA_EXPORT csea
{
public:
	csea();
	~csea();

private:

};


#include "utl/datatype.h"
#include "utl/string.h"
#include "utl/sharememorybase.h"
#include <ace/Process_Mutex.h>
#include <vector>

namespace PDT
{

#define EVENT_SHM_KEY		2010082001			//共享内存标识
/*
#define EVENT_SHM_DIR		"dat/map/sys"		//共享内存相对目录
#define	EVENT_SHM_FILE		"evnet.dat"			//共享内存映射文件
*/
	//事项结构中条目长度枚举定义
	enum EventNameLenth
	{
		SHORTNAME_LEN		=	32,
		NORMALNAME_LEN		=	32,
		LONGNAME_LEN		=	64,
		EVENTDESCRIBE_LEN	=	128,
		FULLDESCRIBE_LEN	=	256,
		EVENTMAX			=	2048			//wfp changed from 200
	};

	enum EventConfirmType
	{
		AllEvent		= 0,
		StationEvent,
		DeviceEvent,
		CodeEvent
	};

	//事项输出级定义
#define		PrintOut		m_output.b0
#define		DiskOut			m_output.b1
#define		ToneOut			m_output.b2			//语音输出
#define		GraphOut		m_output.b3
#define		DispOut			m_output.b4			//
#define		SmsOut			m_output.b5			//短信输出


	//事项版本
	enum EventVerionFlag
	{
		NOMAL_EVENT_FLAG = 0,		
		EX_EVENT_FLAG
	};

	/**事项类型例子
	* 1 进程事项：m_stationCode,m_stationName 可不填，m_code:进程名；m_name:进程描述；m_parentCode,m_parentName 可填主机，也可不填
	* 2 通道事项：m_stationCode,m_stationName 厂站内容，m_code:通道编码；m_name:通道描述；m_parentCode,m_parentName 可不填
	* 3 遥测事项 m_stationCode,m_stationName 厂站内容，m_code:测点编码；m_name:测点描述；m_parentCode,m_parentName 数据组编码，数据组描述
	*/
	//网络事项结构
	typedef struct
	{
		nUInt16		m_eventSort;					//0、事件分类号
		nUInt16		m_eventType;					//1、事件类型
		nUInt32		m_ymd;							//2、年月日
		nUInt32		m_hmsms;						//3、时分秒毫秒
		nUInt8		m_security;						//4、安全级
		nUInt8		m_voltLevel;					//5、电压等级
		nBit8		m_output;						//6、输出级
		nUInt16		m_stateValue;					//7、当前状态值(越限事项代表越限级别)
		nUInt16		m_stateValue1;					//8、上一次状态
		nFloat		m_floatValue;					//9、当前浮点值
		nFloat		m_floatValue1;					//10、上一次浮点值
		nUInt8		m_isRevert;						//11、是否复归(yc,yx)
		char		m_stationCode[SHORTNAME_LEN];	//12、厂站编码
		char		m_stationName[SHORTNAME_LEN];	//13、厂站名
		char		m_code[SHORTNAME_LEN];			//14、事件对象本身编码
		char		m_name[SHORTNAME_LEN];			//15、事件对象本身名称
		char		m_parentCode[SHORTNAME_LEN];	//16、事件对象父编码
		char		m_parentName[SHORTNAME_LEN];	//17、事件对象父名称
		char		m_graphName[NORMALNAME_LEN];	//18、推图名
		char		m_toneDesc[FULLDESCRIBE_LEN];	//19、语音描述信息
		char		m_charDesc[FULLDESCRIBE_LEN];	//20、文字描述信息
	}nSysEvent,*nSysEvent_ptr;

	//扩展的网络事项结构
	typedef struct {
		nSysEvent   m_evt;
		char		m_valueDesc[EVENTDESCRIBE_LEN];
		char		m_groupDesc[EVENTDESCRIBE_LEN];
		char		m_memberDesc0[EVENTDESCRIBE_LEN];
		char		m_memberDesc1[EVENTDESCRIBE_LEN];
		char		m_memberDesc2[EVENTDESCRIBE_LEN];
		char		m_otherDesc0[EVENTDESCRIBE_LEN];
		char		m_otherDesc1[EVENTDESCRIBE_LEN];
		char		m_otherDesc2[EVENTDESCRIBE_LEN];
		char		m_otherDesc3[EVENTDESCRIBE_LEN];
		char		m_otherDesc4[EVENTDESCRIBE_LEN];
		char		m_otherDesc5[EVENTDESCRIBE_LEN];
		char		m_longDesc[FULLDESCRIBE_LEN];
	}nSysEventEx,*nSysEventEx_ptr;

	//存到历史数据库中的事件结构
	typedef struct
	{
		short int		m_eventSort;					//0、事件分类号
		short int		m_eventType;					//1、事件类型
		int				m_ymd;							//2、年月日
		int				m_hmsms;						//3、时分秒毫秒
		unsigned char	m_security;						//4、安全级
		unsigned char	m_voltLevel;					//5、电压等级
		unsigned char	m_output;						//6、输出级
		short			m_stateValue;					//7.当前状态值(越限事项代表越限级别)
		short			m_stateValue1;					//8.
		float			m_floatValue;					//9.当前浮点值
		float			m_floatValue1;					//10.

		nUInt8			m_isRevert;						//11.是否复归(yc,yx)
		char			m_stationCode[SHORTNAME_LEN];	//12.厂站编码
		char			m_stationName[SHORTNAME_LEN];	//13.厂站名
		char			m_code[SHORTNAME_LEN];			//14.事件对象本身编码
		char			m_name[SHORTNAME_LEN];			//15.事件对象本身名称
		char			m_parentCode[SHORTNAME_LEN];	//16.事件对象父编码
		char			m_parentName[SHORTNAME_LEN];	//17.事件对象父名称
		char			m_charDesc[FULLDESCRIBE_LEN];	//18、文字描述信息
	}hSysEvent,*hSysEvent_ptr;

	//节点中事项集结构
	typedef struct
	{
		short int m_wrPtr;
		short int m_number;
		nSysEvent m_event[EVENTMAX];
	}nSysEventSet,*nSysEventSet_ptr;

	typedef struct
	{
		unsigned char cfmType;
		char		m_stationCode[SHORTNAME_LEN];	//1.厂站编码
		char		m_parentCode[SHORTNAME_LEN];	//2.事件对象父编码
		char		m_code[SHORTNAME_LEN];			//3.事件对象本身编码
		short int	m_eventType;					//4.事件类型
		char		m_confirmer[SHORTNAME_LEN];
	}hConfirmEvent,*hConfirmEvent_ptr;
	//确认事项结构
	typedef struct
	{
		short int m_wrPtr;
		short int m_number;
		hConfirmEvent	m_event[EVENTMAX];
	}hConfirmEventSet,*hConfirmEventSet_ptr;

	class CSEA_EXPORT CSysEventItem
	{
	public:
		CSysEventItem();
		CSysEventItem(const nSysEvent& a){ m_sysEventItem = a;}
		CSysEventItem(const nSysEvent_ptr a){ m_sysEventItem = *a; }
		~CSysEventItem();

		//整体赋值
		void setSysEventItem(const nSysEvent& a)	{ m_sysEventItem = a;}
		void setSysEventItem(const nSysEvent_ptr a) { m_sysEventItem = *a ;}
		//数值赋值
		void setEventSort(short int		m_eventSort	){ m_sysEventItem.m_eventSort = m_eventSort ;}
		void setEventType(short int		m_eventType	){ m_sysEventItem.m_eventType = m_eventType ;}
		void setYmd(int					m_ymd		){ m_sysEventItem.m_ymd		 = m_ymd		;}
		void setHmsms(int				m_hmsms		){ m_sysEventItem.m_hmsms	 = m_hmsms		;}
		void setSecurity(unsigned char	m_security	){ m_sysEventItem.m_security = m_security	;}
		void setVoltLevel(unsigned char	m_voltLevel	){ m_sysEventItem.m_voltLevel = m_voltLevel ;}
		void setOutput(nBit8 m_output				){m_sysEventItem.m_output = m_output		;}
		void setPrintOut(bool flag					){m_sysEventItem.PrintOut = flag			;}
		void setDiskOut(bool flag					){m_sysEventItem.DiskOut = flag				;}
		void setToneOut(bool flag					){m_sysEventItem.ToneOut = flag				;}
		void setGraphOut(bool flag					){m_sysEventItem.GraphOut = flag			;}
		void setDispOut(bool flag					){m_sysEventItem.DispOut = flag				;}
		void setStateValue(short	m_stateValue	){ m_sysEventItem.m_stateValue = m_stateValue	; }
		void setStateValue1(short	m_stateValue1	){ m_sysEventItem.m_stateValue1 = m_stateValue1 ; }
		void setFloatValue(float	m_floatValue	){ m_sysEventItem.m_floatValue = m_floatValue	; }
		void setFloatValue1(float	m_floatValue1	){ m_sysEventItem.m_floatValue1 = m_floatValue1 ; }

		//字符赋值
		//厂站
		void setStationCode	(const char* m_stationName);
		void setStationCode	(const CString& m_stationName);
		void setStationName	(const char* m_stationName);
		void setStationName	(const CString& m_stationName);
		//事件对象本身
		void setCode	(const char* m_code);
		void setCode	(const CString& m_code);
		void setName	(const char* m_name);
		void setName	(const CString& m_name);
		//事件对象父编码等
		void setParentCode	(const char* m_parentCode);
		void setParentCode	(const CString& m_parentCode);
		void setParentName	(const char* m_parentName);
		void setParentName	(const CString& m_parentName);
		//描述
		void setCharDesc	(const char* m_charDesc);
		void setCharDesc	(const CString& m_charDesc);

		nSysEvent & operator= ( const nSysEvent & other ){ return m_sysEventItem = other;}

		//获取事项条目
		nSysEvent sysEventItem() const { return m_sysEventItem ;}
		//nSysEvent_ptr sysEventItem() const { return &m_sysEventItem ;}

	private:
		nSysEvent	m_sysEventItem;
	};

	typedef std::vector<CSysEventItem>		CSysEventItemVector;
	//typedef std::vector<hConfirmEvent>	CConfirmEventVector;


	class CSEA_EXPORT CSysEvent : public CShareMemoryBase
	{
	public:
		CSysEvent();
		virtual ~CSysEvent();
	public:
		int					insertEvent(const nSysEvent& a);
		int					insertEvent(const CSysEventItem& a);			
		int                 insertEvent(const CSysEventItemVector& a);

		int					insertConfirmEvent(const hConfirmEvent& a);
		
		bool				fetchOneConfirmEvent(int readPos,hConfirmEvent& aa);
		bool				fetchOneConfirmEvent(hConfirmEvent& aa);

		//内部自行维护事项列表
		bool				hasEvent() const;
		bool				fetchOneEvent(nSysEvent& aa);
		bool				fetchOneEvent(CSysEventItem& aa);
		int					fetchEvent(CSysEventItemVector& aa);

		//由外部指定从哪里开始读取事项
		int					writePos() const;							//返回写指针位置
		int					writeCfmPos() const;							//返回确认写指针位置
		bool				hasEvent(int readPos) const;
		bool				fetchOneEvent(int readPos,nSysEvent& aa);
		bool				fetchOneEvent(int readPos,CSysEventItem& aa);
		int					fetchEvent(int readPos,CSysEventItemVector& aa);
	private:
		static nSysEventSet	*m_ptr;
		static hConfirmEventSet *m_cfmptr;
		ACE_Process_Mutex	m_processMutex;
		hUInt32				m_read;			//读指针
		hUInt32				m_cfmRead;
	};
}//end namespace



#endif // CSEA_H

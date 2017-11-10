/*===================================================================
*	ģ�����ƣ�	csea
*	��������:	2013��11��27�� 9:42
*	�� �� ��: 	csea.h
*	�ļ�·��:	E:\pdt3000\include\event
*	�ļ�����:	h
	
*	��    ��:	wlq
*	��    ��:	V2.0
	
*	ģ��˵��:	����ṹ��������

*	�޸İ汾��	V2.1
*	�޸����ߣ�	wlq
*	�޸����ڣ�	27/11/2013
*	�޸�ժҪ��

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

#define EVENT_SHM_KEY		2010082001			//�����ڴ��ʶ
/*
#define EVENT_SHM_DIR		"dat/map/sys"		//�����ڴ����Ŀ¼
#define	EVENT_SHM_FILE		"evnet.dat"			//�����ڴ�ӳ���ļ�
*/
	//����ṹ����Ŀ����ö�ٶ���
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

	//�������������
#define		PrintOut		m_output.b0
#define		DiskOut			m_output.b1
#define		ToneOut			m_output.b2			//�������
#define		GraphOut		m_output.b3
#define		DispOut			m_output.b4			//
#define		SmsOut			m_output.b5			//�������


	//����汾
	enum EventVerionFlag
	{
		NOMAL_EVENT_FLAG = 0,		
		EX_EVENT_FLAG
	};

	/**������������
	* 1 �������m_stationCode,m_stationName �ɲ��m_code:��������m_name:����������m_parentCode,m_parentName ����������Ҳ�ɲ���
	* 2 ͨ�����m_stationCode,m_stationName ��վ���ݣ�m_code:ͨ�����룻m_name:ͨ��������m_parentCode,m_parentName �ɲ���
	* 3 ң������ m_stationCode,m_stationName ��վ���ݣ�m_code:�����룻m_name:���������m_parentCode,m_parentName ��������룬����������
	*/
	//��������ṹ
	typedef struct
	{
		nUInt16		m_eventSort;					//0���¼������
		nUInt16		m_eventType;					//1���¼�����
		nUInt32		m_ymd;							//2��������
		nUInt32		m_hmsms;						//3��ʱ�������
		nUInt8		m_security;						//4����ȫ��
		nUInt8		m_voltLevel;					//5����ѹ�ȼ�
		nBit8		m_output;						//6�������
		nUInt16		m_stateValue;					//7����ǰ״ֵ̬(Խ���������Խ�޼���)
		nUInt16		m_stateValue1;					//8����һ��״̬
		nFloat		m_floatValue;					//9����ǰ����ֵ
		nFloat		m_floatValue1;					//10����һ�θ���ֵ
		nUInt8		m_isRevert;						//11���Ƿ񸴹�(yc,yx)
		char		m_stationCode[SHORTNAME_LEN];	//12����վ����
		char		m_stationName[SHORTNAME_LEN];	//13����վ��
		char		m_code[SHORTNAME_LEN];			//14���¼����������
		char		m_name[SHORTNAME_LEN];			//15���¼�����������
		char		m_parentCode[SHORTNAME_LEN];	//16���¼����󸸱���
		char		m_parentName[SHORTNAME_LEN];	//17���¼���������
		char		m_graphName[NORMALNAME_LEN];	//18����ͼ��
		char		m_toneDesc[FULLDESCRIBE_LEN];	//19������������Ϣ
		char		m_charDesc[FULLDESCRIBE_LEN];	//20������������Ϣ
	}nSysEvent,*nSysEvent_ptr;

	//��չ����������ṹ
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

	//�浽��ʷ���ݿ��е��¼��ṹ
	typedef struct
	{
		short int		m_eventSort;					//0���¼������
		short int		m_eventType;					//1���¼�����
		int				m_ymd;							//2��������
		int				m_hmsms;						//3��ʱ�������
		unsigned char	m_security;						//4����ȫ��
		unsigned char	m_voltLevel;					//5����ѹ�ȼ�
		unsigned char	m_output;						//6�������
		short			m_stateValue;					//7.��ǰ״ֵ̬(Խ���������Խ�޼���)
		short			m_stateValue1;					//8.
		float			m_floatValue;					//9.��ǰ����ֵ
		float			m_floatValue1;					//10.

		nUInt8			m_isRevert;						//11.�Ƿ񸴹�(yc,yx)
		char			m_stationCode[SHORTNAME_LEN];	//12.��վ����
		char			m_stationName[SHORTNAME_LEN];	//13.��վ��
		char			m_code[SHORTNAME_LEN];			//14.�¼����������
		char			m_name[SHORTNAME_LEN];			//15.�¼�����������
		char			m_parentCode[SHORTNAME_LEN];	//16.�¼����󸸱���
		char			m_parentName[SHORTNAME_LEN];	//17.�¼���������
		char			m_charDesc[FULLDESCRIBE_LEN];	//18������������Ϣ
	}hSysEvent,*hSysEvent_ptr;

	//�ڵ�������ṹ
	typedef struct
	{
		short int m_wrPtr;
		short int m_number;
		nSysEvent m_event[EVENTMAX];
	}nSysEventSet,*nSysEventSet_ptr;

	typedef struct
	{
		unsigned char cfmType;
		char		m_stationCode[SHORTNAME_LEN];	//1.��վ����
		char		m_parentCode[SHORTNAME_LEN];	//2.�¼����󸸱���
		char		m_code[SHORTNAME_LEN];			//3.�¼����������
		short int	m_eventType;					//4.�¼�����
		char		m_confirmer[SHORTNAME_LEN];
	}hConfirmEvent,*hConfirmEvent_ptr;
	//ȷ������ṹ
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

		//���帳ֵ
		void setSysEventItem(const nSysEvent& a)	{ m_sysEventItem = a;}
		void setSysEventItem(const nSysEvent_ptr a) { m_sysEventItem = *a ;}
		//��ֵ��ֵ
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

		//�ַ���ֵ
		//��վ
		void setStationCode	(const char* m_stationName);
		void setStationCode	(const CString& m_stationName);
		void setStationName	(const char* m_stationName);
		void setStationName	(const CString& m_stationName);
		//�¼�������
		void setCode	(const char* m_code);
		void setCode	(const CString& m_code);
		void setName	(const char* m_name);
		void setName	(const CString& m_name);
		//�¼����󸸱����
		void setParentCode	(const char* m_parentCode);
		void setParentCode	(const CString& m_parentCode);
		void setParentName	(const char* m_parentName);
		void setParentName	(const CString& m_parentName);
		//����
		void setCharDesc	(const char* m_charDesc);
		void setCharDesc	(const CString& m_charDesc);

		nSysEvent & operator= ( const nSysEvent & other ){ return m_sysEventItem = other;}

		//��ȡ������Ŀ
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

		//�ڲ�����ά�������б�
		bool				hasEvent() const;
		bool				fetchOneEvent(nSysEvent& aa);
		bool				fetchOneEvent(CSysEventItem& aa);
		int					fetchEvent(CSysEventItemVector& aa);

		//���ⲿָ�������￪ʼ��ȡ����
		int					writePos() const;							//����дָ��λ��
		int					writeCfmPos() const;							//����ȷ��дָ��λ��
		bool				hasEvent(int readPos) const;
		bool				fetchOneEvent(int readPos,nSysEvent& aa);
		bool				fetchOneEvent(int readPos,CSysEventItem& aa);
		int					fetchEvent(int readPos,CSysEventItemVector& aa);
	private:
		static nSysEventSet	*m_ptr;
		static hConfirmEventSet *m_cfmptr;
		ACE_Process_Mutex	m_processMutex;
		hUInt32				m_read;			//��ָ��
		hUInt32				m_cfmRead;
	};
}//end namespace



#endif // CSEA_H

/*==============================================================================
* �ļ����� : middlelayer.h
* ģ�� : �м���Լ����
* ����ʱ�� : 2010-11-09 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_BFS_DAC_MIDDLELAYER_H_
#define	_BFS_DAC_MIDDLELAYER_H_

#include "dac/bufferext.h"
#include "dac/protocol.h"
#include "dac/toolbase.h"
#include "utl/intervaltimerset.h"
#include "sys/blog.h"
#include <QtCore/QtCore>

//�����궨��--------------begin-------
#define LOG_PRO_BASE		20000		//��־�������
//�����궨��--------------end---------

//ELBЭ����غ궨��-------begin-------
//��׼ͬ��ͷ�ֽ�
#define SYNC_HEAD_BYTE1		0xeb
#define SYNC_HEAD_BYTE2		0x90

//��׼ͬ��β�ֽ�
#define SYNC_TAIL_BYTE		0x7e

//��ע��ELB��������
#define ELB_CMD_REG			0xe6		//ע��
#define ELB_CMD_DAT			0xe3		//����

//��ELB����ע������ļ�����룩
#define ELB_REG_INTERVAL	10*60
//ELB��С֡��
#define ELB_MIN_FRM_LEN		15

//ELBЭ����غ궨��-------end---------


namespace BFS
{
	class CMiddleLayer : public CProtocol
	{
	public:
		CMiddleLayer();
		virtual ~CMiddleLayer();

		//�Ƿ�򿪣�ע��������ʵ�ֱ������Ĵ���ǰ�ȵ��������isOpen() ��
		virtual	bool	isOpen() const;
		//�򿪹�Լ��ע��������ʵ�ֱ������Ĵ���ǰ�ȵ��������open() ��
		virtual bool	open();
		//�رչ�Լ��ע��������ʵ�ֱ������Ĵ�����ٵ��������close() ��
		virtual void	close();
		//���մ�����ڣ�ע��������ʵ�ֱ������Ĵ���ǰ�ȵ��������mainRx() ��
		virtual void	mainRx();
		//���ʹ�����ڣ�ע��������ʵ�ֱ������Ĵ���ǰ�ȵ��������mainTx() ��
		virtual void	mainTx();

	protected://�м�Э�鹫�������뺯��
		bool				m_isOpen;			//�Ƿ�򿪱�־
		CBufferExt			m_rxDataBuf;		//ʵ��Э�����ݻ�����
		DAC_ROUTE*			m_pRoute;			//routeָ��
		DAC_ROUTE_INFO*		m_pRouteInfo;		//routeInfoָ�� 
		DAC_CHANNEL*		m_pChannel;			//channelָ��
		DAC_CHANNEL_INFO*	m_pChannelInfo;		//channelinfoָ��
		CIntervalTimerSet	m_timeSet;			//ʱ�䶨ʱ����

		//�Ƿ�����������
		bool	canSend() const;
		//��������
		bool	sendData(const hUChar *buf,hUInt32 len);
		///��ʾ����--֧����ʾ�����ȵı���
		void	showMsg(hUInt8 type,const char* desc,const hUChar* data,hUInt32 len);

	private://�м�Э�鹫�������뺯��
		bool				m_canSend;			//�Ƿ���Է�������
		CBufferExt			m_rxMiddleBuf;		//�м�Э�����ݻ�����
			
		//����ͬ��ͷ
		int		findSyncHeader(hUChar* buf,int len,int startPos,int& foundBytes);
		//���ֽ���תС�ֽ���int
		void	bigEndToLittleEndInt(hUChar* buf,int len,int& retVal);
		//���ֽ���תС�ֽ���float
		void	bigEndToLittleEndFloat(hUChar* buf,float& retVal);

	private://ELBЭ������뺯��	
		int				m_elbTimeIDReg;			//elbע��֡��ʱ����ʶ
		QString			m_elbPara;				//elb����
		int				m_elbCom;				//elb���ں�

		//����elb����
		bool	elbParsePara(const char* para);
		//����elbע��֡
		void	elbTxReg();
		//����elb����֡
		bool	elbTxData(const hUChar* buf,int len);
		//����elb����
		int		elbParseData();

	private://����Э������뺯��
		
	private://���Э������뺯��
		
	};
}

#include "middlelayer.inl"

#endif //_BFS_DAC_MIDDLELAYER_H_

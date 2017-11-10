/*==============================================================================
* �ļ����� : protocol.h
* ģ�� : ��Լ����
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_PDT_DAC_PROTOCOL_H_
#define	_PDT_DAC_PROTOCOL_H_

#include "dac/comminf.h"
#include "dac/datainf.h"
#include "dac/buffer.h"
#include "dac/message.h"
#include "dac/ctrlinf.h"
#include "dac/dacctrldef.h"

#define	PROTO_ONESEC_TIMES	(1000000/DAC_PROTO_RUN_DELAY)		//��Լÿ��ĵ��ȴ���

namespace PDT
{
		//ͨ���������ͣ���Ӧchannel��f_datatype�ֶ�(wfp add at 20100409)
		enum PRO_CHAN_DATATYPE
		{
			DATATYPE_NORMAL		=	0,			//��ͨ
			DATATYPE_EII		=	1,			//EII
			DATATYPE_HONGDIAN	=	2			//���
		};

		class DAC_EXPORT CProtocol
		{
		public:
			CProtocol();
			virtual ~CProtocol();
			//�Ƿ��
			virtual	bool	isOpen() const = 0;
			//�򿪹�Լ
			virtual bool	open() = 0;
			//�رչ�Լ
			virtual void	close() = 0;
			//���մ������
			virtual void	mainRx() = 0;
			//���ʹ������
			virtual void	mainTx() = 0;

			hBool			setTimeOut();
			void			setTimeOut(hBool bTimeOut){m_bTimeOut = bTimeOut;}

			hBool			isExclusive();


		public:
			//��ʼ��ʱ����
			bool			init(hInt32 channel,hInt32 route);
		protected:
			virtual	bool	readFeature();

			//one channel -- multi routes 
			/**
			* @function			����Լ��Ҫ�յ���Ӧ��������У���ʹ�øú�����������
			* @param isSend		true:���������ڷ��ͣ�false:��
			* @simple example	mainTx()������ʼλ��,if( !isSending() ) return;
			*/
			hBool			isSending() const;
			/**
			* @function			mainTx()�б����Ƿ�������������
			* @param isSend		true:����������ͣ�false:������
			*/
			void			reportCmdSend(hBool isSend = true);	
			/**
			* @function			mainRx()�б����յ���������Ļ�У����
			* @param isOk		true:������ȷ��false:���ݴ���
			*/
			void			repotrAckRecv(hBool isOk = true);

			//���ջ�����
			bool			get(hUChar& val);
			hUInt32			get(hUChar *buf,hUInt32 len);
			hUInt32			length()	const;		//�����ֽ���
			bool			back(hUInt32 num);		//����ƶ���ǰ��ָ��

			//���ͻ�����
			bool			put(hUChar val);
			bool			put(const hUChar *buf,hUInt32 len);
			hUInt32			remain();				//ʣ��ռ䳤��

			//���֡����
			bool			msgCopy(const hUChar* buf,hUInt32 len);
			//�Ӻ���ǰɾ��֡����
			bool			msgRemove(hUInt32 len);
			//���֡����
			void			msgClean();
			//��ʾ��ϵ�֡����
			bool			msgDisplay(hUInt8 type,const char* desc); 
			/**
			* @function			����������ʾ
			* @param type		֡���ͣ�����ֵ�ο�FRAME_TYPE
			* @param desc		֡��������������64�ֽ�
			* @param data		֡���ݣ���������1024�ֽ�
			* @param len		֡����
			* @retval			true:�ɹ���false:ʧ��
			*/
			bool			msgDisplay(hUInt8 type,const char* desc,const hUChar* data,hUInt32 len);

			//�޸�·����Ϣ֡����
			/**
			* @function			����·���ķ���֡����
			* @param frmNum		��Ҫ���ӵ�֡��Ŀ
			* @retval			true:�ɹ���false:ʧ��
			*/
			bool			addSendFrmNum(int frmNum=1);
			/**
			* @function			����·���Ľ�����ȷ֡����
			* @param frmNum		��Ҫ���ӵ���ȷ֡��Ŀ
			* @retval			true:�ɹ���false:ʧ��
			*/
			bool			addRecvOkFrmNum(int frmNum=1);
			/**
			* @function			����·���Ľ��մ���֡����
			* @param frmNum		��Ҫ���ӵĴ���֡��Ŀ
			* @retval			true:�ɹ���false:ʧ��
			*/
			bool			addRecvErrFrmNum(int frmNum=1);

		protected:	//���ƽӿ�
			/**
			* @function			��ȡ��ǰ·���Ĺ�Լ��������
			* @param cmdBuf		in&out������ָ��
			* @param len		in����������
			* @retval			true:�ɹ���false:ʧ��
			*/
			bool			getCtrlCmd(char* cmdBuf,int len);
			bool			getCtrlCmdByRoute(hInt32 routeno,char* cmdBuf,int len);
			/**
			* @function			���ɵ�ǰ·���Ĺ�Լ���Ʒ�У����
			* @param ackBuf		in������ָ��
			* @param len		in���������ݳ���
			* @retval			true:�ɹ���false:ʧ��
			*/
			bool			setCtrlCmdAck(const char* ackBuf,int len);
			bool			setCtrlCmdAckEx(const char* ackBuf,int len);


		private:
			//ͬCDacCtrl.commonCmdAck()����
			bool			commonCmdAck(const ctrl_pro_common_ack* pCommonAck);
		protected:
			CCommInf		m_commInf;
			CDataInf		m_dataInf;
			CCtrlInf		m_ctrlInf;
			hInt32			m_route;		//·����
			hInt32			m_channel;		//ͨ����

			hBool			m_bTimeOut;
			hBool			m_isExclusive;		//�Ƿ���Ҫ��ռ��ͨ��������������������·���

		private:
			hUChar			m_msg[DAC_MSG_DATA_LEN];
			hUInt32			m_msgLen;
			CBuffer			m_rxBuffer;
			CBuffer			m_txBuffer;
			CMessage		m_message;
			hBool			m_isSending;		//�����������У��ȴ���ʱ
		};
}
#endif //_PDT_DAC_PROTOCOL_H_

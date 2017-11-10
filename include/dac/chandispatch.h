/*==============================================================================
* �ļ����� : chandispatch.h
* ģ�� : ǰ��ͨ�����ݷ����ӿ�
* ����ʱ�� : 2009-10-16 15:55:23
* ���� : ����ƽ
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_BFS_DAC_CHANDISP_H_
#define	_BFS_DAC_CHANDISP_H_

#include <ace/config-all.h>
//=============================================================================
#if defined (_DAC_CHANDISP_)
#  define	CHANDISP_EXPORT	ACE_Proper_Export_Flag
#else
#  define	CHANDISP_EXPORT	ACE_Proper_Import_Flag
#endif	// _DAC_CHANDISP_
//=============================================================================
#include "utl/datatype.h"
#include "dac/dacdef.h"
#include "dacmacro.h"
#include "dac/comminf.h"
#include <ace/OS.h>
#include <ace/Task.h>
#include "dac/cyclebuffer.h"
#include <utl/intervaltimerset.h>
#include "dac/buffer.h"

namespace BFS
{
	enum DISP_LOG_TYPE
	{
		LOG_DISP_BASE			=	3000,		//base��־ID
		LOG_DISP_DATA_PUT		=	3001,		//���ݴ���+����
		LOG_DISP_DATA_GET		=	3002,		//���ݻ�ȡ
		LOG_DISP_DATA_ERR		=	3003,		//���Ĵ�����־
		LOG_DISP_DATA_SHOW		=	3004,		//���������еı�����ʾ
		LOG_DISP_DATA_RX_SHOW	=	3005,		//�����RX������ʾ
		LOG_DISP_DATA_TX_SHOW	=	3006		//�����TX������ʾ
		
	};

	#define	DISP_MAX_BUFFER_LEN	10240		//10k 

	//֡����
	#define FRAM_START			0x68		//֡��ʼ��
	#define FRAM_END			0x16		//֡������
	//������
	#define FRAM_CTRL_REG		0x21		//��¼��
	#define FRAM_CTRL_REG_ACK	0xA1		//��¼��У

	#define FRAM_CTRL_UNREG		0x22		//��¼�˳���
	#define FRAM_CTRL_UNREG_ACK	0xA2		//��¼�˳���У

	#define FRAM_CTRL_HEART		0x24		//������
	#define FRAM_CTRL_HEART_ACK	0xA4		//������У

	class CHANDISP_EXPORT CChanDispTask : public ACE_Task<ACE_NULL_SYNCH>
	{
	public:
		CChanDispTask();
		virtual ~CChanDispTask();
	public:
		//����վַ+����
		bool	init(int localaddr,const char* pwd);
		int		start();
		void	stop();
	protected:
		//���������
		int		svc();
		void	mainRx();
		bool	checkSUMC(const hUChar *buf,int len);
		nUInt8	makeSUMC(const nUInt8 *buf,int len);
		int		checkSerialNo(const hUChar* buf,int len);
		bool	putToChannel(int channel,const hUChar* buf,int len);
		bool	isLocalCtrlCode(hUInt32 ctrlCode);
		bool	checkLocalSerialNo(const hUChar* buf,int len);
		bool	procLocalHeartAck();
		bool	procLocalRegisterAck();
		//ͨ��·���Ȳ���+��Ϣ����
		void	startChannel(int channel);
		void	stopChannel(int channel);
		void	stopAllChannel();
		void	txBytesChannel(int channel,int num);
		bool	addRecvOkFrmNum(int channel,int frmNum=1);
		//wfp temp add
		void	setRemoteRegisterAck(const hUChar* buf);
	public:
		int		putData(const hUChar* data,int length);
		int		getData(hUChar* data,int size);
		int		getLocalHeartData(hUChar* data,int size);
		int		getLocalRegisterData(hUChar* data,int size);
		int		getLocalUnRegisterData(hUChar* data,int size);
		bool	checkLocalHeart();
		bool	isLocalRegistered();
		bool	goClose();
		void	displayData(int logId,const char* descr,const hUChar* buf,int len);
		//wfp temp add
		int		getRemoteRegisterData(hUChar* data,int size);
	private:
		//˵���� localָ���ؿͻ��������ط���֮��
		CCommInf			m_commInf;
		CCycleBuffer		m_buffer;
		CBuffer				m_rxBuffer[DAC_CHANNEL_NUM];			//ͨ�����ջ�����
		CBuffer				m_txBuffer[DAC_CHANNEL_NUM];			//ͨ�����ͻ�����
		bool				m_isLocalRegisterd;						//���ؿͻ����Ƿ������ط����¼�ɹ�
		int					m_localHeartAck;						//������У����
		int					m_curChannel;							//��ǰ����ͨ�������

		CIntervalTimerSet	m_timer;
		int					m_timeId;

		int					m_localAddr;		//����վַ
		char				m_pwd[DAC_PWD_LEN];	//���ط����¼����

		//wfp temp add at 20100105
		nUInt8				m_regAckBuffer[32];//��ʱ���ӵĵ�ѹ���¼��У
		bool				m_isNeedSendRegAck;//��ʱ���ӵĵ�ѹ���¼��У

	};

	typedef	ACE_Singleton<CChanDispTask,ACE_SYNCH_NULL_MUTEX>	CHANDISPTASK;

}

#endif	//_BFS_DAC_CHANDISP_H_

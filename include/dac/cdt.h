/*==============================================================================
* �ļ����� : cdt.h
* ģ�� :     CDT��׼��Լ
* ����ʱ�� : 2009-08-18
* ���� : lj
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef   _BFS_DAC_CDT_H_
#define  _BFS_DAC_CDT_H_

#include "dac/protocol.h"
#include <ace/config-all.h>
#include "utl/intervaltimerset.h"
#include <QString>

//=============================================================================
#if defined (_CDT_)
# define	CDT_EXPORT	ACE_Proper_Export_Flag
#else
#  define	CDT_EXPORT	ACE_Proper_Import_Flag
#endif
//=============================================================================

namespace BFS
{

    #define ELBHEAD_LENGTH      6		//elbͷ����
    #define ELBEND_LENGTH		1		//elbβ����
	#define	BYTE_LENGTH			2
    #define LOG_DLL				20000   //��־��
    #define	MAX_CMD_COUNT		16		//�����������
    #define MAX_LENGTH          64  
    #define CMD_LENGTH          256     //������󳤶�
    #define MAX_BUF_SIZE        2048	//�������ݻ��泤��


	#define DTB_CMD_HEAD1		0xEB	//ELBЭ�鿪ͷ
	#define DTB_CMD_HEAD2		0x90
	#define DTB_CMD_TAIL		0x7E	//ELBЭ���β
	#define SC_REGISTER_CMD_1	0xE6	//EIS��sc��ע������
	#define SC_TRANSFER_CMD		0xE3	//ת������

	#define  TYPE_10			0		//���������� 10����
	#define  TYPE_16			1		//16����
	#define  TYPE_8				2		//8����

    #define  TYPE_YCPARA_STD	0		//��׼cdt
	#define  TYPE_YCPARA_LH		1		//��λ��ǰ
	#define  TYPE_YCPARA_HL		2		//��λ��ǰ
  
	#define  YCIMPBEGINNO		0
	#define  YCSUBBEGINNO		100
	#define  YCCOMBEGINNO		200

	#define  ELBRETTIME			10*60	//10���Ӷ�ʱע��һ��


	typedef struct _sent_cmd_
	{
		unsigned char cmd[CMD_LENGTH];		//����                                             
		unsigned int  length;				//����� 
		char		  cmdstr[CMD_LENGTH];	//��������
	}SENT_CMD, *SENT_CMD_PTR;

	typedef struct rcv_cmd
	{
		hUChar cmd[MAX_BUF_SIZE];
		hUInt16 length;
	}RCV_CMD, *RCV_CMD_PTR;

	typedef struct cdt_feature   //modbus��Լ�����ṹ
	{
		hUInt16 ycfuntiontype;      //���������� 0����ʾ10���� 1����ʾ16���� 2:��ʾ8����
		hUInt16 ycparadtype;	    //������ݽ������ͣ�0����׼cdt��ʽ��Ĭ�ϣ�1���ߵ�λ����λ��ǰ����λ�ں� 2���ߵ�λ����λ��ǰ����λ�ں�
		hUInt16 ycimpbeginno;		//��Ҫң�⿪ʼ���	
		hUInt16 ycsubbeginno;		//��Ҫң�⿪ʼ���
		hUInt16 yccombeginno;		//��ͨң�⿪ʼ���
	}CDT_FEATURE, *CDT_FEATURE_PTR;


	class CDT_EXPORT CCdt:public CProtocol
	{
		public:
			CCdt();
			 ~CCdt();
			virtual bool	isOpen() const;														//�Ƿ��
			virtual bool	open();																//�򿪹�Լ
			virtual void	close();															//�رչ�Լ
			virtual void	mainRx();															//���մ������
			virtual void	mainTx();															//���ʹ������

		protected:
			virtual	bool	readFeature();														//���ع�Լ��
			virtual void	buildRegCmd();														//ע������
			virtual int 	buildCommand();														//��������,������ʵ�ַ�����������
			virtual int		paraDevData(hUChar *buff, int nLen) = 0;							//�����豸����Э��
			
			int				getCmdCount(); 
	    private:		
			void			setRegInfo();														//дע����������ڴ�	
			void			setCmdinfoToBuf(hUInt16 cursentfrm);
			QString			getEiiPara(int &com, const char *para);								//��ȡelb������Ϣ

		    void			buildElbRetCmd();                                                   //ebl��ע������
			void			buildELBProtocol(hUChar *byteCmd,int &RtnLeng, hUChar *buff, int nLen);//���elbЭ��
			
			int				handleData(hUChar *buff, hUInt16 nLen);			//��������
			void            paraElbProtocol(hUChar *byteCmd, hUInt16 &leng);//����elbЭ��
			void            paraDevProtocol();								//�����豸Э�� 
			void			putData(hUChar *buff, hUInt16 leng);            //�����ݷ��빲������
	
		protected:
			SENT_CMD		m_SendCmd[MAX_CMD_COUNT];		//����ͻ���
			SENT_CMD		m_RetCmd;						//ע������
			
			CDT_FEATURE     m_feature;						//��Լ�����ṹ										

			DAC_ROUTE_INFO	*m_prouteinfo;					//·�������ڴ�
			DAC_ROUTE		*m_proute;						//·������
			DAC_CHANNEL		*m_pchannel;					//ͨ��
			DAC_CHANNEL_INFO *m_pchannelinfo;				//ͨ����Ϣ
		
		private:
			RCV_CMD					m_RcvCmd;				//�������ݻ���
			BFS::CIntervalTimerSet	m_timeSet;				//ʱ�䶨ʱ��
			QString					m_elbPara;				//elbͨ�Ų�����Ϣ
		    int						m_icom;					//���ں�
			int						m_timeAllDataID;		//ȫ����ʱ��id
			int						m_timeRetID;			//ע��ʱ��id;
			hUInt16					m_iCmdCount;			//�ܷ�����������
			hUInt16					m_iCurCmdFrm;			//��ǰ������
			bool					m_isOpen;				//��Լ�Ƿ��
			hUChar                  m_elbhead[ELBHEAD_LENGTH];
			hUChar                  m_elbend[ELBEND_LENGTH];
	
	};
}
#endif//_BFS_DAC_MODBUS_H_��Լ��





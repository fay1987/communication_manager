#ifndef	_BFS_EVENTTYPE_
#define	_BFS_EVENTTYPE_

enum	EventSort
{
	EVENT_SORT_SYS=1,
	EVENT_SORT_OPT,
	EVENT_SORT_DEVICE_OPT,
	EVENT_SORT_RUN,
	EVENT_SORT_YC,
	EVENT_SORT_YX,
	EVENT_SORT_SOE
};
enum	EventType
{
	EVENT_UNKOWN=0,			//δ����

	//ϵͳ���1����100��
	EVENT_SYS=1,
	EVENT_SYS_SERVER=2,		//������
	EVENT_SYS_HOST,			//����
	EVENT_SYS_PROC,			//����
	EVENT_SYS_MEM,			//�ڴ�
	EVENT_SYS_CPU,			//CPU
	EVENT_SYS_DISK,			//DISK
	EVENT_SYS_MAIL,			//�ʼ�
	EVENT_SYS_FILE,			//�ļ�

	//�������100����199��
	EVENT_OPT = 100,
	EVENT_OPT_SERVER,		//������
	EVENT_OPT_HOST,			//����
	EVENT_OPT_PROC,			//����
	EVENT_OPT_FILE,			//�ļ�
	EVENT_OPT_PARA,			//װ�ز���
	EVENT_OPT_CHANNEL,		//ͨ��
	EVENT_OPT_GROUP,		//������
	EVENT_OPT_ROUTE,		//·��
	EVENT_OPT_YC,			//YC
	EVENT_OPT_YX,			//YX
	EVENT_OPT_KWH,			//KWH
	EVENT_OPT_PALN,			//PLAN
	EVENT_OPT_YK,			//yaokong
	EVENT_OPT_YT,			//yaotiao

	//�豸��������(200--299)
	EVENT_DEVICE_OPT=200,	//��ͨ�豸��������
	EVENT_DEVICE_OPT_DTFD,	//��̬�ŵ�
	EVENT_DEVICE_OPT_JTFD,	//��̬�ŵ�

	//��������(300-399)
	EVENT_RUN=300,			//��ͨ��������
	EVENT_RUN_NET,			//��������
	EVENT_RUN_SERVER,		//����������
	EVENT_RUN_PROC,			//��������
	EVENT_RUN_CHANNEL,		//ͨ������
	EVENT_RUN_GROUP,		//����������
	EVENT_RUN_ROUTE,		//·������

	//YC����(500-599)
	EVENT_YC=501,			//YCԽ��
	EVENT_YC_I,             //�������ϸ�
	EVENT_YC_U,             //��ѹ���ϸ�
	
	

	//YX����(1000-1999)
	EVENT_YX=1000,			//YX��ͨ����
	EVENT_YX_GZZT=1001,		//����״̬
	EVENT_YX_YCZT,			//�쳣״̬
	EVENT_YX_KGBW=1010,		//����������λ
	EVENT_YX_KGBH,			//���ؿ���״̬
	EVENT_YX_KGSG,			//�����¹ʱ�λ
	EVENT_YX_DZBW,			//��բ��λ
	//��ѹ״̬
	EVENT_YX_DYZT=1020,		//��ѹ״̬
	EVENT_YX_DYQY,			//��ѹǷѹ״̬
	EVENT_YX_DYGY,			//��ѹ��ѹ״̬
	EVENT_YX_DYGQY,			//��ѹ��Ƿѹ״̬
	//���״̬
	EVENT_YX_DCZT=1030,		//���״̬
	//�豸״̬
	EVENT_YX_SBZW=1040,		//�豸��λ״̬
	EVENT_YX_SBTQ,			//�豸��Ͷ״̬
	
	EVENT_YX_YXZT=1050,		//����״̬

	EVENT_YX_KGJZT=1052,	//���ػ�״̬
	EVENT_YX_DZZT,			//����״̬

	//����
	EVENT_YX_JLSD=1060,		//����ʧ��
	EVENT_YX_JLQX,			//����ȱ��
	EVENT_YX_RDQZT,			//�۶���״̬
	EVENT_YX_XLJD,			//��·�ӵ�״̬
	EVENT_YX_XLSY,			//��·ʹ��״̬

	EVENT_YX_SDZT=2000,		//�е�״̬



	//���ݿ����4000����4999��
	EVENT_HISDB = 4000,

	//ʵʱ�����5000����5999��
	EVENT_MEMDB = 5000,

	//���Ʒ��������6000����6999��
	EVENT_CTLSRV = 6000,

	//ת���ļ�֪ͨ����
	EVENT_TRANS_FILE = 10000,

	//ͨ���¼���30000����30999��
	EVENT_NORMAL = 30000
};

#endif

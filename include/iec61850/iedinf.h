#ifndef __IEC61850_IEDINF_H__
#define __IEC61850_IEDINF_H__

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------
//�����ڴ����ݽṹ
//----------------------------------------------------------------
//�����ڴ��ֵ
#define	SCL_SHM_KEY		18931226

//��������С����
#define SCL_GROUP_MAX	256		//������
#define SCL_AI_MAX		64		//ң��
#define SCL_DI_MAX		32		//ң��
#define SCL_EI_MAX		32		//���
#define SCL_SI_MAX		32		//�ַ���
#define SCL_TI_MAX		32		//ʱ��
#define SCL_YK_MAX		32		//ң��
#define SCL_YK_ACK_MAX	32		//ң��Ӧ��

//�ַ������ֵ��󳤶�
#define SCL_SI_VAL_LEN_MAX		128	

//����������С
#define SCL_CHG_AI_MAX	(2*SCL_AI_MAX)		//��ң��
#define SCL_CHG_DI_MAX	(SCL_DI_MAX)		//��ң��
#define SCL_CHG_EI_MAX	(SCL_EI_MAX)		//����
#define SCL_CHG_SI_MAX	(SCL_SI_MAX)		//���ַ���
#define SCL_CHG_TI_MAX	(SCL_TI_MAX)		//��ʱ��


//������(����ռλ)
//����ʹ�ú�  BSTR_BSTR_SET_ON��BSTR_BIT_SET_OFF��BSTR_BIT_GET 
//����Ӧ��BitString��������ִ�в�������������q���ܹ�13λ��
#define Q_BIT_RESERVED		0		//��������Чռ2λ��00-������01-��Ч��10-������11-���ʣ�
#define Q_BIT_INVALID		1		//��Ч	
#define Q_BIT_OVERFLOW		2		//����
#define Q_BITL_OUTOFRANGE	3		//������Χ
#define Q_BIT_BADREFERENCE	4		//���������
#define Q_BIT_OSCILLATORY	5		//��
#define Q_BIT_FAILURE		6		//����
#define Q_BIT_OLDDATA		7		//������
#define Q_BIT_INCONSISTENT	8		//��һ��
#define Q_BIT_INACCURATE	9		//��׼ȷ
#define Q_BIT_SUBSTITUTED	10		//���
#define Q_BIT_TEST			11		//����
#define Q_BIT_BLOCKED		12		//����
#define Q_BIT_INIT			15		//��ʼ��
		
//������(ֵ)
#define Q_VAL_GOOD			0x0000		//��Ч
#define Q_VAL_INVALID		0x0001		//��Ч	
#define Q_VAL_RESERVED		0x0002		//����
#define Q_VAL_QUESTIONABLE	0x0003		//����
#define Q_VAL_OVERFLOW		0x0004		//���
#define Q_VAL_OUTOFRANGE	0x0008		//������Χ
#define Q_VAL_BADREFERENCE	0x0010		//���������
#define Q_VAL_OSCILLATORY	0x0020		//��
#define Q_VAL_FAILURE		0x0040		//����
#define Q_VAL_OLDDATA		0x0080		//������
#define Q_VAL_INCONSISTENT	0x0100		//��һ��
#define Q_VAL_INACCURATE	0x0200		//��׼ȷ
#define Q_VAL_SUBSTITUTED	0x0400		//���
#define Q_VAL_TEST			0x0800		//����
#define Q_VAL_BLOCKED		0x1000		//����
#define Q_VAL_INIT			0x8000		//��ʼ��

//yhh20110929>>>
//�ַ�������
#define SCL_MAX_LEN_NAME		64		//������󳤶�
#define SCL_MAX_LEN_ADDR		32		//��ַ���磺IP����ַ��󳤶�

//����������
#define SCL_GRP_TYPE_SERVER		0		//�����
#define SCL_GRP_TYPE_CLIENT		1		//�ͻ���

//������״̬
#define SCL_GRP_STATE_DOWN		0		//������״̬��δ����
#define SCL_GRP_STATE_UP		1		//������״̬������
//yhh20110929<<<

//ʱ��
struct scl_ts 
{
	time_t			secs;	//����(1970-1-1 ��ҹ����������)
	unsigned int	ms;		//����		
};
typedef struct scl_ts SCL_TS;

//������ yhh20110929
struct scl_group
{
	unsigned char	valid;
	int				no;	
	unsigned char	type;
	char			name[SCL_MAX_LEN_NAME];
	char			addr[SCL_MAX_LEN_ADDR];
	unsigned char	state;

};
typedef struct scl_group SCL_GROUP;

//ң��
struct scl_ai
{
	float			value;		//ֵ
	float			h;			//����
	float			l;			//����
	float			hh;			//������
	float			ll;			//������
	unsigned int	quality;	//������
	SCL_TS			updtime;	//ˢ��ʱ��
};
typedef struct scl_ai SCL_AI;

//ң��
struct scl_di
{
	unsigned short	value;		//ֵ
	unsigned int	quality;	//������
	SCL_TS			updtime;	//ˢ��ʱ��
};
typedef struct scl_di SCL_DI;

//���
struct scl_ei
{
	float			value;		//ֵ
	unsigned int	quality;	//������
	SCL_TS			updtime;	//ˢ��ʱ��
};
typedef struct scl_ei SCL_EI;


//�ַ���
struct scl_si 
{
	char			value[SCL_SI_VAL_LEN_MAX];	//ֵ
	unsigned int	quality;					//������
	SCL_TS			updtime;	//ˢ��ʱ��
};
typedef struct scl_si SCL_SI;

//ʱ��
struct scl_ti 
{
	time_t			value;						//ֵ
	unsigned int	quality;					//������
	SCL_TS			updtime;	//ˢ��ʱ��
};
typedef struct scl_ti SCL_TI;

//wfp add at 20110703 for sgcb begin
//ң��SE
struct scl_ai_se
{
	int				modify;		//�޸ı�־(�յ��Զ˵�SE��ֵʱΪ1)
	float			value;		//ֵ
	unsigned int	quality;	//������
	SCL_TS			updtime;	//ˢ��ʱ��
};
typedef struct scl_ai_se SCL_AI_SE;

//ң��SE
struct scl_di_se
{
	int				modify;		//�޸ı�־(�յ��Զ˵�SE��ֵʱΪ1)
	unsigned short	value;		//ֵ
	unsigned int	quality;	//������
	SCL_TS			updtime;	//ˢ��ʱ��
};
typedef struct scl_di_se SCL_DI_SE;
//wfp add at 20110703 for sgcb begin

//��ң��
struct scl_chg_ai
{
	int				no;			//���	
	SCL_AI			data;		//����
};
typedef struct scl_chg_ai SCL_CHG_AI;

//��ң��
struct scl_chg_di
{
	int				no;			//���
	SCL_DI			data;		//����
};
typedef struct scl_chg_di SCL_CHG_DI;

//����
struct scl_chg_ei
{
	int				no;			//���
	SCL_EI			data;		//����
};
typedef struct scl_chg_ei SCL_CHG_EI;


//���ַ��� 
struct scl_chg_si 
{
	int				no;			//���
	SCL_SI			data;		//����			
};
typedef struct scl_chg_si SCL_CHG_SI;

//��ʱ�� 
struct scl_chg_ti 
{
	int				no;			//���
	SCL_TI			data;		//����			
};
typedef struct scl_chg_ti SCL_CHG_TI;

//ң��ֵԼ��
#define SCL_YK_VAL_OFF				0
#define SCL_YK_VAL_ON				1
#define SCL_YK_VAL_ERR				3


//ң������
#define SCL_YK_TYPE_NO				0	//�������
#define SCL_YK_TYPE_DO				1	//���氲ȫ��ֱ�ӿ���(���ʱ������)
#define	SCL_YK_TYPE_SO				2	//���氲ȫ��ѡ�����
#define SCL_YK_TYPE_SDO				3	//��ǿ��ȫ��ֱ�ӿ���
#define SCL_YK_TYPE_SSO				4	//��ǿ��ȫ��ѡ�����

//ң�ض���
#define	SCL_YK_ACTION_SELECT		0	//����ѡ��
#define	SCL_YK_ACTION_SELECT_V		1	//����ѡ��(��ֵ)
#define	SCL_YK_ACTION_EXECUTE		2	//����ִ��
#define SCL_YK_ACTION_CANCEL		3	//���Ƴ���
#define SCL_YK_ACTION_SET_VALUE		4	//���--ʵ��ֵ (��ֵΪ�������)
#define SCL_YK_ACTION_SET_H			5	//���--����	(��ֵ���ǵ������)
#define SCL_YK_ACTION_SET_L			6	//���--����	(��ֵ���ǵ������)
#define SCL_YK_ACTION_SET_HH		7	//���--������	(��ֵ���ǵ������)
#define SCL_YK_ACTION_SET_LL		8	//���--������	(��ֵ���ǵ������)	

//wfp add at 20110703 for sgcb
#define SCL_YK_ACTION_SET_ACTSG		10	//���--����ָ����ֵ��	
#define SCL_YK_ACTION_SET_EDITSG	11	//���--�༭ָ����ֵ��	
#define SCL_YK_ACTION_SET_SGVALUES	12	//���--��ֵд��༭��
#define SCL_YK_ACTION_SET_CNFEDIT	13	//���--ȷ�ϱ༭ָ����ֵ��

//ң��
struct scl_yk
{
	unsigned char	type;			//�������ͼ� SCL_YK_TYPE_xx
	int				grpno;			//������
	int				no;				//���
	int				ctrlAction;		//���ƶ��� SCL_YK_ACTION_xx
	int				ctrlIntValue;	//����״ֵ̬
	float			ctrlFloatValue;	//���Ƹ���ֵ(ctrlAction=SCL_YK_ACTION_SET_xx��Ч)
	unsigned char	ctrlNum;		//����˳��ţ�61850-8-1��
	unsigned char	testFlag;		//���Ա�־(61850-8-1)
	unsigned char	ctrlDuration;	//���Ƴ���ʱ��
	unsigned char	elapseTime;		//����δִ�ж����ŵ�ʱ�䣨����λ����
};
typedef struct scl_yk SCL_YK;

//ң��Ӧ��
struct scl_yk_ack 
{
	unsigned char	type;			//�������ͼ� SCL_YK_TYPE_xx
	int				grpno;			//������
	int				no;				//���
	int				ctrlAction;		//���ƶ��� SCL_YK_ACTION_xx
	int				oldIntValue;	//����ǰԭ����ֵ
	int				newIntValue;	//���ƺ�����ֵ
	float			oldFloatValue;	//����ǰ����ֵ
	float			newFloatValue;	//���ƺ󸡵�ֵ
	unsigned char	result;			//0--�ɹ�������ֵ--ʧ�ܣ��ɹ�ʱ��newIntValue
									//�� newFloatValue��Ч
};
typedef struct scl_yk_ack SCL_YK_ACK;

//��ʷ��������
#define SCL_HIS_VAL_TYPE_AI		0		//��ʷ����ֵ���ͣ�AI
#define SCL_HIS_VAL_TYPE_DI		1		//��ʷ����ֵ���ͣ�DI
#define SCL_HIS_VAL_TYPE_EI		2		//��ʷ����ֵ���ͣ�EI
#define SCL_HIS_VAL_TYPE_SI		3		//��ʷ����ֵ���ͣ�SI
#define SCL_HIS_VAL_TYPE_TI		4		//��ʷ����ֵ���ͣ�TI

//һ���������ʷ������
#define SCL_HIS_VAL_MAX			8192

//��ʷ��������
struct scl_his_cmd
{
	SCL_TS	from;				//��ʼʱ��
	SCL_TS	to;					//����ʱ��
};
typedef struct scl_his_cmd	SCL_HIS_CMD;

//��ʷ����ֵ
struct	scl_his_val 
{
	int		type;				//��ʷ����ֵ���ͣ��������궨�壺SCL_HIS_VAL_TYPE_*
	int		no;					//���
	int		intVal;				//����ֵ���� type=SCL_HIS_VAL_TYPE_DI ʱ
	float	fltVal;				//����ֵ���� type!=SCL_HIS_VAL_TYPE_DI ʱ
	SCL_TS	time;				//��ʷ����ʱ��
};
typedef struct scl_his_val	SCL_HIS_VAL;

//��ʷ����
struct scl_his_dat
{
	int				count;						//����values����ʷ����ֵ����Ч����
	SCL_HIS_VAL		values[SCL_HIS_VAL_MAX];	//��ʷ����ֵ����
};
typedef struct scl_his_dat	SCL_HIS_DAT;

//��ʷ���ݿ���
struct scl_his_ctrl
{
	int				cmdFlag;		//�Ƿ��л�ȡ��ʷ����ֵ�������־��0--��1--��
	SCL_HIS_CMD		cmd;			//��ȡ��ʷ�����������
	int				datFlag;		//�Ƿ���ʷ����׼���õı�־��0--��1--��
	SCL_HIS_DAT		dat;			//��ȡ������ʷ����
};
typedef struct scl_his_ctrl SCL_HIS_CTRL;

//�����ڴ�������
struct scl_shm
{
	SCL_GROUP		group[SCL_GROUP_MAX];
	SCL_AI			ai[SCL_GROUP_MAX][SCL_AI_MAX];
	SCL_DI			di[SCL_GROUP_MAX][SCL_DI_MAX];
	SCL_EI			ei[SCL_GROUP_MAX][SCL_EI_MAX];
	SCL_SI			si[SCL_GROUP_MAX][SCL_SI_MAX];	
	SCL_TI			ti[SCL_GROUP_MAX][SCL_TI_MAX];	
	SCL_CHG_AI		chgAi[SCL_GROUP_MAX][SCL_CHG_AI_MAX];
	SCL_CHG_DI		chgDi[SCL_GROUP_MAX][SCL_CHG_DI_MAX];
	SCL_CHG_EI		chgEi[SCL_GROUP_MAX][SCL_CHG_EI_MAX];
	SCL_CHG_SI		chgSi[SCL_GROUP_MAX][SCL_CHG_SI_MAX];			
	SCL_CHG_TI		chgTi[SCL_GROUP_MAX][SCL_CHG_TI_MAX];				
	SCL_YK			yk[SCL_YK_MAX];
	SCL_YK_ACK		ykAck[SCL_YK_ACK_MAX];
	unsigned int	chgAiWrite[SCL_GROUP_MAX];
	unsigned int	chgDiWrite[SCL_GROUP_MAX];
	unsigned int	chgEiWrite[SCL_GROUP_MAX];
	unsigned int	chgSiWrite[SCL_GROUP_MAX];		
	unsigned int	chgTiWrite[SCL_GROUP_MAX];		
	unsigned int	ykWrite;
	unsigned int	ykAckWrite;
	unsigned char	initFlag;
	unsigned char	utcQFlags;	//yhh20110817
	SCL_AI_SE		aiSE[SCL_GROUP_MAX][SCL_AI_MAX];	//wfp at 20110703 for sgcb
	SCL_DI_SE		diSE[SCL_GROUP_MAX][SCL_DI_MAX];	//wfp at 20110703 for sgcb
	SCL_HIS_CTRL	hisDataCtrl[SCL_GROUP_MAX];			//yhh20111025 for his data
};
typedef struct   scl_shm	SCL_SHM;

//----------------------------------------------------------------
//�����ڴ���ʽӿ�
//----------------------------------------------------------------
#ifndef TRUE 
#define TRUE	1
#endif

#ifndef FALSE	
#define FALSE	0
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

//UTC ʱ��Ʒ��
#define UTC_QFLAG_OK						0x0a
#define UTC_QFLAG_LEAP_SECOND_KNOWN			0x8a
#define UTC_QFLAG_CLOCK_FAILURE				0x4a
#define UTC_QFLAG_CLOCK_NOT_SYNCHRONIZED	0x2a

//��ʼ��
int					initScl();
void				finitScl();

//ʱ�侫�ȼ�ͬ���źŴ���--yhh20110817
unsigned char		getUtcQFlags();
void				setUtcQFlags(unsigned char qflags);

//������ yhh20110929
SCL_GROUP*			getGroupFromNo(int groupNo);
SCL_GROUP*			getGroupFromName(const char* groupName);
SCL_GROUP*			getGroupFromAddr(const char* groupAddr);
SCL_GROUP*			getGroupFromNoDirect(int groupNo);
SCL_GROUP*			getGroupFromNameDirect(const char* groupName);
SCL_GROUP*			getGroupFromAddrDirect(const char* groupAddr);



//��ֵ��SE--wfp add at 20110703 for sgcb
void				putAiSE(int group,int no,const SCL_AI_SE* ai);
const SCL_AI_SE*	getAiSE(int group,int no);
void				putDiSE(int group,int no,const SCL_DI_SE* di);
const SCL_DI_SE*	getDiSE(int group,int no);

//ң��
void				putAi(int group,int no,const SCL_AI* ai);
const SCL_AI*		getAi(int group,int no);
int					hasChgAi(int group,unsigned int readPos);
const SCL_CHG_AI*	getChgAi(int group,unsigned int* readPos);
unsigned int		getChgAiWritePos(int group);

void				putAiFromName(const char* groupName,int no,const SCL_AI* ai);
const SCL_AI*		getAiFromName(const char* groupName,int no);
int					hasChgAiFromName(const char* groupName,unsigned int readPos);
const SCL_CHG_AI*	getChgAiFromName(const char* groupName,unsigned int* readPos);
unsigned int		getChgAiWritePosFromName(const char* groupName);

void				putAiFromAddr(const char* groupAddr,int no,const SCL_AI* ai);
const SCL_AI*		getAiFromAddr(const char* groupAddr,int no);
int					hasChgAiFromAddr(const char* groupAddr,unsigned int readPos);
const SCL_CHG_AI*	getChgAiFromAddr(const char* groupAddr,unsigned int* readPos);
unsigned int		getChgAiWritePosFromAddr(const char* groupAddr);


//ң��
void				putDi(int group,int no,const SCL_DI* di);
const SCL_DI*		getDi(int group,int no);
int					hasChgDi(int group,unsigned int readPos);
const SCL_CHG_DI*	getChgDi(int group,unsigned int* readPos);
unsigned int		getChgDiWritePos(int group);

void				putDiFromName(const char* groupName,int no,const SCL_DI* di);
const SCL_DI*		getDiFromName(const char* groupName,int no);
int					hasChgDiFromName(const char* groupName,unsigned int readPos);
const SCL_CHG_DI*	getChgDiFromName(const char* groupName,unsigned int* readPos);
unsigned int		getChgDiWritePosFromName(const char* groupName);

void				putDiFromAddr(const char* groupAddr,int no,const SCL_DI* di);
const SCL_DI*		getDiFromAddr(const char* groupAddr,int no);
int					hasChgDiFromAddr(const char* groupAddr,unsigned int readPos);
const SCL_CHG_DI*	getChgDiFromAddr(const char* groupAddr,unsigned int* readPos);
unsigned int		getChgDiWritePosFromAddr(const char* groupAddr);

//���
void				putEi(int group,int no,const SCL_EI* ei);
const SCL_EI*		getEi(int group,int no);
int					hasChgEi(int group,unsigned int readPos);
const SCL_CHG_EI*	getChgEi(int group,unsigned int* readPos);
unsigned int		getChgEiWritePos(int group);

void				putEiFromName(const char* groupName,int no,const SCL_EI* ei);
const SCL_EI*		getEiFromName(const char* groupName,int no);
int					hasChgEiFromName(const char* groupName,unsigned int readPos);
const SCL_CHG_EI*	getChgEiFromName(const char* groupName,unsigned int* readPos);
unsigned int		getChgEiWritePosFromName(const char* groupName);

void				putEiFromAddr(const char* groupAddr,int no,const SCL_EI* ei);
const SCL_EI*		getEiFromAddr(const char* groupAddr,int no);
int					hasChgEiFromAddr(const char* groupAddr,unsigned int readPos);
const SCL_CHG_EI*	getChgEiFromAddr(const char* groupAddr,unsigned int* readPos);
unsigned int		getChgEiWritePosFromAddr(const char* groupAddr);

//�ַ��� 
void				putSi(int group,int no,const SCL_SI* si);
const SCL_SI*		getSi(int group,int no);
int					hasChgSi(int group,unsigned int readPos);
const SCL_CHG_SI*	getChgSi(int group,unsigned int* readPos);
unsigned int		getChgSiWritePos(int group);

void				putSiFromName(const char* groupName,int no,const SCL_SI* si);
const SCL_SI*		getSiFromName(const char* groupName,int no);
int					hasChgSiFromName(const char* groupName,unsigned int readPos);
const SCL_CHG_SI*	getChgSiFromName(const char* groupName,unsigned int* readPos);
unsigned int		getChgSiWritePosFromName(const char* groupName);

void				putSiFromAddr(const char* groupAddr,int no,const SCL_SI* si);
const SCL_SI*		getSiFromAddr(const char* groupAddr,int no);
int					hasChgSiFromAddr(const char* groupAddr,unsigned int readPos);
const SCL_CHG_SI*	getChgSiFromAddr(const char* groupAddr,unsigned int* readPos);
unsigned int		getChgSiWritePosFromAddr(const char* groupAddr);

//ʱ�� 
void				putTi(int group,int no,const SCL_TI* ti);
const SCL_TI*		getTi(int group,int no);
int					hasChgTi(int group,unsigned int readPos);
const SCL_CHG_TI*	getChgTi(int group,unsigned int* readPos);
unsigned int		getChgTiWritePos(int group);

void				putTiFromName(const char* groupName,int no,const SCL_TI* ti);
const SCL_TI*		getTiFromName(const char* groupName,int no);
int					hasChgTiFromName(const char* groupName,unsigned int readPos);
const SCL_CHG_TI*	getChgTiFromName(const char* groupName,unsigned int* readPos);
unsigned int		getChgTiWritePosFromName(const char* groupName);

void				putTiFromAddr(const char* groupAddr,int no,const SCL_TI* ti);
const SCL_TI*		getTiFromAddr(const char* groupAddr,int no);
int					hasChgTiFromAddr(const char* groupAddr,unsigned int readPos);
const SCL_CHG_TI*	getChgTiFromAddr(const char* groupAddr,unsigned int* readPos);
unsigned int		getChgTiWritePosFromAddr(const char* groupAddr);

//ң��
int					hasYk(unsigned int readPos);
void				putYk(const SCL_YK* yk);
SCL_YK*				getYk(unsigned int* readPos);
unsigned int		getYkWritePos();

//ң��Ӧ��
int					hasYkAck(unsigned int readPos);
void				putYkAck(const SCL_YK_ACK* ykAck);
SCL_YK_ACK*			getYkAck(unsigned int* readPos);
unsigned int		getYkAckWritePos();

//��ʷ����
int					hasHisCmd(int group);
void				setHisCmd(int group,const SCL_HIS_CMD* cmd);
SCL_HIS_CMD*		getHisCmd(int group);
int					hasHisData(int group);
void				putHisDataBegin(int group);
void				putHisDataVal(int group,const SCL_HIS_VAL* dat);
void				putHisDataEnd(int group);
SCL_HIS_DAT*		getHisData(int group);
SCL_HIS_DAT*		viewHisData(int group);

#ifdef __cplusplus
}
#endif	//__cpluscplus

#endif	//__IEC61850_IEDINF_H__
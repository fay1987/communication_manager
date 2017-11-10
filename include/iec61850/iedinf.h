#ifndef __IEC61850_IEDINF_H__
#define __IEC61850_IEDINF_H__

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------
//共享内存数据结构
//----------------------------------------------------------------
//共享内存键值
#define	SCL_SHM_KEY		18931226

//数据区大小限制
#define SCL_GROUP_MAX	256		//数据组
#define SCL_AI_MAX		64		//遥测
#define SCL_DI_MAX		32		//遥信
#define SCL_EI_MAX		32		//电度
#define SCL_SI_MAX		32		//字符串
#define SCL_TI_MAX		32		//时间
#define SCL_YK_MAX		32		//遥控
#define SCL_YK_ACK_MAX	32		//遥控应答

//字符串测点值最大长度
#define SCL_SI_VAL_LEN_MAX		128	

//变数据区大小
#define SCL_CHG_AI_MAX	(2*SCL_AI_MAX)		//变遥测
#define SCL_CHG_DI_MAX	(SCL_DI_MAX)		//变遥信
#define SCL_CHG_EI_MAX	(SCL_EI_MAX)		//变电度
#define SCL_CHG_SI_MAX	(SCL_SI_MAX)		//变字符串
#define SCL_CHG_TI_MAX	(SCL_TI_MAX)		//变时间


//质量码(掩码占位)
//必须使用宏  BSTR_BSTR_SET_ON、BSTR_BIT_SET_OFF、BSTR_BIT_GET 
//对相应的BitString类型数据执行操作。对质量码q，总共13位。
#define Q_BIT_RESERVED		0		//保留（有效占2位：00-正常，01-无效，10-保留，11-疑问）
#define Q_BIT_INVALID		1		//无效	
#define Q_BIT_OVERFLOW		2		//保留
#define Q_BITL_OUTOFRANGE	3		//超出范围
#define Q_BIT_BADREFERENCE	4		//错误的引用
#define Q_BIT_OSCILLATORY	5		//震荡
#define Q_BIT_FAILURE		6		//故障
#define Q_BIT_OLDDATA		7		//旧数据
#define Q_BIT_INCONSISTENT	8		//不一致
#define Q_BIT_INACCURATE	9		//不准确
#define Q_BIT_SUBSTITUTED	10		//替代
#define Q_BIT_TEST			11		//测试
#define Q_BIT_BLOCKED		12		//闭锁
#define Q_BIT_INIT			15		//初始化
		
//质量码(值)
#define Q_VAL_GOOD			0x0000		//有效
#define Q_VAL_INVALID		0x0001		//无效	
#define Q_VAL_RESERVED		0x0002		//保留
#define Q_VAL_QUESTIONABLE	0x0003		//疑问
#define Q_VAL_OVERFLOW		0x0004		//益出
#define Q_VAL_OUTOFRANGE	0x0008		//超出范围
#define Q_VAL_BADREFERENCE	0x0010		//错误的引用
#define Q_VAL_OSCILLATORY	0x0020		//震荡
#define Q_VAL_FAILURE		0x0040		//故障
#define Q_VAL_OLDDATA		0x0080		//旧数据
#define Q_VAL_INCONSISTENT	0x0100		//不一致
#define Q_VAL_INACCURATE	0x0200		//不准确
#define Q_VAL_SUBSTITUTED	0x0400		//替代
#define Q_VAL_TEST			0x0800		//测试
#define Q_VAL_BLOCKED		0x1000		//闭锁
#define Q_VAL_INIT			0x8000		//初始化

//yhh20110929>>>
//字符串长度
#define SCL_MAX_LEN_NAME		64		//名称最大长度
#define SCL_MAX_LEN_ADDR		32		//地址（如：IP）地址最大长度

//数据组类型
#define SCL_GRP_TYPE_SERVER		0		//服务端
#define SCL_GRP_TYPE_CLIENT		1		//客户端

//数据组状态
#define SCL_GRP_STATE_DOWN		0		//数据组状态：未连接
#define SCL_GRP_STATE_UP		1		//数据组状态：连接
//yhh20110929<<<

//时标
struct scl_ts 
{
	time_t			secs;	//秒数(1970-1-1 午夜以来的秒数)
	unsigned int	ms;		//毫秒		
};
typedef struct scl_ts SCL_TS;

//数据组 yhh20110929
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

//遥测
struct scl_ai
{
	float			value;		//值
	float			h;			//上限
	float			l;			//下限
	float			hh;			//上上限
	float			ll;			//下下限
	unsigned int	quality;	//质量码
	SCL_TS			updtime;	//刷新时间
};
typedef struct scl_ai SCL_AI;

//遥信
struct scl_di
{
	unsigned short	value;		//值
	unsigned int	quality;	//质量码
	SCL_TS			updtime;	//刷新时间
};
typedef struct scl_di SCL_DI;

//电度
struct scl_ei
{
	float			value;		//值
	unsigned int	quality;	//质量码
	SCL_TS			updtime;	//刷新时间
};
typedef struct scl_ei SCL_EI;


//字符串
struct scl_si 
{
	char			value[SCL_SI_VAL_LEN_MAX];	//值
	unsigned int	quality;					//质量码
	SCL_TS			updtime;	//刷新时间
};
typedef struct scl_si SCL_SI;

//时间
struct scl_ti 
{
	time_t			value;						//值
	unsigned int	quality;					//质量码
	SCL_TS			updtime;	//刷新时间
};
typedef struct scl_ti SCL_TI;

//wfp add at 20110703 for sgcb begin
//遥测SE
struct scl_ai_se
{
	int				modify;		//修改标志(收到对端的SE改值时为1)
	float			value;		//值
	unsigned int	quality;	//质量码
	SCL_TS			updtime;	//刷新时间
};
typedef struct scl_ai_se SCL_AI_SE;

//遥信SE
struct scl_di_se
{
	int				modify;		//修改标志(收到对端的SE改值时为1)
	unsigned short	value;		//值
	unsigned int	quality;	//质量码
	SCL_TS			updtime;	//刷新时间
};
typedef struct scl_di_se SCL_DI_SE;
//wfp add at 20110703 for sgcb begin

//变遥测
struct scl_chg_ai
{
	int				no;			//点号	
	SCL_AI			data;		//数据
};
typedef struct scl_chg_ai SCL_CHG_AI;

//变遥信
struct scl_chg_di
{
	int				no;			//点号
	SCL_DI			data;		//数据
};
typedef struct scl_chg_di SCL_CHG_DI;

//变电度
struct scl_chg_ei
{
	int				no;			//点号
	SCL_EI			data;		//数据
};
typedef struct scl_chg_ei SCL_CHG_EI;


//变字符串 
struct scl_chg_si 
{
	int				no;			//点号
	SCL_SI			data;		//数据			
};
typedef struct scl_chg_si SCL_CHG_SI;

//变时间 
struct scl_chg_ti 
{
	int				no;			//点号
	SCL_TI			data;		//数据			
};
typedef struct scl_chg_ti SCL_CHG_TI;

//遥控值约定
#define SCL_YK_VAL_OFF				0
#define SCL_YK_VAL_ON				1
#define SCL_YK_VAL_ERR				3


//遥控类型
#define SCL_YK_TYPE_NO				0	//不许控制
#define SCL_YK_TYPE_DO				1	//常规安全的直接控制(设点时可重用)
#define	SCL_YK_TYPE_SO				2	//常规安全的选择控制
#define SCL_YK_TYPE_SDO				3	//增强安全的直接控制
#define SCL_YK_TYPE_SSO				4	//增强安全的选择控制

//遥控动作
#define	SCL_YK_ACTION_SELECT		0	//控制选择
#define	SCL_YK_ACTION_SELECT_V		1	//控制选择(带值)
#define	SCL_YK_ACTION_EXECUTE		2	//控制执行
#define SCL_YK_ACTION_CANCEL		3	//控制撤销
#define SCL_YK_ACTION_SET_VALUE		4	//设点--实际值 (限值为单独测点)
#define SCL_YK_ACTION_SET_H			5	//设点--上限	(限值不是单独测点)
#define SCL_YK_ACTION_SET_L			6	//设点--下限	(限值不是单独测点)
#define SCL_YK_ACTION_SET_HH		7	//设点--上上限	(限值不是单独测点)
#define SCL_YK_ACTION_SET_LL		8	//设点--下下限	(限值不是单独测点)	

//wfp add at 20110703 for sgcb
#define SCL_YK_ACTION_SET_ACTSG		10	//设点--激活指定定值组	
#define SCL_YK_ACTION_SET_EDITSG	11	//设点--编辑指定定值组	
#define SCL_YK_ACTION_SET_SGVALUES	12	//设点--将值写入编辑区
#define SCL_YK_ACTION_SET_CNFEDIT	13	//设点--确认编辑指定定值组

//遥控
struct scl_yk
{
	unsigned char	type;			//过程类型见 SCL_YK_TYPE_xx
	int				grpno;			//数据组
	int				no;				//点号
	int				ctrlAction;		//控制动作 SCL_YK_ACTION_xx
	int				ctrlIntValue;	//控制状态值
	float			ctrlFloatValue;	//控制浮点值(ctrlAction=SCL_YK_ACTION_SET_xx有效)
	unsigned char	ctrlNum;		//控制顺序号（61850-8-1）
	unsigned char	testFlag;		//测试标志(61850-8-1)
	unsigned char	ctrlDuration;	//控制持续时间
	unsigned char	elapseTime;		//命令未执行而流逝的时间（）单位：秒
};
typedef struct scl_yk SCL_YK;

//遥控应答
struct scl_yk_ack 
{
	unsigned char	type;			//过程类型见 SCL_YK_TYPE_xx
	int				grpno;			//数据组
	int				no;				//点号
	int				ctrlAction;		//控制动作 SCL_YK_ACTION_xx
	int				oldIntValue;	//控制前原整型值
	int				newIntValue;	//控制后整型值
	float			oldFloatValue;	//控制前浮点值
	float			newFloatValue;	//控制后浮点值
	unsigned char	result;			//0--成功，其它值--失败，成功时，newIntValue
									//或 newFloatValue有效
};
typedef struct scl_yk_ack SCL_YK_ACK;

//历史数据类型
#define SCL_HIS_VAL_TYPE_AI		0		//历史数据值类型：AI
#define SCL_HIS_VAL_TYPE_DI		1		//历史数据值类型：DI
#define SCL_HIS_VAL_TYPE_EI		2		//历史数据值类型：EI
#define SCL_HIS_VAL_TYPE_SI		3		//历史数据值类型：SI
#define SCL_HIS_VAL_TYPE_TI		4		//历史数据值类型：TI

//一组中最大历史数据数
#define SCL_HIS_VAL_MAX			8192

//历史数据命令
struct scl_his_cmd
{
	SCL_TS	from;				//开始时间
	SCL_TS	to;					//结束时间
};
typedef struct scl_his_cmd	SCL_HIS_CMD;

//历史数据值
struct	scl_his_val 
{
	int		type;				//历史数据值类型，见上述宏定义：SCL_HIS_VAL_TYPE_*
	int		no;					//点号
	int		intVal;				//整型值：当 type=SCL_HIS_VAL_TYPE_DI 时
	float	fltVal;				//浮点值：当 type!=SCL_HIS_VAL_TYPE_DI 时
	SCL_TS	time;				//历史数据时间
};
typedef struct scl_his_val	SCL_HIS_VAL;

//历史数据
struct scl_his_dat
{
	int				count;						//后续values中历史数据值的有效条数
	SCL_HIS_VAL		values[SCL_HIS_VAL_MAX];	//历史数据值数组
};
typedef struct scl_his_dat	SCL_HIS_DAT;

//历史数据控制
struct scl_his_ctrl
{
	int				cmdFlag;		//是否有获取历史数据值的命令标志：0--否，1--是
	SCL_HIS_CMD		cmd;			//获取历史数据命令参数
	int				datFlag;		//是否历史数据准备好的标志：0--否，1--是
	SCL_HIS_DAT		dat;			//获取到的历史数据
};
typedef struct scl_his_ctrl SCL_HIS_CTRL;

//共享内存数据区
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
//共享内存访问接口
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

//UTC 时间品质
#define UTC_QFLAG_OK						0x0a
#define UTC_QFLAG_LEAP_SECOND_KNOWN			0x8a
#define UTC_QFLAG_CLOCK_FAILURE				0x4a
#define UTC_QFLAG_CLOCK_NOT_SYNCHRONIZED	0x2a

//初始化
int					initScl();
void				finitScl();

//时间精度及同步信号处理--yhh20110817
unsigned char		getUtcQFlags();
void				setUtcQFlags(unsigned char qflags);

//数据组 yhh20110929
SCL_GROUP*			getGroupFromNo(int groupNo);
SCL_GROUP*			getGroupFromName(const char* groupName);
SCL_GROUP*			getGroupFromAddr(const char* groupAddr);
SCL_GROUP*			getGroupFromNoDirect(int groupNo);
SCL_GROUP*			getGroupFromNameDirect(const char* groupName);
SCL_GROUP*			getGroupFromAddrDirect(const char* groupAddr);



//定值组SE--wfp add at 20110703 for sgcb
void				putAiSE(int group,int no,const SCL_AI_SE* ai);
const SCL_AI_SE*	getAiSE(int group,int no);
void				putDiSE(int group,int no,const SCL_DI_SE* di);
const SCL_DI_SE*	getDiSE(int group,int no);

//遥测
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


//遥信
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

//电度
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

//字符串 
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

//时间 
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

//遥控
int					hasYk(unsigned int readPos);
void				putYk(const SCL_YK* yk);
SCL_YK*				getYk(unsigned int* readPos);
unsigned int		getYkWritePos();

//遥控应答
int					hasYkAck(unsigned int readPos);
void				putYkAck(const SCL_YK_ACK* ykAck);
SCL_YK_ACK*			getYkAck(unsigned int* readPos);
unsigned int		getYkAckWritePos();

//历史数据
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
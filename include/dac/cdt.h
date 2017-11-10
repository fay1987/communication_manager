/*==============================================================================
* 文件名称 : cdt.h
* 模块 :     CDT标准规约
* 创建时间 : 2009-08-18
* 作者 : lj
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
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

    #define ELBHEAD_LENGTH      6		//elb头长度
    #define ELBEND_LENGTH		1		//elb尾长度
	#define	BYTE_LENGTH			2
    #define LOG_DLL				20000   //日志号
    #define	MAX_CMD_COUNT		16		//最大命令条数
    #define MAX_LENGTH          64  
    #define CMD_LENGTH          256     //命令最大长度
    #define MAX_BUF_SIZE        2048	//接收数据缓存长度


	#define DTB_CMD_HEAD1		0xEB	//ELB协议开头
	#define DTB_CMD_HEAD2		0x90
	#define DTB_CMD_TAIL		0x7E	//ELB协议结尾
	#define SC_REGISTER_CMD_1	0xE6	//EIS中sc端注册命令
	#define SC_TRANSFER_CMD		0xE3	//转发命令

	#define  TYPE_10			0		//功能码类型 10进制
	#define  TYPE_16			1		//16进制
	#define  TYPE_8				2		//8进制

    #define  TYPE_YCPARA_STD	0		//标准cdt
	#define  TYPE_YCPARA_LH		1		//低位在前
	#define  TYPE_YCPARA_HL		2		//高位在前
  
	#define  YCIMPBEGINNO		0
	#define  YCSUBBEGINNO		100
	#define  YCCOMBEGINNO		200

	#define  ELBRETTIME			10*60	//10分钟定时注册一次


	typedef struct _sent_cmd_
	{
		unsigned char cmd[CMD_LENGTH];		//命令                                             
		unsigned int  length;				//命令长度 
		char		  cmdstr[CMD_LENGTH];	//命令描述
	}SENT_CMD, *SENT_CMD_PTR;

	typedef struct rcv_cmd
	{
		hUChar cmd[MAX_BUF_SIZE];
		hUInt16 length;
	}RCV_CMD, *RCV_CMD_PTR;

	typedef struct cdt_feature   //modbus规约特征结构
	{
		hUInt16 ycfuntiontype;      //功能码类型 0：表示10进制 1：表示16进制 2:表示8进制
		hUInt16 ycparadtype;	    //测点数据解析类型（0：标准cdt方式（默认）1：高低位，低位在前，高位在后 2：高低位，高位在前，低位在后）
		hUInt16 ycimpbeginno;		//重要遥测开始序号	
		hUInt16 ycsubbeginno;		//次要遥测开始序号
		hUInt16 yccombeginno;		//普通遥测开始序号
	}CDT_FEATURE, *CDT_FEATURE_PTR;


	class CDT_EXPORT CCdt:public CProtocol
	{
		public:
			CCdt();
			 ~CCdt();
			virtual bool	isOpen() const;														//是否打开
			virtual bool	open();																//打开规约
			virtual void	close();															//关闭规约
			virtual void	mainRx();															//接收处理入口
			virtual void	mainTx();															//发送处理入口

		protected:
			virtual	bool	readFeature();														//加载规约特
			virtual void	buildRegCmd();														//注册命令
			virtual int 	buildCommand();														//构建命令,子类中实现返回命令条数
			virtual int		paraDevData(hUChar *buff, int nLen) = 0;							//解析设备具体协议
			
			int				getCmdCount(); 
	    private:		
			void			setRegInfo();														//写注册命令到共享内存	
			void			setCmdinfoToBuf(hUInt16 cursentfrm);
			QString			getEiiPara(int &com, const char *para);								//获取elb参数信息

		    void			buildElbRetCmd();                                                   //ebl桥注册命令
			void			buildELBProtocol(hUChar *byteCmd,int &RtnLeng, hUChar *buff, int nLen);//添加elb协议
			
			int				handleData(hUChar *buff, hUInt16 nLen);			//解析数据
			void            paraElbProtocol(hUChar *byteCmd, hUInt16 &leng);//解析elb协议
			void            paraDevProtocol();								//解析设备协议 
			void			putData(hUChar *buff, hUInt16 leng);            //把数据放入共享缓存中
	
		protected:
			SENT_CMD		m_SendCmd[MAX_CMD_COUNT];		//命令发送缓存
			SENT_CMD		m_RetCmd;						//注册命令
			
			CDT_FEATURE     m_feature;						//规约特征结构										

			DAC_ROUTE_INFO	*m_prouteinfo;					//路径共享内存
			DAC_ROUTE		*m_proute;						//路径参数
			DAC_CHANNEL		*m_pchannel;					//通道
			DAC_CHANNEL_INFO *m_pchannelinfo;				//通道信息
		
		private:
			RCV_CMD					m_RcvCmd;				//接收数据缓存
			BFS::CIntervalTimerSet	m_timeSet;				//时间定时器
			QString					m_elbPara;				//elb通信参数信息
		    int						m_icom;					//串口号
			int						m_timeAllDataID;		//全数据时间id
			int						m_timeRetID;			//注册时间id;
			hUInt16					m_iCmdCount;			//总发送命令条数
			hUInt16					m_iCurCmdFrm;			//当前发送桢
			bool					m_isOpen;				//规约是否打开
			hUChar                  m_elbhead[ELBHEAD_LENGTH];
			hUChar                  m_elbend[ELBEND_LENGTH];
	
	};
}
#endif//_BFS_DAC_MODBUS_H_规约库





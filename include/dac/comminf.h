/*==============================================================================
* 文件名称 : comminf.h
* 文件功能 ：定义公共参数信息接口
* 模块 : 前置采集接口
* 创建时间 : 2009-06-08
* 作者 : www
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_COMMINF_H_
#define	_PDT_DAC_COMMINF_H_

#include "dac/dacshm.h"

namespace PDT
{
		class DAC_EXPORT CCommInf
		{
		public:
			CCommInf();
			~CCommInf();

			/////////////////////////////参数获取///////////////////////////////////////////	
			DAC_SYSTEM*				system();					/*获取系统参数的共享内存地址*/
			const DAC_SYSTEM*		system() const;			
			DAC_STATION*			station(const char* code,bool isValidCheck = true);
			DAC_STATION*			station(hInt32 no=0,bool isValidCheck = true);
			const DAC_STATION*		station(const char* code,bool isValidCheck = true) const;
			const DAC_STATION*		station(hInt32 no=0,bool isValidCheck = true) const;
			
			DAC_SENDDEV*			senddev(hInt32 no =0,bool isValidCheck = true);
			const DAC_SENDDEV*		senddev(hInt32 no =0 ,bool isValidCheck = true) const;
			DAC_SENDDEV*			senddev(const char* code,bool isValidCheck = true);
			const DAC_SENDDEV*		senddev(const char* code,bool isValidCheck = true) const;

			DAC_RECVDEV*			getRecvdevbychan(hInt32 no);				//根据通道号获取对应的接收装置
			DAC_SENDDEV*			getSenddevbygroup(hInt32 no);				//根据数据组号获取对应的发送装置
			


			DAC_CHANNEL*			channel(const char* code,bool isValidCheck = true);		/*根据编码获取通道参数的共享内存地址*/
			DAC_CHANNEL*			channel(hInt32 no=0,bool isValidCheck = true);
			const DAC_CHANNEL*		channel(const char* code,bool isValidCheck = true) const;
			const DAC_CHANNEL*		channel(hInt32 no=0,bool isValidCheck = true) const;

			DAC_GROUP*				group(const char* code,bool isValidCheck = true);	/*根据组名获取通道参数的共享内存地址*/
			DAC_GROUP*				group(hInt32 no=0,bool isValidCheck = true);
			const DAC_GROUP*		group(const char* code,bool isValidCheck = true) const;
			const DAC_GROUP*		group(hInt32 no=0,bool isValidCheck = true) const;
			DAC_ROUTE*				route(const char* code,bool isValidCheck = true);	/*根据路径名获取通道参数的共享内存地址*/
			DAC_ROUTE*				route(hInt32 no=0,bool isValidCheck = true);
			const DAC_ROUTE*		route(const char* code,bool isValidCheck = true) const;
			const DAC_ROUTE*		route(hInt32 no=0,bool isValidCheck = true) const;
			DAC_PROTOCOL*			protocol(const char* code,bool isValidCheck = true);	/*根据规约名获取通道参数的共享内存地址*/
			DAC_PROTOCOL*			protocol(hInt32 no=0,bool isValidCheck = true);
			const DAC_PROTOCOL*		protocol(const char* code,bool isValidCheck = true) const;
			const DAC_PROTOCOL*		protocol(hInt32 no=0,bool isValidCheck = true) const;

			//wfp add at 20110916
			DAC_YK*					yk(const char* routeName,const char* code);
			DAC_YK*					ykShmPtr();
			const DAC_YK*			yk(const char* routeName,const char* code) const;
			const DAC_YK*			ykShmPtr() const;

			DAC_YT*					yt(const char* routeName,const char* code);
			DAC_YT*					ytShmPtr();
			const DAC_YT*			yt(const char* routeName,const char* code) const;
			const DAC_YT*			ytShmPtr() const;
			
			//yaoff add at 20170927
			DAC_YCCON*				yccon(int ntype);

			/////////////////////////////信息获取///////////////////////////////////////////
			//获取系统信息的共享内存地址
			DAC_SYSTEM_INFO*		systemInfo();
			const DAC_SYSTEM_INFO*	systemInfo() const;
			DAC_CHANNEL_INFO*		channelInfo(const char* code,bool isValidCheck = true) ;
			DAC_CHANNEL_INFO*		channelInfo(hInt32 no=0,bool isValidCheck = true);
			const DAC_CHANNEL_INFO*	channelInfo(const char* code,bool isValidCheck = true) const;
			const DAC_CHANNEL_INFO*	channelInfo(hInt32 no=0,bool isValidCheck = true) const;
			DAC_GROUP_INFO*			groupInfo(const char* code,bool isValidCheck = true);
			DAC_GROUP_INFO*			groupInfo(hInt32 no=0,bool isValidCheck = true);
			const DAC_GROUP_INFO*	groupInfo(const char* code,bool isValidCheck = true) const;
			const DAC_GROUP_INFO*	groupInfo(hInt32 no=0,bool isValidCheck = true) const;
			DAC_ROUTE_INFO*			routeInfo(const char* code,bool isValidCheck = true);
			DAC_ROUTE_INFO*			routeInfo(hInt32 no=0,bool isValidCheck = true);
			const DAC_ROUTE_INFO*	routeInfo(const char* code,bool isValidCheck = true) const;
			const DAC_ROUTE_INFO*	routeInfo(hInt32 no=0,bool isValidCheck = true) const;

			DAC_RECVDEV_INFO*		rdevInfo(hInt32 no = 0,bool isValidCheck = true);
		
		public:
			//判断指定序号的节点是否有效
			bool			isStationValid(hInt32 no) const;
			bool			isChannelValid(hInt32 no) const;
			bool			isGroupValid(hInt32 no) const;
			bool			isRouteValid(hInt32 no) const;
			bool			isProtocolValid(hInt32 no) const;
			bool			isSenddevValid(hInt32 no) const;

			//根据名称获取序号
			hInt32			stationNo(const char* code) const;
			hInt32			channelNo(const char* code) const;
			hInt32			groupNo(const char* code) const;
			hInt32			routeNo(const char* code) const;
			hInt32			protocolNo(const char* code) const;
			hInt32			senddevNo(const char* code) const;
			hInt32			recvdevNo(const char* code) const;

			hInt32			groupNoOfRoute(hInt32 routeNo,bool isValidCheck = true);
			hInt32			groupNoOfRoute(const char* routeName,bool isValidCheck = true);

		public:
			//设置channel,group,route的状态
			bool			setChannelState(hInt32 no,hUInt8 state);
			bool			setGroupState(hInt32 no,hUInt8 state);
			bool			setRouteState(hInt32 no,hUInt8 state);
			bool			setRouteManStop(hInt32 no,hBool manStop);

			//获取channel,group,route的信息,于channelInfo()函数不同,本函数获取的是一个拷贝
			bool			getChannelInfo(hInt32 no,DAC_CHANNEL_INFO& info);
			bool			getGroupInfo(hInt32 no,DAC_GROUP_INFO& info);
			bool			getRouteInfo(hInt32 no,DAC_ROUTE_INFO& info);

			//设置channel,group,route的信息,注意:设置前,请获取info的源信息
			bool			setChannelInfo(hInt32 no,const DAC_CHANNEL_INFO& info);
			bool			setGroupInfo(hInt32 no,const DAC_GROUP_INFO& info);
			bool			setRouteInfo(hInt32 no,const DAC_ROUTE_INFO& info);

			//是否存在变化信息
			bool			hasChgChannelInfo();
			bool			hasChgGroupInfo();
			bool			hasChgRouteInfo();

			//获取变化信息
			bool			chgChannelInfo(DAC_CHG_CHANNEL_INFO& info);
			bool			chgGroupInfo(DAC_CHG_GROUP_INFO& info);
			bool			chgRouteInfo(DAC_CHG_ROUTE_INFO& info);
		public:
			//清空channel、route、group的动态信息
			void			resetAllCommInfo();
		protected:
		private:
			DAC_COM_PARA		*m_pComPara;	
			DAC_COM_INFO		*m_pComInfo;	
			DAC_CTRL_CMD		*m_pComCtrl;
			DAC_CTRL_CMD		*m_pComAck;

			//各通道等状态信息变化区的读指针
			hUInt32				m_chgChannelRead;					//变化通道信息读指针
			hUInt32				m_chgGroupRead;						//变化数据组信息读指针
			hUInt32				m_chgRouteRead;						//变化路径信息读指针
		};
}
#endif	//_PDT_DAC_COMMINF_H_

/*==============================================================================
* �ļ����� : comminf.h
* �ļ����� �����幫��������Ϣ�ӿ�
* ģ�� : ǰ�òɼ��ӿ�
* ����ʱ�� : 2009-06-08
* ���� : www
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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

			/////////////////////////////������ȡ///////////////////////////////////////////	
			DAC_SYSTEM*				system();					/*��ȡϵͳ�����Ĺ����ڴ��ַ*/
			const DAC_SYSTEM*		system() const;			
			DAC_STATION*			station(const char* code,bool isValidCheck = true);
			DAC_STATION*			station(hInt32 no=0,bool isValidCheck = true);
			const DAC_STATION*		station(const char* code,bool isValidCheck = true) const;
			const DAC_STATION*		station(hInt32 no=0,bool isValidCheck = true) const;
			
			DAC_SENDDEV*			senddev(hInt32 no =0,bool isValidCheck = true);
			const DAC_SENDDEV*		senddev(hInt32 no =0 ,bool isValidCheck = true) const;
			DAC_SENDDEV*			senddev(const char* code,bool isValidCheck = true);
			const DAC_SENDDEV*		senddev(const char* code,bool isValidCheck = true) const;

			DAC_RECVDEV*			getRecvdevbychan(hInt32 no);				//����ͨ���Ż�ȡ��Ӧ�Ľ���װ��
			DAC_SENDDEV*			getSenddevbygroup(hInt32 no);				//����������Ż�ȡ��Ӧ�ķ���װ��
			


			DAC_CHANNEL*			channel(const char* code,bool isValidCheck = true);		/*���ݱ����ȡͨ�������Ĺ����ڴ��ַ*/
			DAC_CHANNEL*			channel(hInt32 no=0,bool isValidCheck = true);
			const DAC_CHANNEL*		channel(const char* code,bool isValidCheck = true) const;
			const DAC_CHANNEL*		channel(hInt32 no=0,bool isValidCheck = true) const;

			DAC_GROUP*				group(const char* code,bool isValidCheck = true);	/*����������ȡͨ�������Ĺ����ڴ��ַ*/
			DAC_GROUP*				group(hInt32 no=0,bool isValidCheck = true);
			const DAC_GROUP*		group(const char* code,bool isValidCheck = true) const;
			const DAC_GROUP*		group(hInt32 no=0,bool isValidCheck = true) const;
			DAC_ROUTE*				route(const char* code,bool isValidCheck = true);	/*����·������ȡͨ�������Ĺ����ڴ��ַ*/
			DAC_ROUTE*				route(hInt32 no=0,bool isValidCheck = true);
			const DAC_ROUTE*		route(const char* code,bool isValidCheck = true) const;
			const DAC_ROUTE*		route(hInt32 no=0,bool isValidCheck = true) const;
			DAC_PROTOCOL*			protocol(const char* code,bool isValidCheck = true);	/*���ݹ�Լ����ȡͨ�������Ĺ����ڴ��ַ*/
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

			/////////////////////////////��Ϣ��ȡ///////////////////////////////////////////
			//��ȡϵͳ��Ϣ�Ĺ����ڴ��ַ
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
			//�ж�ָ����ŵĽڵ��Ƿ���Ч
			bool			isStationValid(hInt32 no) const;
			bool			isChannelValid(hInt32 no) const;
			bool			isGroupValid(hInt32 no) const;
			bool			isRouteValid(hInt32 no) const;
			bool			isProtocolValid(hInt32 no) const;
			bool			isSenddevValid(hInt32 no) const;

			//�������ƻ�ȡ���
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
			//����channel,group,route��״̬
			bool			setChannelState(hInt32 no,hUInt8 state);
			bool			setGroupState(hInt32 no,hUInt8 state);
			bool			setRouteState(hInt32 no,hUInt8 state);
			bool			setRouteManStop(hInt32 no,hBool manStop);

			//��ȡchannel,group,route����Ϣ,��channelInfo()������ͬ,��������ȡ����һ������
			bool			getChannelInfo(hInt32 no,DAC_CHANNEL_INFO& info);
			bool			getGroupInfo(hInt32 no,DAC_GROUP_INFO& info);
			bool			getRouteInfo(hInt32 no,DAC_ROUTE_INFO& info);

			//����channel,group,route����Ϣ,ע��:����ǰ,���ȡinfo��Դ��Ϣ
			bool			setChannelInfo(hInt32 no,const DAC_CHANNEL_INFO& info);
			bool			setGroupInfo(hInt32 no,const DAC_GROUP_INFO& info);
			bool			setRouteInfo(hInt32 no,const DAC_ROUTE_INFO& info);

			//�Ƿ���ڱ仯��Ϣ
			bool			hasChgChannelInfo();
			bool			hasChgGroupInfo();
			bool			hasChgRouteInfo();

			//��ȡ�仯��Ϣ
			bool			chgChannelInfo(DAC_CHG_CHANNEL_INFO& info);
			bool			chgGroupInfo(DAC_CHG_GROUP_INFO& info);
			bool			chgRouteInfo(DAC_CHG_ROUTE_INFO& info);
		public:
			//���channel��route��group�Ķ�̬��Ϣ
			void			resetAllCommInfo();
		protected:
		private:
			DAC_COM_PARA		*m_pComPara;	
			DAC_COM_INFO		*m_pComInfo;	
			DAC_CTRL_CMD		*m_pComCtrl;
			DAC_CTRL_CMD		*m_pComAck;

			//��ͨ����״̬��Ϣ�仯���Ķ�ָ��
			hUInt32				m_chgChannelRead;					//�仯ͨ����Ϣ��ָ��
			hUInt32				m_chgGroupRead;						//�仯��������Ϣ��ָ��
			hUInt32				m_chgRouteRead;						//�仯·����Ϣ��ָ��
		};
}
#endif	//_PDT_DAC_COMMINF_H_

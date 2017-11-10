/*==============================================================================
* 文件名称 : comminf.h
* 文件功能 ：定义公共数据接口
* 模块 : 前置采集接口
* 创建时间 : 2009-06-08
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_DAC_DATAINF_H_
#define	_PDT_DAC_DATAINF_H_

#include "dac/dacshm.h"
#include "event/csea.h"

namespace PDT
{
	class DAC_EXPORT CDataInf
	{
	public:
		CDataInf();
		~CDataInf();
	public:			//序号
		//数据组
		hInt32				groupNo(const char *code)	const;
		//路径
		hInt32				routeNo(const char *code)	const;
		//路径的组
		hInt32				groupOfRoute(hInt32 route)	const;
		hInt32				groupOfRoute(const char *route)	const;
		//YC
		hInt32				ycNo(hInt32 group,const char *code)	const;
		hInt32				ycNo(const char *group,const char *code)	const;
		//YX
		hInt32				yxNo(hInt32 group,const char *code)	const;
		hInt32				yxNo(const char *group,const char *code)	const;
		//KWH
		hInt32				kwhNo(hInt32 group,const char *code)	const;
		hInt32				kwhNo(const char *group,const char *code)	const;
	public:			//指针
		//YC参数
		DAC_YC*				ycPara(hInt32 group,hInt32 no=0);
		DAC_YC*				ycPara(hInt32 group,const char* code);
		DAC_YC*				ycPara(const char *group,hInt32 no=0);
		DAC_YC*				ycPara(const char *group,const char* code);
		const	DAC_YC*		ycPara(hInt32 group,hInt32 no=0)	const;
		const	DAC_YC*		ycPara(hInt32 group,const char* code)	const;
		const	DAC_YC*		ycPara(const char *group,hInt32 no=0)	const;
		const	DAC_YC*		ycPara(const char *group,const char* code)	const;
		//YX参数
		DAC_YX*				yxPara(hInt32 group,hInt32 no=0);
		DAC_YX*				yxPara(hInt32 group,const char* code);
		DAC_YX*				yxPara(const char *group,hInt32 no=0);
		DAC_YX*				yxPara(const char *group,const char* code);
		const	DAC_YX*		yxPara(hInt32 group,hInt32 no=0)	const;
		const	DAC_YX*		yxPara(hInt32 group,const char* code)	const;
		const	DAC_YX*		yxPara(const char *group,hInt32 no=0)	const;
		const	DAC_YX*		yxPara(const char *group,const char* code)	const;
		//KWH参数
		DAC_KWH*			kwhPara(hInt32 group,hInt32 no=0);
		DAC_KWH*			kwhPara(hInt32 group,const char* code);
		DAC_KWH*			kwhPara(const char *group,hInt32 no=0);
		DAC_KWH*			kwhPara(const char *group,const char* code);
		const	DAC_KWH*	kwhPara(hInt32 group,hInt32 no=0)	const;
		const	DAC_KWH*	kwhPara(hInt32 group,const char* code)	const;
		const	DAC_KWH*	kwhPara(const char *group,hInt32 no=0)	const;
		const	DAC_KWH*	kwhPara(const char *group,const char* code)	const;
		//YC数据
		DAC_YC_DATA*			ycData(hInt32 route,hInt32 no=0);
		DAC_YC_DATA*			ycData(hInt32 route,const char *code);
		DAC_YC_DATA*			ycData(const char *route,hInt32 no=0);
		DAC_YC_DATA*			ycData(const char *route,const char *code);
		const	DAC_YC_DATA*	ycData(hInt32 route,hInt32 no=0)	const;
		const	DAC_YC_DATA*	ycData(hInt32 route,const char *code)	const;
		const	DAC_YC_DATA*	ycData(const char *route,hInt32 no=0)	const;
		const	DAC_YC_DATA*	ycData(const char *route,const char *code)	const;
		//YX数据
		DAC_YX_DATA*			yxData(hInt32 route,hInt32 no=0);
		DAC_YX_DATA*			yxData(hInt32 route,const char *code);
		DAC_YX_DATA*			yxData(const char *route,hInt32 no=0);
		DAC_YX_DATA*			yxData(const char *route,const char *code);
		const	DAC_YX_DATA*	yxData(hInt32 route,hInt32 no=0)	const;
		const	DAC_YX_DATA*	yxData(hInt32 route,const char *code)	const;
		const	DAC_YX_DATA*	yxData(const char *route,hInt32 no=0)	const;
		const	DAC_YX_DATA*	yxData(const char *route,const char *code)	const;
		//KWH数据
		DAC_KWH_DATA*			kwhData(hInt32 route,hInt32 no=0);
		DAC_KWH_DATA*			kwhData(hInt32 route,const char *code);
		DAC_KWH_DATA*			kwhData(const char *route,hInt32 no=0);
		DAC_KWH_DATA*			kwhData(const char *route,const char *code);
		const	DAC_KWH_DATA*	kwhData(hInt32 route,hInt32 no=0)	const;
		const	DAC_KWH_DATA*	kwhData(hInt32 route,const char *code)	const;
		const	DAC_KWH_DATA*	kwhData(const char *route,hInt32 no=0)	const;
		const	DAC_KWH_DATA*	kwhData(const char *route,const char *code)	const;
		//SOE数据
		const	DAC_SOE_DATA*	soeData(hInt32 route);
		const	DAC_SOE_DATA*	soeData(const char *route);
		//YC变化数据
		const	DAC_YC_CHG_DATA*	ycChgData(hInt32 route);
		const	DAC_YC_CHG_DATA*	ycChgData(const char *route);
		//YX变化数据
		const	DAC_YX_CHG_DATA*	yxChgData(hInt32 route);
		const	DAC_YX_CHG_DATA*	yxChgData(const char *route);
		//KWH变化数据
		const	DAC_KWH_CHG_DATA*	kwhChgData(hInt32 route);
		const	DAC_KWH_CHG_DATA*	kwhChgData(const char *route);
	public:			//取值
		//YC
		bool				yc(hInt32 route,hInt32 no,DAC_YC_DATA &val)	const;
		bool				yc(hInt32 route,const char *code,DAC_YC_DATA &val)	const;
		bool				yc(const char *route,hInt32 no,DAC_YC_DATA &val)	const;
		bool				yc(const char *route,const char *code,DAC_YC_DATA &val)	const;
		bool				ycMainRoute(hInt32 group,hInt32 no,DAC_YC_DATA &val)	const;
		//YX
		bool				yx(hInt32 routeNo,hInt32 yxNo,DAC_YX_DATA &val) const;
		bool				yx(hInt32 route,const char *code,DAC_YX_DATA &val)	const;
		bool				yx(const char *route,hInt32 no,DAC_YX_DATA &val)	const;
		bool				yx(const char *route,const char *code,DAC_YX_DATA &val)	const;
		bool				yxMainRoute(hInt32 group,hInt32 yxNo,DAC_YX_DATA &val) const;
		//KWH
		bool				kwh(hInt32 routeNo,hInt32 kwhNo,DAC_KWH_DATA &val) const;
		bool				kwh(hInt32 route,const char *code,DAC_KWH_DATA &val)	const;
		bool				kwh(const char *route,hInt32 no,DAC_KWH_DATA &val)	const;
		bool				kwh(const char *route,const char *code,DAC_KWH_DATA &val)	const;
		bool				kwhMainRoute(hInt32 group,hInt32 kwhNo,DAC_KWH_DATA &val) const;
		//SOE
		bool				soe(hInt32 route,DAC_SOE_DATA &val);
		bool				soe(const char *route,DAC_SOE_DATA &val);
		bool				soeMainRoute(hInt32 group,DAC_SOE_DATA &val);
		//变化YC
		bool				chgYc(hInt32 route,DAC_YC_CHG_DATA &val);
		bool				chgYc(const char *route,DAC_YC_CHG_DATA &val);
		bool				chgYcMainRoute(hInt32 group,DAC_YC_CHG_DATA &val);
		//变化YX
		bool				chgYx(hInt32 routeNo,DAC_YX_CHG_DATA &val);
		bool				chgYx(const char *route,DAC_YX_CHG_DATA &val);
		bool				chgYxMainRoute(hInt32 group,DAC_YX_CHG_DATA &val);
		//变化KWH
		bool				chgKwh(hInt32 routeNo,DAC_KWH_CHG_DATA &val);
		bool				chgKwh(const char *route,DAC_KWH_CHG_DATA &val);
		bool				chgKwhMainRoute(hInt32 group,DAC_KWH_CHG_DATA &val);
	public:			//赋值
		//YC
		bool				setYc(hInt32 route,hInt32 no,const DAC_YC_DATA &val,bool isRaw=true);
		bool				setYc(hInt32 route,const char *code,const DAC_YC_DATA &val,bool isRaw=true);
		bool				setYc(const char *route,hInt32 no,const DAC_YC_DATA &val,bool isRaw=true);
		bool				setYc(const char *route,const char *code,const DAC_YC_DATA &val,bool isRaw=true);
		bool				setYcMainRoute(hInt32 group,hInt32 no,const DAC_YC_DATA &val,bool isRaw=true);
		//YX
		bool				setYx(hInt32 route,hInt32 no,const DAC_YX_DATA &val,bool isRaw=true,bool eventFlag=false);
		bool				setYx(hInt32 route,const char *code,const DAC_YX_DATA &val,bool isRaw=true,bool eventFlag=false);
		bool				setYx(const char *route,hInt32 no,const DAC_YX_DATA &val,bool isRaw=true,bool eventFlag=false);
		bool				setYx(const char *route,const char *code,const DAC_YX_DATA &val,bool isRaw=true,bool eventFlag=false);
		bool				setYxMainRoute(hInt32 group,hInt32 no,const DAC_YX_DATA &val,bool isRaw=true,bool eventFlag=false);
		//KWH
		bool				setKwh(hInt32 route,hInt32 no,const DAC_KWH_DATA &val,bool isRaw=true);
		bool				setKwh(hInt32 route,const char *code,const DAC_KWH_DATA &val,bool isRaw=true);
		bool				setKwh(const char *route,hInt32 no,const DAC_KWH_DATA &val,bool isRaw=true);
		bool				setKwh(const char *route,const char *code,const DAC_KWH_DATA &val,bool isRaw=true);
		bool				setKwhMainRoute(hInt32 group,hInt32 no,const DAC_KWH_DATA &val,bool isRaw=true);
		//YC
		bool				setSoe(hInt32 route,const DAC_SOE_DATA &val,bool valueFlag=false,bool isRaw=true,bool eventFlag=false);
		bool				setSoe(const char *route,const DAC_SOE_DATA &val,bool valueFlag=false,bool isRaw=true,bool eventFlag=false);
		bool				setSoeMainRoute(hInt32 group,const DAC_SOE_DATA &val,bool valueFlag=false,bool isRaw=true,bool eventFlag=false);
	public:			//检测
		//是否存在SOE
		bool				hasSoe(hInt32 route);
		bool				hasSoe(const char *route);
		bool				hasSoeMainRoute(hInt32 group);
		//是否存在变化YC
		bool				hasChgYc(hInt32 route);
		bool				hasChgYc(const char *route);
		bool				hasChgYcMainRoute(hInt32 group);
		//是否存在变化YX
		bool				hasChgYx(hInt32 route);
		bool				hasChgYx(const char *route);
		bool				hasChgYxMainRoute(hInt32 group);
		//是否存在变化KWH
		bool				hasChgKwh(hInt32 route);
		bool				hasChgKwh(const char *route);
		bool				hasChgKwhMainRoute(hInt32 group);
	public:
		//清空数据参数
		void				resetAllDataPara();
		//清空YC,YX,KWH 的动态信息(质量码和更新时间)
		void				resetAllDataInfo();
		void				resetDataInfo(hInt32 group);
	private:
		//创建事项
		bool				createYxEvent(hInt32 group,hInt32 yx,const DAC_YX_DATA &oldVal,const DAC_YX_DATA &newVal);
		bool				createSoeEvent(hInt32 group,const DAC_SOE_DATA &val);
	private:
		void				init();
		hInt32				mainRoute(hInt32 group) const;
		bool				filter(hInt32 group,hInt32 no,DAC_YC_DATA* pData,const DAC_YC_DATA& inputData);
	private:
		CShareMemory		m_shm;
		hInt32				readYc[DAC_ROUTE_NUM];		//变化YC读指针
		hInt32				readYx[DAC_ROUTE_NUM];		//变化YX读指针
		hInt32				readKwh[DAC_ROUTE_NUM];		//变化KWH读指针
		hInt32				readSoe[DAC_ROUTE_NUM];		//SOE读指针
		PDT::CSysEvent			m_event;
	};
}

#endif //_PDT_DAC_DATAINF_H_

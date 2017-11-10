/*==============================================================================
* 文件名称 : hisstainf.h
* 文件功能 ：定义历史统计操作接口
* 模块 : 历史统计接口
* 创建时间 : 2009-06-08
* 作者 : 王方平
* 版本 : v1.0

* 修改后版本 : 1.1
* 修改摘要 : 
* 修改者 :  
* 修改日期 : 
==============================================================================*/
#ifndef	_PDT_SCD_HISSTA_INF_H_
#define	_PDT_SCD_HISSTA_INF_H_

#include <ace/config-all.h>
#include "utl/sharememorybase.h"
#include "scd/hisdatadef.h"

//=============================================================================
#if defined (_HISSTAINF_)
#  define	HIS_EXPORT	ACE_Proper_Export_Flag
#else
#  define	HIS_EXPORT	ACE_Proper_Import_Flag
#endif	// _HISSTAINF_
//=============================================================================

#define HIS_STA_SHM_KEY		2010081901			//共享内存标识
/*
#define HIS_STA_SHM_DIR		"dat/map/his"		//共享内存相对目录
#define	HIS_STA_SHM_FILE	"his.dat"			//共享内存映射文件
*/

namespace PDT
{
	class HIS_EXPORT CHisStaInf : public CShareMemoryBase
	{
	public:
		CHisStaInf();
		virtual ~CHisStaInf();
	public:
		bool			hasData(hInt32 group) const;
		bool			hasRsData() const;
		bool			get(hInt32 group,HIS_STA_DATA& data);
		bool			put(hInt32 group,const HIS_STA_DATA& data);

		bool			rsget(HIS_STA_RS_DATA& data);
		bool			rsput(const HIS_STA_RS_DATA& data);
	protected:
		bool			init();
		int				dataClass(hUInt32 valType) const;
	private:
		hUInt32				m_read[DAC_GROUP_NUM];		//读指针
		hUInt32				m_rsread;					//区域厂站读指针
		static HIS_STA_SHM	*m_ptr;
	};
}
#endif	//_PDT_SCD_HISDATA_INF_H_

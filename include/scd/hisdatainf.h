/*==============================================================================
* �ļ����� : hisstainf.h
* �ļ����� ��������ʷͳ�Ʋ����ӿ�
* ģ�� : ��ʷͳ�ƽӿ�
* ����ʱ�� : 2009-06-08
* ���� : ����ƽ
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
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

#define HIS_STA_SHM_KEY		2010081901			//�����ڴ��ʶ
/*
#define HIS_STA_SHM_DIR		"dat/map/his"		//�����ڴ����Ŀ¼
#define	HIS_STA_SHM_FILE	"his.dat"			//�����ڴ�ӳ���ļ�
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
		hUInt32				m_read[DAC_GROUP_NUM];		//��ָ��
		hUInt32				m_rsread;					//����վ��ָ��
		static HIS_STA_SHM	*m_ptr;
	};
}
#endif	//_PDT_SCD_HISDATA_INF_H_

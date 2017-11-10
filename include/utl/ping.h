/*==============================================================================
* �ļ����� : nethelper.h
* ģ�� : ��ϵ������ӿ�(�����ݿ�֧��)
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : �����
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 2008��08��08��
==============================================================================*/
#ifndef _PDT_DATABASE_PING_H_
#define _PDT_DATABASE_PING_H_

#include "utl/utl_export.h"

namespace PDT
{
	class UTL_EXPORT CPing
	{
	public:
		CPing(void);
		virtual ~CPing(void);
		
		static bool	ping(const char* ip,unsigned short port,bool connectFlag=false,int timeout=1000);
	private:
	};
}


#endif //_PDT_DATABASE_PING_H_

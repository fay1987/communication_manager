/*==============================================================================
* �ļ����� : comminf_c.cpp
* ģ�� : ǰ�����ݷ��ʽӿڣ�֧��C����
* ����ʱ�� : 2017-03-31 15:55:23
* ���� : www
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#include "dac/comminf.h"
#include "dac/comminf_c.h"


///////////////////////////����///////////////////////////////////////////////
//DAC_SYSTEM_C*	system()
//{
//	PDT::CCommInf	comminf;
//	return (DAC_SYSTEM_C*)comminf.system();
//}

DAC_SYSTEM_INFO_C*		systemInfo_c()
{
	PDT::CCommInf	comminf;
	return (DAC_SYSTEM_INFO_C*)comminf.systemInfo();
}


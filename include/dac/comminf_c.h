/*==============================================================================
* �ļ����� : comminf.h
* �ļ����� ��ǰ�����ݷ��ʽӿڣ�֧��C����
* ģ�� : ǰ�òɼ��ӿ�
* ����ʱ�� : 2017-03-31
* ���� : wlq
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_PDT_DAC_COMMINF_C_H_
#define	_PDT_DAC_COMMINF_C_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef	struct _dac_system_c_
{
	char		name[64];				//����
	char		desc[64];				//����
}DAC_SYSTEM_C;

//ϵͳ��Ϣ
typedef	struct _dac_system_info_c_
{
	unsigned char			serverFlag;					//�Ƿ���DAC������
	unsigned int			stationNum;					//ϵͳʵ�ʳ�վ����
	unsigned int			channelNum;					//ϵͳʵ��ͨ����Ŀ
	unsigned int			groupNum;					//ϵͳʵ����������Ŀ
	unsigned int			routeNum;					//ϵͳʵ��·����Ŀ
	unsigned char			initFlag;					//��ʼ����־
	unsigned int			loadParaFlag;				//װ�ز������־������λ��ͬ���ֲ�����
}DAC_SYSTEM_INFO_C;


DAC_SYSTEM_INFO_C*		systemInfo_c();



#ifdef __cplusplus
}
#endif

#endif	//_PDT_DAC_COMMINF_H_

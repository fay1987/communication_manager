/*==============================================================================
* �ļ����� : sysmacro.h
* �ļ����� ��ϵͳ���ֺ궨��
* ģ�� : ���ݲɼ�
* ����ʱ�� : 2009-06-08
* ���� : www
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	PDT_SYS_MACRO_H_
#define	PDT_SYS_MACRO_H_

//wfp add at 20091216
#ifndef	TRUE
#define TRUE	1
#endif
#ifndef	FALSE
#define FALSE	0
#endif

#define SYS_PROCNAME_LEN	16						//����������
#define SYS_PROCPARA_LEN	32						//����ִ�в�������
#define SYS_DESC_LEN		64						//�������� 

#define PATH_LEN			256						//·������
#define FILE_NAME_LEN		32						//�ļ�������
#define FILE_PATH_LEN		PATH_LEN+FILE_NAME_LEN	//�ļ�����·���ܳ���

#define SYS_THREAD_STACK_SIZE	(1*1024*1024)		//1M

#define SYS_ENV_VAR			"PDT_ROOT"				//ϵͳ��������


#endif //PDT_SYS_MACRO_H_

/*==============================================================================
* �ļ����� : scdmacro.h
* �ļ����� ��scada���ֺ궨��
* ģ�� : scada
* ����ʱ�� : 2009-06-08
* ���� : wfp
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	PDT_SCD_MACRO_H_
#define	PDT_SCD_MACRO_H_
	
#define SCD_HISDATA_FILEDIR			"hisdata"				//��ʷ�����ļ���
#define SCD_ERRDATA_FILEDIR			"errdata"				//���������ļ���
#define SCD_ERR_FILE_PREFIX			"err"					//���������ļ�ǰ׺
#define SCD_HISDATA_FILE_PREFIX		"hd"					//��ʷ�����ļ�ǰ׺
#define SCD_MAX_RECORDS_INFILE		4000					//�ļ��ɴ洢������¼��

#define SCD_YC_TABLE_PREFIX			"this_yc"				//ң����ʷ���ݱ�ǰ׺
#define SCD_YC_PROCEDURE			"phis_createyctable"	//ң����ʷ���ݵĽ���洢����
#define	SCD_YC_INSERT_PROCEDURE		"phis_inserthisdata"	//ң����ʷ���ݵĴ洢����
#define SCD_EVENT					"this_event"			//������ʷ��
#define SCD_DAY_STA_TABLE_PREFIX	"this_dyc"				//��ͳ�Ʊ�ǰ׺
#define SCD_DAY_STA_PROCEDURE		"phis_createdaystatable"//��ͳ�ƵĽ���洢����
#define SCD_MONTH_STA_TABLE			"this_myc"				//��ͳ�Ʊ�
#define SCD_SEASON_STA_TABLE		"this_syc"				//��ͳ�Ʊ�
#define SCD_YEAR_STA_TABLE			"this_yyc"				//��ͳ�Ʊ�
#define SCD_STA_INSERT_PROCEDURE	"phis_insertsta"		//ͳ���������Ĵ洢����


//����
#define SCD_BAT_RUN_TB_PREFIX	"this_battery"				//���м���ǰ׺
#define SCD_BAT_RUN_PROCEDURE	"phis_createbatteryruntable" //�������м���Ľ���洢����
#define SCD_BAT_CHARGE_TABLE	"this_battery_charge"		//��ŵ��
#define SCD_BAT_DYNAMIC_TABLE	"this_battery_dynamic"		//��̬�ŵ��
#define SCD_BAT_STATE_TABLE		"this_battery_state"		//������ʷ״̬��

//ϵͳ�Զ�����㹫ʽ����
#define SCD_FORMULA_SYS_SGZ			"sgz"					//��·�������¹ʻ��ܵ�ң��-->��·���¹���
#define SCD_FORMULA_SYS_ROUTESTATE	"routestate"			//��·��״̬-->��·��ң��
#define SCD_FORMULA_SYS_RDL	"rdl"			//��ʵʱ��������
#define SCD_FORMULA_SYS_BKSTATE	"bkstate"			//����״̬����


#endif //PDT_DAC_MACRO_H_

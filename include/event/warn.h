#ifndef __WARN_WARN_H__
#define	__WARN_WARN_H__

//�澯�������Ͷ���
#define	WARN_OBJ_TYPE_YC		0		//ң���
#define	WARN_OBJ_TYPE_YX		1		//ң�ŵ�
#define	WARN_OBJ_TYPE_KWH		2		//��ȵ�
#define	WARN_OBJ_TYPE_GROUP		3		//�����飨�磺������״̬��
#define	WARN_OBJ_TYPE_ROUTE		4		//·�����磺·��״̬��
#define	WARN_OBJ_TYPE_CHANNEL	5		//ͨ�����磺ͨ��״̬��
#define	WARN_OBJ_TYPE_SERVICE	6		//�����磺�������������������л��ȣ�
#define	WARN_OBJ_TYPE_PROCESS	7		//���̣��磺�����������˳��ȣ�
#define	WARN_OBJ_TYPE_HOST		8		//�������磺Ӳ�̡�CPU���ڴ�״���ȣ�
#define	WARN_OBJ_TYPE_SYSTEM	9		//ϵͳ���磺ƽ̨����״���ȣ�

//�澯�ȼ�
#define	WARN_LEVEL_WARNNING		0		//��ͨ�澯
#define WARN_LEVEL_ERROR		1		//���ظ澯
#define	WARN_LEVEL_FATAL		2		//�����澯

//��������
#define WARN_CONDICTION_TYPE_INVALID			0		//��Ч
#define WARN_CONDICTION_TYPE_BATTERYSTATE		1		//����״̬
#define WARN_CONDICTION_TYPE_OTHER				2		//��������ֵ

//����״̬
#define	WARN_BATTERY_STATE_CHARGE_FLOAT			0		//���䣨���м��״̬��
#define	WARN_BATTERY_STATE_CHARGE_EQUAL			1		//���䣨���״̬��
#define	WARN_BATTERY_STATE_DISCHARGE_DYNAMIC	2		//��̬�ŵ磨��Ϊ��
#define	WARN_BATTERY_STATE_DISCHARGE_STATIC		3		//��̬�ŵ磨��Ϊ���˶��Էŵ磩
#define	WARN_BATTERY_STATE_BREAK				4		//��·
#define	WARN_BATTERY_STATE_HANG					5		//����
#define	WARN_BATTERY_STATE_DISCHARGE_AUTO		6		//�Է�

#endif

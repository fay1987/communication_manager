/*==============================================================================
* �ļ����� : pastask.h
* ģ�� : �߼�Ӧ���������
* ����ʱ�� : 2011-11-25 15:55:23
* ���� : ����ƽ
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 
==============================================================================*/
#ifndef	_BFS_PAS_TASK_H_
#define	_BFS_PAS_TASK_H_

#include "utl/datatype.h"
#include "ace/Task.h"

namespace BFS
{
		class CPasTask : public ACE_Task<ACE_NULL_SYNCH>
		{
		public:
			CPasTask(){}
			virtual ~CPasTask(){}
			//�Ƿ��
			virtual	bool	init() = 0;
			//�򿪹�Լ
			virtual int		start() = 0;
			//�رչ�Լ
			virtual void	stop() = 0;
		};
}
#endif //_BFS_PAS_TASK_H_

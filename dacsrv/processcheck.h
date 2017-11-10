#ifndef _BFS_DAC_PROCESSCHECK_H_
#define _BFS_DAC_PROCESSCHECK_H_

#include "sys/procman.h"
#include <QtCore/QtCore>

class CProcessCheck : public QObject 
{
	Q_OBJECT
public:
	CProcessCheck();
	virtual ~CProcessCheck();
	bool	init();

	//���dacsrv����ʱ��ϳ�ʹ��
	void	beginInitSystem();
	void	endInitSystem();

protected Q_SLOTS:
	void					onCheckTimeOut();
private:
	PDT::CProcessManage		m_procMgr;
	QTimer					*m_pCheckTimer;
};

#endif //_BFS_DAC_PROCESSCHECK_H_


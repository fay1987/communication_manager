#ifndef _TRAY_H_
#define _TRAY_H_

#include <QSystemTrayIcon>
#include <QDialog>
#include "sys/procman.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

class CTray : public QDialog
{
    Q_OBJECT

public:
    CTray();
	~CTray();
	bool	init();
	//���dacsrv����ʱ��ϳ�ʹ��
	void	beginInitSystem();
	void	endInitSystem();

public Q_SLOTS:
	void onCheckTimeOut();
	void onAbout();
	void onQuit();
private:
	void setIcon();
    void createActions();
    void createTrayIcon();

private:
    QAction					*minimizeAction;
    QAction					*maximizeAction;
    QAction					*aboutAction;
    QAction					*quitAction;

    QSystemTrayIcon			*trayIcon;
    QMenu					*trayIconMenu;

	QTimer					*m_pCheckTimer;

	PDT::CProcessManage		m_procMan;
};

#endif

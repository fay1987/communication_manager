#ifndef _TRAY_H_
#define _TRAY_H_

#include <QtGui/QSystemTrayIcon>
#include <QtGui/QDialog>
#include "scd.h"

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

public Q_SLOTS:
	void onCheckTimeOut();
	void onAbout();
	void onQuit();
private:
	void setIcon();
    void createActions();
    void createTrayIcon();

private:
    QAction			*minimizeAction;
    QAction			*maximizeAction;
    QAction			*aboutAction;
    QAction			*quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu			*trayIconMenu;

	QTimer			*m_pCheckTimer;
	CScada			m_scada;
};

#endif

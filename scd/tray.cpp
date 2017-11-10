#include <QtGui>
#include "tray.h"

CTray::CTray()
{
    createActions();
    createTrayIcon();

	setIcon();
    trayIcon->show();

    setWindowTitle(tr("ScdTray"));
    resize(400, 300);

	m_pCheckTimer = new QTimer(this);
	m_pCheckTimer->start(300);  //300ms
	connect( m_pCheckTimer,SIGNAL( timeout() ),this, SLOT( onCheckTimeOut() ) );
}

CTray::~CTray()
{
}

bool CTray::init()
{
	return m_scada.init();
}
/*
 * 函数功能：设置托盘图标
 */
void CTray::setIcon()
{
	QIcon icon = QIcon(":/images/scd.png");
	trayIcon->setIcon(icon);
	setWindowIcon(icon);

	trayIcon->setToolTip( QString::fromLocal8Bit("SCADA服务") );
}

void CTray::createActions()
{
    //minimizeAction = new QAction(tr("Mi&nimize"), this);
    //connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    //maximizeAction = new QAction(tr("Ma&ximize"), this);
    //connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(onAbout()));

    quitAction = new QAction(tr("&Quit"), this);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(onQuit()));
}

void CTray::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    //trayIconMenu->addAction(minimizeAction);
    //trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void CTray::onCheckTimeOut()
{
	if ( !m_scada.run() )
		qApp->quit();
}

void CTray::onAbout()
{
	QMessageBox mb(this);
	QString text;

	text = QMessageBox::tr(
		"<h3>欢迎使用PDT3000产品</h3>"
		"<p>本产品：电力通用监控平台--SCADA服务 程序(scdsrv)</p>"
		"<p>版本: V4.0.1</p>"
		"<p>版权所有 2012-2018</p>"
		"<p>更多PDT3000产品和技术支持，请登录 : "
		"<a href=\"http://www.hzpdt.com/\">www.hzpdt.com</a></p>"
		);

	mb.setWindowTitle( QString::fromLocal8Bit("关于") );
	mb.setText(	QString::fromLocal8Bit(text.toAscii().constData()) ); //add for 鞍山直流班
	mb.setIconPixmap(QPixmap(":/images/bfs.png"));
	mb.addButton(QMessageBox::Ok);
	mb.exec();
}

void CTray::onQuit()
{
	QMessageBox box( QMessageBox::Question,tr("退出提示"),tr("确定退出？"),QMessageBox::Yes|QMessageBox::No,this );

	int ret = box.exec();
	if ( ret == QMessageBox::Yes ) 
	{
		qApp->quit();
	}
	else
		return;
}

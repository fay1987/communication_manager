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
 * �������ܣ���������ͼ��
 */
void CTray::setIcon()
{
	QIcon icon = QIcon(":/images/scd.png");
	trayIcon->setIcon(icon);
	setWindowIcon(icon);

	trayIcon->setToolTip( QString::fromLocal8Bit("SCADA����") );
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
		"<h3>��ӭʹ��PDT3000��Ʒ</h3>"
		"<p>����Ʒ������ͨ�ü��ƽ̨--SCADA���� ����(scdsrv)</p>"
		"<p>�汾: V4.0.1</p>"
		"<p>��Ȩ���� 2012-2018</p>"
		"<p>����PDT3000��Ʒ�ͼ���֧�֣����¼ : "
		"<a href=\"http://www.hzpdt.com/\">www.hzpdt.com</a></p>"
		);

	mb.setWindowTitle( QString::fromLocal8Bit("����") );
	mb.setText(	QString::fromLocal8Bit(text.toAscii().constData()) ); //add for ��ɽֱ����
	mb.setIconPixmap(QPixmap(":/images/bfs.png"));
	mb.addButton(QMessageBox::Ok);
	mb.exec();
}

void CTray::onQuit()
{
	QMessageBox box( QMessageBox::Question,tr("�˳���ʾ"),tr("ȷ���˳���"),QMessageBox::Yes|QMessageBox::No,this );

	int ret = box.exec();
	if ( ret == QMessageBox::Yes ) 
	{
		qApp->quit();
	}
	else
		return;
}

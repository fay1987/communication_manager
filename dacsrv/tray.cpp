#include <QtGui>
#include "tray.h"
#include <QtCore/QTextCodec>
#include "dac/dacdef.h"

CTray::CTray()
{
    createActions();
    createTrayIcon();

	setIcon();
    trayIcon->show();

    setWindowTitle(tr("dacsrv"));
    resize(400, 300);
}

CTray::~CTray()
{
}

bool CTray::init()
{
	return true;
}

void CTray::beginInitSystem()
{
}

void CTray::endInitSystem()
{
}

/*
 * �������ܣ���������ͼ��
 */
void CTray::setIcon()
{
	QIcon icon = QIcon(":/images/dacsrv.png");
	trayIcon->setIcon(icon);
	setWindowIcon(icon);

	trayIcon->setToolTip( QString::fromLocal8Bit("���ݲɼ�����") );
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
}

void CTray::onAbout()
{
	QMessageBox mb(this);
	QString text;

	text = QMessageBox::tr(
		"<h3>��ӭʹ��PDT3000��Ʒ</h3>"
		"<p>����Ʒ������ͨ�ü��ƽ̨--ͨ�òɼ����� ����(dacsrv)</p>"
		"<p>�汾: V4.0.1</p>"
		"<p>��Ȩ���� 2012-2018</p>"
		"<p>����PDT3000��Ʒ�ͼ���֧�֣����¼ : "
		"<a href=\"http://www.hzpdt.com/\">www.hzpdt.com</a></p>"
		);

	mb.setWindowTitle( QString::fromLocal8Bit("����") );
	mb.setText(	QString::fromLocal8Bit(text.toAscii().constData()) ); 
	mb.setIconPixmap(QPixmap(":/images/PDT.png"));
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

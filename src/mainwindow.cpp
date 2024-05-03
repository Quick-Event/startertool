#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "stageconfigwidget.h"
#include "application.h"
#include "loginwidget.h"
#include "startlistwidget.h"

#include <shv/coreqt/log.h>

#include <QSettings>
#include <QPushButton>
#include <QMenu>
#include <QTimer>

namespace rpc = shv::iotqt::rpc;

MainWindow::MainWindow(QWidget *parent) :
	Super(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->lblError->hide();

	auto *startlist_widget = new StartListWidget();
	ui->stackedWidget->addWidget(startlist_widget);

	auto *menu = new QMenu(this);
	ui->tbShowMenu->setMenu(menu);
	{
		auto *a = new QAction(tr("Connection"));
		connect(a, &QAction::triggered, this, [this]() {
			auto *widget = new LoginWidget(LoginWidget::AutoconnectEnabled::No);
			showDialogWidget(widget);
		});
		menu->addAction(a);
	}
	{
		auto *a = new QAction(tr("Stage config"));
		connect(a, &QAction::triggered, this, [this]() {
			auto *widget = new StageConfigWidget();
			showDialogWidget(widget);
		});
		menu->addAction(a);
	}
	//connect(ui->tbShowMenu, &QPushButton::clicked, this, [this]() {
	//	auto *login_widget = new LoginWidget();
	//	showDialogWidget(login_widget);
	//});
	auto *app = Application::instance();
	connect(app, &Application::brokerConnectedChanged, this, [this](bool is_connected, const QString &errmsg) {
		if (errmsg.isEmpty()) {
			if (is_connected) {
				ui->connectionState->setPixmap(QPixmap(QString::fromUtf8(":/images/green-led.svg")));
				showError({});
			}
			else {
				ui->connectionState->setPixmap(QPixmap(QString::fromUtf8(":/images/gray-led.svg")));
				showError(tr("Diconnected from broker"));
			}
		}
		else {
			ui->connectionState->setPixmap(QPixmap(QString::fromUtf8(":/images/red-led.svg")));
			showError(errmsg);
		}
	});
	{
		auto *tm = new QTimer(this);
		connect(tm, &QTimer::timeout, this, [this]() {
			auto dt = QDateTime::currentDateTime();
			auto tm = dt.time();
			if (tm.second() != m_currentTime.time().second()) {
				m_currentTime = dt;
				//auto ms = tm.msec();
				//shvInfo() << ms << dt.toString(Qt::ISODate);
				ui->lblCurrentTime->setText(m_currentTime.toString("hh:mm:ss"));
			}
		});
		tm->start(100);
	}

	auto *login_widget = new LoginWidget(LoginWidget::AutoconnectEnabled::Yes);
	showDialogWidget(login_widget);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::showDialogWidget(QWidget *widget)
{
	auto ix = ui->stackedWidget->addWidget(widget);
	ui->stackedWidget->setCurrentIndex(ix);
	ui->toolBarFrame->setDisabled(true);
	connect(widget, &QWidget::destroyed, ui->toolBarFrame, [frm=ui->toolBarFrame]() {
		frm->setDisabled(false);
	});
}

void MainWindow::showError(const QString &msg)
{
	if (msg.isEmpty()) {
		ui->lblError->hide();
	}
	else {
		ui->lblError->setText(msg);
		ui->lblError->show();
	}
}



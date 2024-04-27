#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "application.h"
#include "loginwidget.h"
#include "startlistwidget.h"

#include <shv/coreqt/log.h>

#include <QSettings>
#include <QPushButton>
#include <QMenu>

namespace rpc = shv::iotqt::rpc;

MainWindow::MainWindow(QWidget *parent) :
	Super(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->lblError->hide();

	auto *startlist_widget = new StartListWidget();
	ui->stackedWidget->addWidget(startlist_widget);

	{
		auto *menu = new QMenu(this);
		auto *a_connect = new QAction(tr("Connection"));
		connect(a_connect, &QAction::triggered, this, [this]() {
			auto *login_widget = new LoginWidget();
			login_widget->setAutoConnect(false);
			showDialogWidget(login_widget);
		});
		menu->addAction(a_connect);
		ui->tbShowMenu->setMenu(menu);
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

	auto *login_widget = new LoginWidget();
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



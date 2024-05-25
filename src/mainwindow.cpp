#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "si.h"
#include "settingswidget.h"
#include "application.h"
#include "loginwidget.h"
#include "startlistwidget.h"

#include "serialportsettingspage.h"
#include "stagesettingspage.h"
#include "classfiltersettingspage.h"
#include "uisettingspage.h"

#include <shv/coreqt/log.h>

#include <QSettings>
#include <QPushButton>
#include <QMenu>
#include <QTimer>
#include <QSerialPort>

namespace rpc = shv::iotqt::rpc;

MainWindow::MainWindow(QWidget *parent) :
	Super(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->lblError->hide();
	ui->frmFind->hide();

	auto *startlist_widget = new StartListWidget();
	connect(this, &MainWindow::cardRead, startlist_widget, &StartListWidget::onCardRead);
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
		auto *a = new QAction(tr("Settings"));
		connect(a, &QAction::triggered, this, [this]() {
			auto *widget = new SettingsWidget();

			widget->addPage(new UiSettingsPage());
			widget->addPage(new StageSettingsPage());
			widget->addPage(new ClassFilterSettingsPage());
			auto *serial_port_settings_page = new SerialPortSettingsPage();
			widget->addPage(serial_port_settings_page);
			connect(serial_port_settings_page, &SerialPortSettingsPage::serialPortSettingsChanged, this, [this]() {
				initCardReader();
			});

			showDialogWidget(widget);
			connect(widget, &SettingsWidget::destroyed, Application::instance(), &Application::emitSettingsChanged);

		});
		menu->addAction(a);
	}
	//connect(ui->tbShowMenu, &QPushButton::clicked, this, [this]() {
	//	auto *login_widget = new LoginWidget();
	//	showDialogWidget(login_widget);
	//});
	auto *app = Application::instance();
	connect(app, &Application::showErrorRq, this, &MainWindow::showError);
	connect(ui->btFind, &QAbstractButton::clicked, this, [this](bool checked) {
		shvInfo() << checked;
		ui->frmFind->setVisible(checked);
	});
	connect(app, &Application::brokerConnectedChanged, this, [this](bool is_connected, const QString &errmsg) {
		if (errmsg.isEmpty()) {
			if (is_connected) {
				ui->connectionState->setPixmap(QPixmap(QString::fromUtf8(":/images/green-led.svg")));
				hideError();
			}
			else {
				ui->connectionState->setPixmap(QPixmap(QString::fromUtf8(":/images/gray-led.svg")));
				showError(tr("Diconnected from broker"), NecroLogLevel::Warning);
			}
		}
		else {
			ui->connectionState->setPixmap(QPixmap(QString::fromUtf8(":/images/red-led.svg")));
			showError(errmsg, NecroLogLevel::Error);
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
	initCardReader();
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

void MainWindow::showError(const QString &msg, NecroLogLevel level)
{
	if (msg.isEmpty()) {
		ui->lblError->hide();
	}
	else {
		switch (level) {
		case NecroLogLevel::Invalid:
			ui->lblError->hide();
			return;
		case NecroLogLevel::Fatal:
		case NecroLogLevel::Error:
			ui->lblError->setStyleSheet("background: salmon; color: black");
			break;
		case NecroLogLevel::Warning:
			ui->lblError->setStyleSheet("background: orange; color: black");
			break;
		case NecroLogLevel::Info:
		case NecroLogLevel::Message:
		case NecroLogLevel::Debug:
			ui->lblError->setStyleSheet("background: white; color: black");
			break;
		break;
		}
		ui->lblError->setText(msg);
		ui->lblError->show();
	}
}

void MainWindow::initCardReader()
{
	delete findChild<QSerialPort*>();
	auto settings = SerialPortSettingsPage::loadSettings();
	if (!settings.enabled) {
		return;
	}
	auto *comport = new QSerialPort(settings.deviceName, this);
	comport->setBaudRate(settings.baudRate);
	comport->setDataBits(settings.dataBits);
	comport->setParity(settings.parity);
	comport->setStopBits(settings.stopBits);
	shvInfo() << "Opening" << settings.deviceName;
	ui->edReadSiId->setText({});
	if (comport->open(QIODevice::ReadWrite)) {
		ui->edReadSiId->setStyleSheet({});
		ui->edReadSiId->setText(settings.deviceName);
		connect(comport, &QSerialPort::readyRead, this, [this, comport]() {
			auto data = comport->readAll();
			try {
				auto [siid, serie, cmd] = si::parseDetectMessageData(data);
				if (cmd != si::Command::SICardRemoved) {
					ui->edReadSiId->setStyleSheet({});
					ui->edReadSiId->setText(QString::number(siid));
					emit cardRead(siid);
				}
			}
			catch(const std::exception &e) {
				ui->edReadSiId->setStyleSheet("background: salmon; color: black");
				ui->edReadSiId->setText(e.what());
			}
		});
	}
	else {
		ui->edReadSiId->setStyleSheet("background: salmon; color: black");
		ui->edReadSiId->setText(tr("%1 open error: %2").arg(settings.deviceName).arg(comport->errorString()));
	}
}



#include "serialportsettingspage.h"
#include "ui_serialportsettingspage.h"

#include "si.h"
#include "serialport.h"

#include <shv/coreqt/rpc.h>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>
#include <QLineEdit>
#include <QTimer>

SerialPortSettingsPage::SerialPortSettingsPage(QWidget *parent)
	: Super(tr("Serial port"), parent)
	, ui(new Ui::SerialPortSettingsPage)
{
	ui->setupUi(this);
	ui->txtLog->hide();
	connect(ui->grpSerialPort, &QGroupBox::toggled, this, [this](bool checked) {
		if (checked) {
			loadPorts();
		}
		else {
			ui->lstDevice->clear();
		}
	});
	connect(ui->btTest, &QPushButton::clicked, this, [this](bool checked) {
		auto append_log = [this](const QString &line) {
			ui->txtLog->appendPlainText(line);
		};

		delete findChild<SerialPort*>();
		if (checked) {
			ui->txtLog->show();
			ui->txtLog->clear();

			auto device = ui->lstDevice->currentData().toString();
			SerialPortSettings settings;
			settings.deviceName = device;
			auto *comport = new SerialPort(settings, this);
			append_log(tr("Opening %1 ...").arg(device));
			try {
				comport->open();
				append_log(tr("Ok"));
				connect(comport, &SerialPort::readyRead, this, [this, comport, append_log]() {
					auto data = comport->read();
					try {
						auto [siid, cmd] = si::parseDetectMessageData(data);
						if (cmd == si::Command::SICardRemoved) {
							append_log(tr("SI card removed"));
						}
						else {
							append_log(tr("SI card inserted: %1").arg(siid));
						}
					}
					catch(const std::exception &e) {
						append_log(tr("SI card detected message error: %1").arg(e.what()));
					}
				});
			}
			catch(const std::exception &e) {
				append_log(tr("Error: %1").arg(QString::fromUtf8(e.what())));
			}
		}
		else {
			ui->txtLog->hide();
		}
	});
}

SerialPortSettingsPage::~SerialPortSettingsPage()
{
	delete ui;
}

namespace {
constexpr auto ENABLED = "serialPort/enabled";
constexpr auto DEVICE = "serialPort/device";
}

SerialPortSettings SerialPortSettingsPage::loadSettings()
{
	SerialPortSettings ret;
	QSettings settings;
	ret.deviceName = settings.value(DEVICE).toString();
	ret.enabled = settings.value(ENABLED).toBool();
	return ret;
}

void SerialPortSettingsPage::load()
{
	auto *lst = ui->lstDevice;
	loadPorts();
	auto settings = loadSettings();
	if (auto ix = lst->findData(settings.deviceName); ix >= 0) {
		lst->setCurrentIndex(ix);
	}
	else {
		lst->setCurrentIndex(0);
	}
	ui->grpSerialPort->setChecked(settings.enabled);
}

void SerialPortSettingsPage::save()
{
	auto old = loadSettings();
	QSettings settings;
	settings.setValue(DEVICE, ui->lstDevice->currentData().toString());
	settings.setValue(ENABLED, ui->grpSerialPort->isChecked());
	auto current = loadSettings();
	if (!(old == current)) {
		emit serialPortSettingsChanged();
	}
}

void SerialPortSettingsPage::loadPorts()
{
	auto *lst = ui->lstDevice;
	lst->clear();
#ifdef ANDROID
	auto device = QJniObject::callStaticMethod<jstring>(
					"org/quickbox/startertool/SerialPort",
					"findSerialPort",
					"(Landroid/content/Context;)Ljava/lang/String;",
					QNativeInterface::QAndroidApplication::context());
	auto name = device.toString();
	if (!name.isEmpty()) {
		lst->addItem(QStringLiteral("SportIdent - %1").arg(name), name);
	}
	else {
		lst->addItem(QStringLiteral("%1").arg(name), name);
	}
#else
	const auto infos = QSerialPortInfo::availablePorts();
	for (const QSerialPortInfo &info : infos) {
		lst->addItem(QStringLiteral("%1 - %2").arg(info.portName()).arg(info.description()), info.portName());
	}
#endif
	lst->setCurrentIndex(0);
}

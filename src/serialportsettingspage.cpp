#include "serialportsettingspage.h"
#include "ui_serialportsettingspage.h"

#include "si.h"

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
	ui->txtError->hide();
	connect(ui->btTest, &QPushButton::clicked, this, [this](bool checked) {
		delete findChild<QSerialPort*>();
		if (checked) {
			ui->txtError->show();
			ui->txtError->clear();
			auto append_log = [this](const QString &line) {
				ui->txtError->appendPlainText(line);
				//ui->txtError->appendPlainText("\n");
			};
			auto device = ui->lstDevice->currentData().toString();
			SerialPortSettings settings;
			settings.deviceName = device;
			auto *comport = new QSerialPort(settings.deviceName, this);
			comport->setBaudRate(settings.baudRate);
			comport->setDataBits(settings.dataBits);
			comport->setParity(settings.parity);
			comport->setStopBits(settings.stopBits);
			append_log(tr("Opening %1 ...").arg(device));
			if (comport->open(QIODevice::ReadWrite)) {
				append_log(tr("Ok"));
				//QByteArray data;
				connect(comport, &QSerialPort::readyRead, this, [comport, append_log]() {
					auto data = comport->readAll();
					try {
						auto [siid, serie, cmd] = si::parseDetectMessageData(data);
						if (cmd == si::Command::SICardRemoved) {
							append_log(tr("SI card removed: %1 serie: %2").arg(siid).arg(serie));
						}
						else {
							append_log(tr("SI card inserted: %1 serie: %2").arg(siid).arg(serie));
						}
					}
					catch(const std::exception &e) {
						append_log(tr("SI card detected message error: %1").arg(e.what()));
					}
				});
			}
			else {
				append_log(tr("Error: %1").arg(comport->errorString()));
			}
		}
		else {
			ui->txtError->hide();
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
	const auto infos = QSerialPortInfo::availablePorts();
	for (const QSerialPortInfo &info : infos) {
		lst->addItem(QStringLiteral("%1 - %2").arg(info.portName()).arg(info.description()), info.portName());
	}
	auto settings = loadSettings();
	lst->setCurrentText(settings.deviceName);
	ui->grpSerialPort->setChecked(settings.enabled);
}

void SerialPortSettingsPage::save()
{
	auto old = loadSettings();
	QSettings settings;
	settings.setValue(DEVICE, ui->lstDevice->currentText());
	settings.setValue(ENABLED, ui->grpSerialPort->isChecked());
	auto current = loadSettings();
	if (!(old == current)) {
		emit serialPortSettingsChanged();
	}
}

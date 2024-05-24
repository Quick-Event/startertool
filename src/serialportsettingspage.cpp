#include "serialportsettingspage.h"
#include "ui_serialportsettingspage.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>
#include <QLineEdit>

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
			auto *pport = new QSerialPort(device, this);
			pport->setBaudRate(38400);
			pport->setDataBits(QSerialPort::Data8);
			pport->setParity(QSerialPort::NoParity);
			pport->setStopBits(QSerialPort::OneStop);
			append_log(tr("Opening %1 ...").arg(device));
			if (pport->open(QIODevice::ReadWrite)) {
				append_log(tr("Ok"));
				connect(pport, &QSerialPort::readyRead, this, [pport, append_log]() {
					auto ba = pport->readAll();
					auto hex = ba.toHex();
					hex.replace("02", "<STX>");
					hex.replace("03", "<ETX>");
					append_log(hex);
				});
			}
			else {
				append_log(tr("Error: %1").arg(pport->errorString()));
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
constexpr auto DEVICE = "serialPort/device";
}

void SerialPortSettingsPage::load()
{
	auto *lst = ui->lstDevice;
	const auto infos = QSerialPortInfo::availablePorts();
	for (const QSerialPortInfo &info : infos) {
		lst->addItem(QStringLiteral("%1 - %2").arg(info.portName()).arg(info.description()), info.portName());
	}
	QSettings settings;
	auto device = settings.value(DEVICE).toString();
	lst->setCurrentText(device);
}

void SerialPortSettingsPage::save()
{
	QSettings settings;
	settings.setValue(DEVICE, ui->lstDevice->currentText());
}

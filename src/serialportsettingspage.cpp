#include "serialportsettingspage.h"
#include "ui_serialportsettingspage.h"

#include <shv/coreqt/rpc.h>

#include <siut/sidevicedriver.h>

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
			auto *comport = new QSerialPort(device, this);
			comport->setBaudRate(38400);
			comport->setDataBits(QSerialPort::Data8);
			comport->setParity(QSerialPort::NoParity);
			comport->setStopBits(QSerialPort::OneStop);
			append_log(tr("Opening %1 ...").arg(device));
			if (comport->open(QIODevice::ReadWrite)) {
				append_log(tr("Loading SI station info ..."));
				auto *sidriver = new siut::DeviceDriver(comport);
				connect(comport, &QSerialPort::readyRead, this, [comport, sidriver]() {
					QByteArray ba = comport->readAll();
					sidriver->processData(ba);
				});
				connect(sidriver, &siut::DeviceDriver::dataToSend, comport, [comport](const QByteArray &data) {
					comport->write(data);
				});
				siut::SiTaskStationConfig *cmd = new siut::SiTaskStationConfig();
				connect(cmd, &siut::SiTaskStationConfig::finished, this, [this, comport, sidriver, append_log](bool ok, QVariant result) {
					if(ok) {
						siut::SiStationConfig cfg(result.toMap());
						QString msg = cfg.toString();
						append_log(tr("SI reader config:%1").arg(msg));
					}
					else {
						append_log(tr("Device %1 is not SI reader").arg(comport->portName()));
					}
					connect(sidriver, &siut::DeviceDriver::siTaskFinished, this, [append_log](int task_type, QVariant result) {
						auto task_name = siut::SiTask::typeToString(static_cast<siut::SiTask::Type>(task_type));
						auto msg = shv::coreqt::rpc::qVariantToRpcValue(result).toCpon("  ");
						append_log(tr("SI task finished - type: %1, result: %2").arg(task_name).arg(QString::fromStdString(msg)));
					});
				});
				sidriver->setSiTask(cmd);
			}
			else {
				append_log(tr("Error: %1").arg(comport->errorString()));
			}
			//if (pport->open(QIODevice::ReadWrite)) {
			//	append_log(tr("Ok"));
			//	connect(pport, &QSerialPort::readyRead, this, [pport, append_log]() {
			//		auto ba = pport->readAll();
			//		auto hex = ba.toHex();
			//		hex.replace("02", "<STX>");
			//		hex.replace("03", "<ETX>");
			//		append_log(hex);
			//	});
			//}
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

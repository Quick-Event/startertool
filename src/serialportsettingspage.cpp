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
					if (data.isEmpty()) {
						return;
					}
					if (data[0] != si::STX) {
						append_log("STX not received");
						return;
					}
					if (data[data.size() - 1] != si::ETX) {
						append_log("ETX not received");
						return;
					}

					auto hex = data.toHex();
					auto byte_to_hex = [](uint8_t b) {
						QByteArray ba;
						ba.append(static_cast<char>(b));
						return ba.toHex();
					};
					//auto command_to_hex = [byte_to_hex](siut::SIMessageData::Command cmd) {
					//	return byte_to_hex(static_cast<uint8_t>(cmd));
					//};
					//hex.replace("02", "<STX>");
					//hex.replace("03", "<ETX>");
					//hex.replace(command_to_hex(siut::SIMessageData::Command::SICard5Detected), "<IN5>");
					//hex.replace(command_to_hex(siut::SIMessageData::Command::SICard6Detected), "<IN6>");
					//hex.replace(command_to_hex(siut::SIMessageData::Command::SICard8Detected), "<IN8>");
					//hex.replace(command_to_hex(siut::SIMessageData::Command::SICardRemoved), "<OUT>");
					append_log(hex);

					auto bytes_to_uint = [](QByteArrayView ba) {
						unsigned ret = 0;
						for(auto b : ba) {
							auto u = static_cast<uint8_t>(b);
							ret = 256 * ret + u;
						}
						return ret;
					};
					unsigned card_serie = 0;
					unsigned siid = 0;
					auto cmd = static_cast<uint8_t>(data[1]);
					switch (static_cast<si::Command>(cmd)) {
					case si::Command:: SICard5Detected:
					case si::Command:: SICard6Detected:
					case si::Command:: SICard8Detected:
						card_serie = bytes_to_uint(data.mid(5, 1));
						siid = bytes_to_uint(data.mid(6, 3));
						break;
					case si::Command:: SICardRemoved:
						return;
					default:
						append_log(tr("Invalid command %1 received").arg(byte_to_hex(cmd)));
						return;
					}
					append_log(tr("SI card: %1 serie: %2").arg(siid).arg(card_serie));
				});

				/*
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
				*/
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

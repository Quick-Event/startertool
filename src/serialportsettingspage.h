#pragma once

#include "settingspage.h"

#include <QSerialPort>

struct SerialPortSettings
{
	bool enabled = false;
	QString deviceName;
	int baudRate = 38400;
	QSerialPort::DataBits dataBits = QSerialPort::Data8;
	QSerialPort::Parity parity = QSerialPort::NoParity;
	QSerialPort::StopBits stopBits = QSerialPort::OneStop;

	bool operator==(const SerialPortSettings&) const = default;
};

namespace Ui {
class SerialPortSettingsPage;
}

class SerialPortSettingsPage : public SettingsPage
{
	Q_OBJECT
	using Super = SettingsPage;
public:
	explicit SerialPortSettingsPage(QWidget *parent = nullptr);
	~SerialPortSettingsPage();

	static SerialPortSettings loadSettings();
	Q_SIGNAL void serialPortSettingsChanged();

	void load() override;
	void save() override;
private:
	Ui::SerialPortSettingsPage *ui;
};


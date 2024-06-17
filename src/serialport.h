#ifndef SERIALPORT_H
#define SERIALPORT_H

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

class AbstractSerialPort : public QObject
{
	Q_OBJECT
public:
	explicit AbstractSerialPort(QObject *parent = nullptr) : QObject(parent) {}

	virtual void open() = 0;
	virtual void close() = 0;
	virtual QByteArray read() = 0;
	Q_SIGNAL void readyRead();
};

class SerialPort : public AbstractSerialPort
{
	Q_OBJECT
	using Super = AbstractSerialPort;
public:
	explicit SerialPort(const SerialPortSettings &settings, QObject *parent = nullptr);

	void open() override;
	void close() override;
	QByteArray read() override;
private:
	QSerialPort *m_port = nullptr;
};

#endif // SERIALPORT_H

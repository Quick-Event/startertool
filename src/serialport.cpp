#include "serialport.h"

SerialPort::SerialPort(const SerialPortSettings &settings, QObject *parent)
	: Super(parent)
{
	m_port = new QSerialPort(settings.deviceName, this);
	m_port->setBaudRate(settings.baudRate);
	m_port->setDataBits(settings.dataBits);
	m_port->setParity(settings.parity);
	m_port->setStopBits(settings.stopBits);
	connect(m_port, &QSerialPort::readyRead, this, &SerialPort::readyRead);
}

void SerialPort::open()
{
	if (!m_port->open(QIODevice::ReadOnly)) {
		throw std::runtime_error("Cannot open serial port for reading");
	}
}

void SerialPort::close()
{
	m_port->close();
}

QByteArray SerialPort::read()
{
	return m_port->readAll();
}

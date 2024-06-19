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

#ifdef ANDROID
#include <QJniObject>

class SerialPort : public QObject
{
	Q_OBJECT
	using Super = QObject;
public:
	explicit SerialPort(const SerialPortSettings &settings, QObject *parent = nullptr);
	~SerialPort() override;

	void open();
	void close();
	QByteArray read();
	Q_SIGNAL void readyRead();
	QString errorString() const { return m_errorString; }
private:
	void onNewDataArrived(QByteArray data);
	void onExceptionArrived(QString strA);
	static void registerJniNativeMethods();
private:
	QJniObject m_port;
	SerialPortSettings m_settings;
	QByteArray m_readData;
	QString m_errorString;
};

#else

class SerialPort : public QObject
{
	Q_OBJECT
	using Super = QObject;
public:
	explicit SerialPort(const SerialPortSettings &settings, QObject *parent = nullptr);

	void open();
	void close();
	QByteArray read();
	Q_SIGNAL void readyRead();
	QString errorString() const;
private:
	QSerialPort *m_port = nullptr;
};

#endif

#endif // SERIALPORT_H

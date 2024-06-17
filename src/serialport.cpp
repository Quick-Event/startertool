#include "serialport.h"

#ifdef ANDROID
#include <QCoreApplication>

static char V_jniClassName[] {"org/quickbox/startertool/SerialPort"};

static void jniDeviceNewData(JNIEnv *envA, jobject thizA, jlong userDataA, jbyteArray dataA)
{
	Q_UNUSED(thizA);

	if (userDataA != 0) {
		jbyte *bytesL = envA->GetByteArrayElements(dataA, NULL);
		jsize lenL = envA->GetArrayLength(dataA);
		((SerialPort *)userDataA)->newDataArrived((char *)bytesL, lenL);
		envA->ReleaseByteArrayElements(dataA, bytesL, JNI_ABORT);
	}
}

static void jniDeviceException(JNIEnv *envA, jobject thizA, jlong userDataA, jstring messageA)
{
	Q_UNUSED(thizA);

	if (userDataA != 0) {
		const char *stringL = envA->GetStringUTFChars(messageA, NULL);
		QString strL = QString::fromUtf8(stringL);
		envA->ReleaseStringUTFChars(messageA, stringL);
		if (envA->ExceptionCheck()) {
			envA->ExceptionClear();
		}
		((SerialPort *)userDataA)->exceptionArrived(strL);
	}
}

SerialPort::SerialPort(const SerialPortSettings &settings, QObject *parent)
	: Super(parent)
	, m_settings(settings)
{
	m_port = QJniObject::callStaticObjectMethod(V_jniClassName,
												"create",
												"(J)Lorg/quickbox/startertool/SerialPort;",
												(jlong)this);
}

SerialPort::~SerialPort()
{
	close();
}

void SerialPort::open()
{
	m_port.callMethod<void>("test", "()V");
	m_port.callMethod<void>("open", "(Landroid/content/Context;)V", QNativeInterface::QAndroidApplication::context());
	if (!errorString().isEmpty()) {
		throw std::runtime_error("Cannot open serial port for reading: " + errorString().toStdString());
	}
}

void SerialPort::close()
{
	if (m_port.isValid()) {
		m_port.callMethod<void>("close", "()V");
	}
}

QByteArray SerialPort::read()
{
	auto ret = m_readData;
	m_readData.clear();
	return ret;
}

void SerialPort::registerJniNativeMethods()
{
	static bool registered = false;
	if (!registered) {
		registered = true;
		// register the native methods first, ideally it better be done with the app start
		const JNINativeMethod methods[] = {
			{"nativeDeviceNewData", "(J[B)V", reinterpret_cast<void *>(jniDeviceNewData)},
			{"nativeDeviceException", "(JLjava/lang/String;)V", reinterpret_cast<void *>(jniDeviceException)}
		};
		QJniEnvironment env;
		env.registerNativeMethods(V_jniClassName, methods, 2);
	}
}

void SerialPort::newDataArrived(char *bytesA, int lengthA)
{
	m_readData.append(bytesA, lengthA);
	emit readyRead();
}



void SerialPort::exceptionArrived(QString strA)
{
	m_errorString = strA;
}

#else
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
#endif



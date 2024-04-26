#pragma once

#include <QApplication>
#include <QTimeZone>
#include <QUrl>
#include <QFont>

namespace shv::iotqt::rpc { class ClientConnection; }
class AppCliOptions;
class QWidget;

class Application : public QApplication
{
	Q_OBJECT
	using Super = QApplication;

public:
	static const QString DEFAULT_LOCALE;

	Application(int &argc, char **argv, AppCliOptions* cli_opts);
	~Application() override;

	static Application *instance() { return qobject_cast<Application*>(Super::instance()); }
	static void applyCssStyleClass(QWidget *widget, const QString &css_class_name);
	AppCliOptions *cliOptions() { return m_cliOptions; }

	void connectToBroker(const QUrl &connection_url);
	Q_SIGNAL void brokerConnectedChanged(bool is_connected, const QString &error);
private:
	void loadStyle();

private:
	shv::iotqt::rpc::ClientConnection *m_rpcConnection = nullptr;
	AppCliOptions *m_cliOptions = nullptr;
};


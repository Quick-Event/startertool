#include "application.h"
#include "appclioptions.h"
#include <shv/iotqt/rpc/clientconnection.h>

#include <shv/coreqt/log.h>

#ifdef Q_OS_WIN
	#include <QStyleFactory>
	#include "shlwapi.h"
#endif

#include <QFile>
#include <QStyle>

Application::Application(int &argc, char **argv, AppCliOptions* cli_opts)
	: Super(argc, argv)
	, m_cliOptions(cli_opts)
{
#ifdef Q_OS_WIN
		// set default style to fusion to overcome ugly look on some Windows installations
	for(auto key : QStyleFactory::keys()) {
		if(key == QLatin1String("Fusion")) {
			setStyle(QStyleFactory::create(key));
			break;
		}
	}
#endif

	loadStyle();
}

Application::~Application()
{

}

void Application::connectToBroker(const QUrl &connection_url)
{
	if (m_rpcConnection) {
		m_rpcConnection->disconnect();
		m_rpcConnection->deleteLater();
	}
	m_rpcConnection = new shv::iotqt::rpc::ClientConnection(this);
	connect(m_rpcConnection, &shv::iotqt::rpc::ClientConnection::brokerConnectedChanged, this, [this](bool is_connected) {
		emit brokerConnectedChanged(is_connected, {});
	});
	connect(m_rpcConnection, &shv::iotqt::rpc::ClientConnection::brokerLoginError, this, [this](auto err) {
		emit brokerConnectedChanged(false, QString::fromStdString(err.toString()));
	});
	m_rpcConnection->setConnectionUrl(connection_url);
	m_rpcConnection->open();
}

void Application::loadStyle()
{
	QFile file(":/style/style.css");
	if(!file.open(QIODevice::ReadOnly)) {
		shvWarning() << "Failed to load style.css";
	}
	else {
		setStyleSheet(file.readAll());
	}

	file.close();
}

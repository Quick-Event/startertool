#include "application.h"
#include "appclioptions.h"

#include <shv/iotqt/rpc/clientconnection.h>
#include <shv/coreqt/log.h>
#include <shv/chainpack/rpc.h>

#ifdef Q_OS_WIN
	#include <QStyleFactory>
	#include "shlwapi.h"
#endif

#include <QFile>
#include <QStyle>
#include <QUrlQuery>

#include <shv/iotqt/rpc/rpccall.h>

using namespace shv::chainpack;

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

bool Application::isBrokerConnected() const
{
	return m_rpcConnection != nullptr && m_rpcConnection->isBrokerConnected();
}

void Application::connectToBroker(const QUrl &connection_url)
{
	m_eventName = QUrlQuery(connection_url.query()).queryItemValue("event");
	if (m_rpcConnection) {
		m_rpcConnection->disconnect();
		m_rpcConnection->deleteLater();
	}
	m_rpcConnection = new shv::iotqt::rpc::ClientConnection(this);
	connect(m_rpcConnection, &shv::iotqt::rpc::ClientConnection::brokerConnectedChanged, this, [this](bool is_connected) {
		if (is_connected) {
			loadCurrentStageConfig();
		}
		emit brokerConnectedChanged(is_connected, {});
	});
	connect(m_rpcConnection, &shv::iotqt::rpc::ClientConnection::brokerLoginError, this, [this](auto err) {
		emit brokerConnectedChanged(false, QString::fromStdString(err.toString()));
	});
	m_rpcConnection->setConnectionUrl(connection_url);
	m_rpcConnection->open();
}

QDateTime Application::currentStageStart() const
{
	return currentStageConfig().value("startDateTime").toDateTime();
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

void Application::loadCurrentStageConfig()
{
	callShvApiMethod("event/currentStage/config", Rpc::METH_GET, {}, this,
		[this](const RpcValue &result) {
			m_currentStageConfig = shv::coreqt::rpc::rpcValueToQVariant(result).toMap();
		}
	);
}

void Application::callShvMethod(const QString &shv_path,
								const QString &method,
								const QVariant &params,
								const QObject *context,
								std::function<void (const RpcValue &)> success_callback,
								std::function<void (const RpcError &)> error_callback)
{
	if(isBrokerConnected()) {
		auto *rpcc = shv::iotqt::rpc::RpcCall::create(m_rpcConnection)
				->setShvPath(shv_path)
				->setMethod(method)
				->setParams(shv::coreqt::rpc::qVariantToRpcValue(params));
		if (rpcc) {
			if(success_callback) {
				connect(rpcc, &shv::iotqt::rpc::RpcCall::result, context, [success_callback](const ::RpcValue &result) {
					success_callback(result);
				});
			}
			if(error_callback) {
				connect(rpcc, &shv::iotqt::rpc::RpcCall::error, context, [error_callback](const ::RpcError &error) {
					error_callback(error);
				});
			}
			else {
				connect(rpcc, &shv::iotqt::rpc::RpcCall::error, context, [shv_path, method](const ::RpcError &error) {
					shvError() << "RPC method call error, path:" << shv_path << "method:" << method << "error:" << error.toString();
				});
			}
			rpcc->start();
		}
		else {
			shvWarning() << shv_path << method << "RPC connection is not open";
			if(error_callback)
				error_callback(RpcError("RPC connection is not open"));
		}
	}
	else if(error_callback) {
		error_callback(RpcError("Connection is not open"));
	}
}

void Application::callShvApiMethod(const QString &shvapi_path,
								   const QString &method,
								   const QVariant &params,
								   const QObject *context,
								   std::function<void (const RpcValue &)> success_callback,
								   std::function<void (const RpcError &)> error_callback)
{
	QString shv_path = "test/" + m_eventName + '/' + shvapi_path;
	callShvMethod(shv_path, method, params, context, success_callback, error_callback);
}


#include "application.h"

#include "appclioptions.h"
#include "mainwindow.h"
#include "uisettingspage.h"

#include <shv/iotqt/rpc/clientconnection.h>
#include <shv/coreqt/log.h>
#include <shv/chainpack/rpc.h>
#include <shv/iotqt/rpc/rpccall.h>

#ifdef Q_OS_WIN
	#include <QStyleFactory>
	#include "shlwapi.h"
#endif

#include <QFile>
#include <QStyle>
#include <QUrlQuery>
#include <QSettings>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

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
	{
		auto *tm = new QTimer(this);
		connect(tm, &QTimer::timeout, this, [this]() {
			auto dt = QDateTime::currentDateTime();
			auto tm = dt.time();
			if (tm.second() != m_currentTime.time().second()) {
				auto tm = dt.time();
				m_currentTime = QDateTime(dt.date(), QTime(tm.hour(), tm.minute(), tm.second()));
				emit currentTimeChanged(dt);
			}
		});
		tm->start(100);
	}
}

MainWindow *Application::mainWindow()
{
	const QWidgetList all_widgets = QApplication::allWidgets();
	for (QWidget *w : all_widgets) {
		if (auto *mw = qobject_cast<MainWindow*>(w)) {
			return mw;
		}
	}
	Q_ASSERT(false);
	return nullptr;
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
	connect(m_rpcConnection, &shv::iotqt::rpc::ClientConnection::rpcMessageReceived, this, &Application::onRpcMessageReceived);
	connect(m_rpcConnection, &shv::iotqt::rpc::ClientConnection::brokerConnectedChanged, this, [this](bool is_connected) {
		if (is_connected) {
			subscribeChanges();
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
	return currentStageConfig().startTime;
}

void Application::updateRun(int run_id, const QVariant &record)
{
	callShvApiMethod("event/currentStage/runs", "setRecord", QVariantList{run_id, record});
}

void Application::setCardRead(unsigned int siid)
{
	if (siid == m_cardRead)
		return;
	m_cardRead = siid;
	if (siid > 0) {
		playAlert(Application::Alert::CardInserted);
	}
	emit cardReadChanged(m_cardRead);
}

void Application::playAlert(Alert alert)
{
	auto settings = UiSettingsPage::loadSettings();
	switch (alert) {
	case Alert::CardInserted: {
		if (auto file = settings.soundCardInserted; !file.isEmpty()) {
			playSound(file);
		}
		break;
	}
	case Alert::CorridorTimeCheckError: {
		if (auto file = settings.soundCorridorTimeError; !file.isEmpty()) {
			playSound(file);
		}
		break;
	}
	}
}

void Application::playSound(const QString &file)
{
	auto *player = findChild<QMediaPlayer*>();
	if (!player) {
		player = new QMediaPlayer(this);
		auto *audio_output = new QAudioOutput(player);
		audio_output->setVolume(50);
		player->setAudioOutput(audio_output);
		connect(player, &QMediaPlayer::errorOccurred, [file](QMediaPlayer::Error error, const QString &error_string) {
			shvError() << file << "Media player error:" << static_cast<int>(error) << error_string;
		});
	}
	player->setSource(QUrl(file));
	player->play();
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
}

void Application::subscribeChanges()
{
	QString sig = "runchng";
	QString source = "record";
	auto *rpc_call = shv::iotqt::rpc::RpcCall::createSubscriptionRequest(m_rpcConnection, {}, sig, source);
	rpc_call->start();
}

void Application::loadCurrentStageConfig()
{
	callShvApiMethod("event/currentStage/config", Rpc::METH_GET, {}, this,
		[this](const RpcValue &result) {
			auto config = shv::coreqt::rpc::rpcValueToQVariant(result).toMap();
			m_currentStageConfig.stageNumber = config.value("id").toInt();
			//m_currentStageConfig.startTime = config.value("startDateTime").toDateTime();
			auto dtm = QDateTime::currentDateTime();
			auto tm = dtm.time();
			m_currentStageConfig.startTime = QDateTime(dtm.date(), QTime(tm.hour(), tm.minute(), 0)).addSecs(-5*60);
		}
	);
}

void Application::onRpcMessageReceived(const shv::chainpack::RpcMessage &msg)
{
	if (msg.isSignal()) {
		//shvInfo() << "SIG---->" << msg.toCpon();
		RpcSignal sig(msg);
		if (sig.method().asString() == "runchng" && sig.source() == "record") {
			const auto param = sig.params();
			const auto &lst = param.asList();
			auto run_id = lst.value(0).toInt();
			auto record = lst.value(1);
			emit runChanged(run_id, shv::coreqt::rpc::rpcValueToQVariant(record));
		}
	}
}

void Application::callShvMethod(const QString &shv_path,
								const QString &method,
								const QVariant &params,
								const QObject *context,
								std::function<void (const RpcValue &)> success_callback,
								std::function<void (const RpcError &)> error_callback)
{
	shvLogFuncFrame() << shv_path << method;
	if(isBrokerConnected()) {
		auto *rpcc = shv::iotqt::rpc::RpcCall::create(m_rpcConnection)
				->setShvPath(shv_path)
				->setMethod(method)
				->setParams(shv::coreqt::rpc::qVariantToRpcValue(params));
		if (rpcc) {
			if ((success_callback || error_callback) && !context) {
				shvError() << "Cannot use calbacks without context object set.";
				return;
			}
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
				connect(rpcc, &shv::iotqt::rpc::RpcCall::error, [shv_path, method](const ::RpcError &error) {
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


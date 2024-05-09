#pragma once

#include <necrologlevel.h>

#include <QApplication>
#include <QTimeZone>
#include <QUrl>
#include <QFont>

namespace shv::iotqt::rpc { class ClientConnection; }
namespace shv::chainpack { class RpcValue; class RpcError; class RpcMessage; }

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
	bool isBrokerConnected() const;

	void callShvMethod(const QString &shv_path, const QString &method, const QVariant &params = QVariant(),
						const QObject *context = nullptr,
						std::function<void(const shv::chainpack::RpcValue &)> success_callback = nullptr,
						std::function<void (const shv::chainpack::RpcError &)> error_callback = nullptr);
	void callShvApiMethod(const QString &shvapi_path, const QString &method, const QVariant &params = QVariant(),
						const QObject *context = nullptr,
						std::function<void(const shv::chainpack::RpcValue &)> success_callback = nullptr,
						std::function<void (const shv::chainpack::RpcError &)> error_callback = nullptr);

	void connectToBroker(const QUrl &connection_url);
	Q_SIGNAL void brokerConnectedChanged(bool is_connected, const QString &error);

	QVariantMap currentStageConfig() const { return m_currentStageConfig; }
	QDateTime currentStageStart() const;

	Q_SIGNAL void showErrorRq(const QString &msg, NecroLogLevel level);
	void showError(const QString &msg, NecroLogLevel level) { emit showErrorRq(msg, level); }
	void hideError() { showError({}, NecroLogLevel::Invalid); }

	Q_SIGNAL void settingsChanged();
	void emitSettingsChanged() { emit settingsChanged(); }

	Q_SIGNAL void runChanged(int run_id, const QVariant &record);
private:
	void loadStyle();
	void subscribeChanges();
	void loadCurrentStageConfig();
	void onRpcMessageReceived(const shv::chainpack::RpcMessage &msg);
private:
	shv::iotqt::rpc::ClientConnection *m_rpcConnection = nullptr;
	AppCliOptions *m_cliOptions = nullptr;
	QString m_eventName;
	QVariantMap m_currentStageConfig;
};


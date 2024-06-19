#include "logindialogwidget.h"
#include "ui_logindialogwidget.h"

#include "application.h"
#include "appclioptions.h"

#include <QSettings>
#include <QTimer>
#include <QUrlQuery>

LoginDialogWidget::LoginDialogWidget(AutoconnectEnabled autoconnect_enbled, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::LoginDialogWidget)
{
	ui->setupUi(this);

	auto *app = Application::instance();
	connect(app, &Application::brokerConnectedChanged, this, [this](bool is_connected, const QString ) {
		if (is_connected) {
			deleteLater();
		}
	});

	QString url_str;
	if (app->cliOptions()->serverHost_isset()) {
		url_str = QString::fromStdString(app->cliOptions()->serverHost());
	}
	QSettings settings;
	if (url_str.isEmpty()) {
		url_str = settings.value("brokerUrl").toString();
	}
	if (url_str.isEmpty()) {
		url_str = "tcp://nirvana.elektroline.cz:3756";
	}
	auto url = QUrl(url_str);
	if (url.isValid()) {
		auto user = url.userName();
		url.setUserName(QString());
		auto password = url.password();
		url.setPassword(QString());
		auto query = QUrlQuery(url);
		url.setQuery(QString());
		if (user.isEmpty()) {
			user = query.queryItemValue("user");
		}
		if (password.isEmpty()) {
			password = query.queryItemValue("password");
		}
		auto event_path = query.queryItemValue("event_path");
		if (event_path.isEmpty()) {
			event_path = "QE";
		}
		auto api_key = query.queryItemValue("api_key");
		ui->url->setText(url.toString());
		ui->user->setText(user);
		ui->password->setText(password);
		ui->eventPath->setText(event_path);
		ui->shvApiKey->setText(api_key);
	}
	auto auto_connect = settings.value("autoConnect").toBool();
	ui->autoConnect->setChecked(auto_connect);
	connect(ui->btConnect, &QPushButton::clicked, this, [this]() {
		connectToBroker();
	});
	if (autoconnect_enbled == AutoconnectEnabled::Yes) {
		QTimer::singleShot(0, this, &LoginDialogWidget::checkAutoConnect);
	}
}

LoginDialogWidget::~LoginDialogWidget()
{
	QSettings settings;
	settings.setValue("brokerUrl", connectionUrl().toString());
	settings.setValue("autoConnect", ui->autoConnect->isChecked());

	delete ui;
}

void LoginDialogWidget::checkAutoConnect()
{
	auto auto_connect = ui->autoConnect->isChecked();
	if (auto_connect) {
		connectToBroker();
	}
}

QUrl LoginDialogWidget::connectionUrl() const
{
	QUrl ret(ui->url->text());
	QUrlQuery q;
	q.addQueryItem("user", ui->user->text());
	q.addQueryItem("password", ui->password->text());
	q.addQueryItem("event_path", ui->eventPath->text());
	q.addQueryItem("api_key", ui->shvApiKey->text());
	ret.setQuery(q);
	return ret;
}

void LoginDialogWidget::connectToBroker()
{
	auto *app = Application::instance();
	app->showError(tr("Connecting to broker"), NecroLogLevel::Info);
	app->connectToBroker(connectionUrl());
}


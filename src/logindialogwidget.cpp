#include "logindialogwidget.h"
#include "ui_logindialogwidget.h"

#include "application.h"
#include "appclioptions.h"

#include <shv/coreqt/log.h>

#include <QSettings>
#include <QTimer>
#include <QUrlQuery>
#include <QClipboard>

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
	ui->url->setText(url_str);
	parseConnectionUrl();
	auto auto_connect = settings.value("autoConnect").toBool();
	ui->autoConnect->setChecked(auto_connect);
	connect(ui->btConnect, &QPushButton::clicked, this, [this]() {
		connectToBroker();
	});
	if (autoconnect_enbled == AutoconnectEnabled::Yes) {
		QTimer::singleShot(0, this, &LoginDialogWidget::checkAutoConnect);
	}
	connect(ui->url, &QLineEdit::editingFinished, this, &LoginDialogWidget::parseConnectionUrl);
	connect(ui->btPasteUrl, &QAbstractButton::clicked, this, [this]() {
		auto urlstr = QGuiApplication::clipboard()->text();
		ui->url->setText(urlstr);
		parseConnectionUrl();
	});
}

LoginDialogWidget::~LoginDialogWidget()
{
	QSettings settings;
	settings.setValue("brokerUrl", connectionUrl().toString());
	settings.setValue("autoConnect", ui->autoConnect->isChecked());

	delete ui;
}

void LoginDialogWidget::parseConnectionUrl()
{
	auto url_str = ui->url->text();
	auto url = QUrl(url_str);
	if (url.isValid()) {
		auto user = url.userName();
		url.setUserName(QString());
		auto password = url.password();
		url.setPassword(QString());
		auto event_path = url.path().mid(1); // cut leading slash
		url.setPath({});
		auto query = QUrlQuery(url);
		url.setQuery(QString());
		if (user.isEmpty()) {
			user = query.queryItemValue("user");
		}
		if (password.isEmpty()) {
			password = query.queryItemValue("password");
		}
		auto api_key = query.queryItemValue("api_key");
		ui->url->setText(url.toString());
		ui->user->setText(user);
		ui->password->setText(password);
		ui->eventPath->setText(event_path);
		ui->shvApiKey->setText(api_key);
	}
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
	ret.setPath('/' + ui->eventPath->text());
	QUrlQuery q;
	q.addQueryItem("user", ui->user->text());
	q.addQueryItem("password", ui->password->text());
	q.addQueryItem("api_key", ui->shvApiKey->text());
	ret.setQuery(q);
	// shvError() << "connection url:" << ret.toString();
	return ret;
}

void LoginDialogWidget::connectToBroker()
{
	auto *app = Application::instance();
	app->showError(tr("Connecting to broker"), NecroLogLevel::Info);
	app->connectToBroker(connectionUrl());
}


#include "startlistwidget.h"
#include "ui_startlistwidget.h"

#include "application.h"
#include "rpcsqlresultmodel.h"

using namespace shv::chainpack;
using namespace shv::coreqt::data;

StartListWidget::StartListWidget(QWidget *parent) :
	QWidget(parent)
	, ui(new Ui::StartListWidget)
	, m_model(new RpcSqlResultModel(this))
{
	ui->setupUi(this);
	ui->tableView->setModel(m_model);

	auto *app = Application::instance();
	connect(app, &Application::brokerConnectedChanged, this, [this](bool is_connected, const QString &) {
		if (is_connected) {
			reload();
		}
	});
}

StartListWidget::~StartListWidget()
{
	delete ui;
}

void StartListWidget::reload()
{
	auto *app = Application::instance();
	app->callShvApiMethod("event/currentStage/startList", "table", {}, this,
		[this](const RpcValue &result) {
			auto res = RpcSqlResult::fromRpcValue(result);
			m_model->setResult(res);
		}
	);
}

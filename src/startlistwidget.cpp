#include "startlistwidget.h"
#include "ui_startlistwidget.h"

#include "startlistitemdelegate.h"
#include "application.h"
#include "rpcsqlresultmodel.h"

#include <QResizeEvent>
#include <QScrollBar>

using namespace shv::chainpack;
using namespace shv::coreqt::data;

StartListWidget::StartListWidget(QWidget *parent) :
	QWidget(parent)
	, ui(new Ui::StartListWidget)
	, m_model(new StartListModel(this))
{
	ui->setupUi(this);
	ui->tableView->setModel(m_model);
	ui->tableView->viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
	ui->tableView->setItemDelegate(new StartListItemDelegate(this));

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

void StartListWidget::resizeEvent(QResizeEvent *event)
{
	Super::resizeEvent(event);
	updateHeadersSectionSizes();
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

void StartListWidget::updateHeadersSectionSizes()
{
	{
		auto *hh = ui->tableView->horizontalHeader();
		Q_ASSERT(hh);
		auto w = ui->tableView->geometry().width();
		if (auto *sb = ui->tableView->verticalScrollBar(); sb && sb->isVisible()) {
			w -= sb->width();
		}
		w -= 5;
		hh->resizeSection(0, w);
	}
	{
		auto font = ui->tableView->font();
		QFontMetrics m(font);
		auto h = m.height();
		auto *vh = ui->tableView->verticalHeader();
		Q_ASSERT(vh);
		vh->setDefaultSectionSize(2 * h);
	}
}

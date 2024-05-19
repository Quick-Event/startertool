#include "startlistwidget.h"
#include "ui_startlistwidget.h"

#include "mainwindow.h"
#include "startlistitemdelegate.h"
#include "application.h"
#include "startlistmodel.h"
#include "classfiltersettingspage.h"
#include "runwidget.h"

#include <shv/coreqt/log.h>

#include <QResizeEvent>
#include <QScrollBar>
#include <QTimer>

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
	connect(app, &Application::settingsChanged, this, &StartListWidget::reload);
	connect(ui->tableView, &StartListTableView::editButtonPressed, this, [this](int run_id) {
		auto *widget = new RunWidget(m_model);
		widget->load(run_id);
		Application::instance()->mainWindow()->showDialogWidget(widget);
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
	auto class_filter = ClassFilterSettingsPage::checkedClasses();
	QVariant param;
	if (class_filter.enabled) {
		auto where = QStringLiteral("classes.name IN ('%1')").arg(class_filter.checkedClasses.join("','"));
		param = QVariantMap {{"where", where}};
	}
	auto *app = Application::instance();
	app->callShvApiMethod("event/currentStage/runs", "table", param, this,
		[this](const RpcValue &result) {
		// shvInfo() << result.toCpon("  ");
			auto res = RpcSqlResult::fromRpcValue(result);
			m_model->setResult(res);
			QTimer::singleShot(10, this, &StartListWidget::updateHeadersSectionSizes);
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

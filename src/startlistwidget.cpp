#include "startlistwidget.h"
#include "ui_startlistwidget.h"

#include "mainwindow.h"
#include "startlistitemdelegate.h"
#include "application.h"
#include "startlistmodel.h"
#include "classfiltersettingspage.h"
#include "uisettingspage.h"
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

	connect(ui->tableView, &StartListTableView::manualScroll, this, &StartListWidget::manualScroll);

	ui->tableView->setModel(m_model);
	ui->tableView->viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
	ui->tableView->setItemDelegate(new StartListItemDelegate(this));

	auto *app = Application::instance();
	connect(app, &Application::brokerConnectedChanged, this, [this](bool is_connected, const QString &) {
		if (is_connected) {
			reload();
		}
	});
	connect(m_model, &StartListModel::corridorTimeCheckError, []() {
		Application::instance()->playAlert(Application::Alert::CorridorTimeCheckError);
	});
	connect(app, &Application::settingsChanged, this, [this]() {
		reload();
	});
	connect(ui->tableView, &StartListTableView::editButtonPressed, this, [this](int run_id) {
		auto *widget = new RunWidget(m_model);
		widget->load(run_id);
		Application::instance()->mainWindow()->showDialogWidget(widget);
	});
	connect(ui->tableView, &StartListTableView::corridorTimeButtonPressed, this, [this](int run_id) {
		shvDebug() << "run id:" << run_id;
		if (auto o_row = m_model->runIdToRow(run_id); o_row.has_value()) {
			auto row = o_row.value();
			auto corridor_time = m_model->roleValue(row, StartListModel::Role::CorridorTime).toDateTime();
			shvDebug() << "row:" << run_id << "dt:" << corridor_time.toString();
			auto ui_settings = UiSettingsPage::loadSettings();
			QMap<StartListModel::Role, QVariant> record;
			if (ui_settings.toggleCorridorTime) {
				auto v = corridor_time.isValid()? QVariant(): QVariant(QDateTime::currentDateTime());
				record.insert(StartListModel::CorridorTime, v);
			}
			else {
				if (!corridor_time.isValid()) {
					record.insert(StartListModel::CorridorTime, QDateTime::currentDateTime());
				}
			}
			m_model->updateLocalRecord(run_id, record);
		}
	});
}

StartListWidget::~StartListWidget()
{
	delete ui;
}

void StartListWidget::setSelectedRow(std::optional<int> row)
{
	m_model->setSelectedRow(row);
	if (row.has_value()) {
		auto ix = m_model->index(row.value(), 0);
		ui->tableView->scrollTo(ix);
	}
}

void StartListWidget::scrollToStartTime(const QDateTime &tm)
{
	for (auto row = 0; row < m_model->rowCount(); ++row) {
		auto start = m_model->roleValue(row, StartListModel::Role::StartDateTime).toDateTime();
		if (start >= tm) {
			// shvInfo() << tm << "scroll to:" << start;
			auto ix = m_model->index(row, 0);
			ui->tableView->scrollTo(ix, QTableView::PositionAtTop);
			return;
		}
	}
	// scroll to end
	// shvInfo() << tm << "scroll to end";
	auto ix = m_model->index(m_model->rowCount() - 1, 0);
	ui->tableView->scrollTo(ix, QTableView::PositionAtTop);
}

void StartListWidget::onCardInserted(unsigned int siid)
{
	shvDebug() << "SI id:" << siid;
	for (auto row = 0; row < m_model->rowCount(); ++row) {
		auto siid2 = m_model->roleValue(row, StartListModel::Role::SiId).toUInt();
		if (siid == siid2) {
			auto run_id = m_model->roleValue(row, StartListModel::Role::RunId).toInt();
			QMap<StartListModel::Role, QVariant> record;
			record.insert(StartListModel::CorridorTime, QDateTime::currentDateTime());
			//shvDebug() << "row:" << run_id << "record:" << shv::coreqt::rpc::qVariantToPrettyString();
			m_model->updateLocalRecord(run_id, record);
			setSelectedRow(row);
			return;
		}
	}
	// card not found
	Application::instance()->playAlert(Application::Alert::CardNotFound);
}

void StartListWidget::resizeEvent(QResizeEvent *event)
{
	Super::resizeEvent(event);
	updateHeadersSectionSizes();
}

void StartListWidget::applySettings()
{
	auto ui_settings = UiSettingsPage::loadSettings();
	auto *delegate = findChild<StartListItemDelegate*>();
	Q_ASSERT(delegate);
	delegate->setRelativeStartTime(ui_settings.relativeStartTime);
}

void StartListWidget::reload()
{
	applySettings();
	auto class_filter = ClassFilterSettingsPage::checkedClasses();
	QVariantMap param;
	param["orderBy"] = "runs.startTimeMs";
	if (class_filter.enabled) {
		auto where = QStringLiteral("classes.name IN ('%1')").arg(class_filter.checkedClasses.join("','"));
		param["where"] = where;
	}
	auto *app = Application::instance();
	app->callShvApiMethod("event/currentStage/runs", "table", param, this,
		[this](const RpcValue &result) {
			//shvInfo() << result.toCpon("  ");
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

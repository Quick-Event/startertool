#include "runwidget.h"
#include "ui_runwidget.h"

#include "application.h"
#include "startlistmodel.h"

namespace {
	const auto NULL_TIME = QDateTime::fromSecsSinceEpoch(0);
}
RunWidget::RunWidget(StartListModel *model, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::RunWidget)
	, m_model(model)
{
	ui->setupUi(this);
	ui->corridorTime->setMinimumDateTime(NULL_TIME);
	connect(ui->btBack, &QAbstractButton::clicked, this, [this]() {
		save();
		deleteLater();
	});
	connect(ui->btReload, &QAbstractButton::clicked, this, [this]() {
		load(m_runId);
	});
	connect(ui->btSetCorridorTimeNow, &QAbstractButton::clicked, this, [this]() {
		ui->corridorTime->setDateTime(QDateTime::currentDateTime());
	});
	connect(ui->btClearCorridorTime, &QAbstractButton::clicked, this, [this]() {
		ui->corridorTime->setDateTime(NULL_TIME);
	});
}

RunWidget::~RunWidget()
{
	delete ui;
}

void RunWidget::load(int run_id)
{
	if (auto row = m_model->runIdToRow(run_id); row.has_value()) {
		m_runId = run_id;
		auto i = row.value();
		ui->className->setText(m_model->roleValue(i, StartListModel::ClassName).toString());
		ui->competitorName->setText(m_model->roleValue(i, StartListModel::CompetitorName).toString());
		ui->siId->setValue(m_model->roleValue(i, StartListModel::SiId).toInt());
		ui->registration->setText(m_model->roleValue(i, StartListModel::Registration).toString());
		{
			auto msecs = m_model->roleValue(i, StartListModel::Role::StartTime).toInt();
			auto start00 = Application::instance()->currentStageStart();
			auto start = start00.addMSecs(msecs);
			ui->startTime->setText(start.toString("hh:mm:ss"));
		}
		{
			auto corridor = m_model->roleValue(i, StartListModel::Role::CorridorTime).toDateTime();
			ui->corridorTime->setDateTime(corridor.isValid()? corridor: NULL_TIME);
		}
	}
}

void RunWidget::save()
{
	QMap<StartListModel::Role, QVariant> record;

	record.insert(StartListModel::SiId, ui->siId->value());
	auto corridor_time = ui->corridorTime->dateTime();
	auto v = corridor_time == NULL_TIME? QVariant(): QVariant(corridor_time);
	record.insert(StartListModel::CorridorTime, v);

	m_model->updateLocalRecord(m_runId, record);
}

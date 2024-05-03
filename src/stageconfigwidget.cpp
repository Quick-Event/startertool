#include "stageconfigwidget.h"
#include "ui_stageconfigwidget.h"

#include "application.h"

StageConfigWidget::StageConfigWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::StageConfigWidget)
{
	ui->setupUi(this);

	auto config = Application::instance()->currentStageConfig();
	ui->stageNumber->setText(config.value("id").toString());
	ui->startTime->setText(config.value("startDateTime").toDateTime().toString(Qt::ISODate));

	connect(ui->btBack, &QPushButton::clicked, this, &QObject::deleteLater);
}

StageConfigWidget::~StageConfigWidget()
{
	delete ui;
}

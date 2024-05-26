#include "stagesettingspage.h"
#include "ui_stagesettingspage.h"

#include "application.h"

StageSettingsPage::StageSettingsPage(QWidget *parent)
	: Super(tr("Stage"), parent)
	, ui(new Ui::StageSettingsPage)
{
	ui->setupUi(this);
}

StageSettingsPage::~StageSettingsPage()
{
	delete ui;
}

void StageSettingsPage::load()
{
	const auto& config = Application::instance()->currentStageConfig();
	ui->stageNumber->setText(QString::number(config.stageNumber));
	ui->startTime->setDateTime(config.startTime);
}

void StageSettingsPage::save()
{
}

#include "stagesettingspage.h"
#include "ui_stagesettingspage.h"

#include "application.h"

StageSettingsPage::StageSettingsPage(QWidget *parent)
	: Super(parent)
	, ui(new Ui::StageSettingsPage)
{
	m_caption = tr("Stage");
	ui->setupUi(this);
}

StageSettingsPage::~StageSettingsPage()
{
	delete ui;
}

void StageSettingsPage::load()
{
	auto config = Application::instance()->currentStageConfig();
	ui->stageNumber->setText(config.value("id").toString());
	ui->startTime->setText(config.value("startDateTime").toDateTime().toString(Qt::ISODate));
}

void StageSettingsPage::save()
{
}

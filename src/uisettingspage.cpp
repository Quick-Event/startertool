#include "uisettingspage.h"
#include "ui_uisettingspage.h"

#include "application.h"

#include <QSettings>

UiSettingsPage::UiSettingsPage(QWidget *parent) :
	Super(tr("UI"), parent),
	ui(new Ui::UiSettingsPage)
{
	ui->setupUi(this);
}

UiSettingsPage::~UiSettingsPage()
{
	delete ui;
}

void UiSettingsPage::load()
{
	auto *app = Application::instance();
	auto ui_settings = app->uiSettings();
	ui->toggleCorridorTime->setChecked(ui_settings.toggleCorridorTime);
}

void UiSettingsPage::save()
{
	QSettings settings;
	settings.setValue(UiSettings::TOGGLE_CORRIDOR_TIME, ui->toggleCorridorTime->isChecked());
}

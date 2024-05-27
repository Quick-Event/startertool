#include "uisettingspage.h"
#include "ui_uisettingspage.h"

#include "application.h"

#include <QSettings>

namespace {
constexpr auto TOGGLE_CORRIDOR_TIME = "ui/toggleCorridorTime";
constexpr auto SOUND_CARD_INSERTED = "ui/sound/CardInserted";
constexpr auto SOUND_CORRIDOR_TIME_ERROR = "ui/sound/corridorTimeError";

constexpr auto SOUND_CARD_INSERTED_FILE = "qrc:/sound/dingding.wav";
constexpr auto SOUND_CORRIDOR_TIME_ERROR_FILE = "qrc:/sound/buzz.wav";
}

UiSettingsPage::UiSettingsPage(QWidget *parent) :
	Super(tr("UI"), parent),
	ui(new Ui::UiSettingsPage)
{
	ui->setupUi(this);
	connect(ui->btPlayCardInserted, &QAbstractButton::clicked, this, []() {
		Application::instance()->playSound(SOUND_CARD_INSERTED_FILE);
	});
	connect(ui->btPlayCorridorTimeError, &QAbstractButton::clicked, this, []() {
		Application::instance()->playSound(SOUND_CORRIDOR_TIME_ERROR_FILE);
	});
}

UiSettingsPage::~UiSettingsPage()
{
	delete ui;
}

UiSettings UiSettingsPage::loadSettings()
{
	QSettings settings;
	return UiSettings {
		.toggleCorridorTime = settings.value(TOGGLE_CORRIDOR_TIME).toBool(),
		.soundCardInserted = settings.value(SOUND_CARD_INSERTED).toString(),
		.soundCorridorTimeError = settings.value(SOUND_CORRIDOR_TIME_ERROR).toString(),
	};
}

void UiSettingsPage::load()
{
	auto ui_settings = loadSettings();
	ui->toggleCorridorTime->setChecked(ui_settings.toggleCorridorTime);
	ui->chkSoundCardInserted->setChecked(!ui_settings.soundCardInserted.isEmpty());
	ui->chkSoundCorridorTimeError->setChecked(!ui_settings.soundCorridorTimeError.isEmpty());
}

void UiSettingsPage::save()
{
	QSettings settings;
	settings.setValue(TOGGLE_CORRIDOR_TIME, ui->toggleCorridorTime->isChecked());
	settings.setValue(SOUND_CARD_INSERTED, ui->chkSoundCardInserted->isChecked()? SOUND_CARD_INSERTED_FILE: "");
	settings.setValue(SOUND_CORRIDOR_TIME_ERROR, ui->chkSoundCorridorTimeError->isChecked()? SOUND_CORRIDOR_TIME_ERROR_FILE: "");
}

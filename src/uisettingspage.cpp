#include "uisettingspage.h"
#include "ui_uisettingspage.h"

#include "application.h"

#include <shv/coreqt/log.h>

#include <QSettings>
#include <QComboBox>
#include <QDir>
#include <QFileInfo>

namespace {
constexpr auto TOGGLE_CORRIDOR_TIME = "ui/toggleCorridorTime";

constexpr auto SOUND_CARD_INSERTED = "ui/sound/cardInserted";
constexpr auto SOUND_CARD_NOT_FOUND = "ui/sound/cardNotFound";
constexpr auto SOUND_CORRIDOR_TIME_ERROR = "ui/sound/corridorTimeError";

constexpr auto SOUND_CARD_INSERTED_FILE = "qrc:/sound/ding.wav";
constexpr auto SOUND_CARD_NOT_FOUND_FILE = "qrc:/sound/look.wav";
constexpr auto SOUND_CORRIDOR_TIME_ERROR_FILE = "qrc:/sound/buzz.wav";
}

UiSettingsPage::UiSettingsPage(QWidget *parent) :
	Super(tr("UI"), parent),
	ui(new Ui::UiSettingsPage)
{
	ui->setupUi(this);
	auto load_sounds = [](QComboBox *lst) {
		QDir dir(":/sound");
		lst->addItem(tr("None"));
		for (const QFileInfo &file : dir.entryInfoList(QDir::Files)) {
			lst->addItem(file.baseName(), "qrc:/sound/" + file.fileName());
		}
		lst->setCurrentIndex(0);
	};
	load_sounds(ui->lstSoundCardInserted);
	load_sounds(ui->lstSoundCardNotFound);
	load_sounds(ui->lstSoundCorridorTimeError);
	connect(ui->btPlayCardInserted, &QAbstractButton::clicked, this, [this]() {
		auto fn = ui->lstSoundCardInserted->currentData().toString();
		if (!fn.isEmpty()) {
			Application::instance()->playSound(fn);
		}
	});
	connect(ui->btPlayCardNotFound, &QAbstractButton::clicked, this, [this]() {
		auto fn = ui->lstSoundCardNotFound->currentData().toString();
		if (!fn.isEmpty()) {
			Application::instance()->playSound(fn);
		}
	});
	connect(ui->btPlayCorridorTimeError, &QAbstractButton::clicked, this, [this]() {
		auto fn = ui->lstSoundCorridorTimeError->currentData().toString();
		if (!fn.isEmpty()) {
			Application::instance()->playSound(fn);
		}
	});
}

UiSettingsPage::~UiSettingsPage()
{
	delete ui;
}

UiSettings UiSettingsPage::loadSettings()
{
	QSettings settings;
	//shvInfo() << "soundCardInserted" << settings.value(SOUND_CARD_INSERTED, SOUND_CARD_INSERTED_FILE).toString();
	//shvInfo() << "soundCardNotFound" << settings.value(SOUND_CARD_NOT_FOUND, SOUND_CARD_NOT_FOUND_FILE).toString();
	//shvInfo() << "soundCorridorTimeError" << settings.value(SOUND_CORRIDOR_TIME_ERROR, SOUND_CORRIDOR_TIME_ERROR_FILE).toString();
	return UiSettings {
		.toggleCorridorTime = settings.value(TOGGLE_CORRIDOR_TIME).toBool(),
		.soundCardInserted = settings.value(SOUND_CARD_INSERTED, SOUND_CARD_INSERTED_FILE).toString(),
		.soundCardNotFound = settings.value(SOUND_CARD_NOT_FOUND, SOUND_CARD_NOT_FOUND_FILE).toString(),
		.soundCorridorTimeError = settings.value(SOUND_CORRIDOR_TIME_ERROR, SOUND_CORRIDOR_TIME_ERROR_FILE).toString(),
	};
}

void UiSettingsPage::load()
{
	auto ui_settings = loadSettings();
	ui->toggleCorridorTime->setChecked(ui_settings.toggleCorridorTime);

	auto set_sound_file = [](QComboBox *lst, const QString &file_name) {
		if (auto ix = lst->findData(file_name); ix >= 0) {
			lst->setCurrentIndex(ix);
		}
		else {
			lst->setCurrentIndex(0);
		}
	};
	set_sound_file(ui->lstSoundCardInserted, ui_settings.soundCardInserted);
	set_sound_file(ui->lstSoundCardNotFound, ui_settings.soundCardNotFound);
	set_sound_file(ui->lstSoundCorridorTimeError, ui_settings.soundCorridorTimeError);
}

void UiSettingsPage::save()
{
	QSettings settings;
	settings.setValue(TOGGLE_CORRIDOR_TIME, ui->toggleCorridorTime->isChecked());

	settings.setValue(SOUND_CARD_INSERTED, ui->lstSoundCardInserted->currentData().toString());
	settings.setValue(SOUND_CARD_NOT_FOUND, ui->lstSoundCardNotFound->currentData().toString());
	settings.setValue(SOUND_CORRIDOR_TIME_ERROR, ui->lstSoundCorridorTimeError->currentData().toString());
}

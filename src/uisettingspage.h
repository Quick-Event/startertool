#ifndef UISETTINGSPAGE_H
#define UISETTINGSPAGE_H

#include "settingspage.h"

struct UiSettings
{
	bool toggleCorridorTime = false;
	QString soundCardInserted;
	QString soundCardNotFound;
	QString soundCorridorTimeError;
};

namespace Ui {
class UiSettingsPage;
}

class UiSettingsPage : public SettingsPage
{
	Q_OBJECT
	using Super = SettingsPage;

public:
	explicit UiSettingsPage(QWidget *parent = nullptr);
	~UiSettingsPage();

	static UiSettings loadSettings();

protected:
	void load() override;
	void save() override;
private:
	Ui::UiSettingsPage *ui;
};

#endif // UISETTINGSPAGE_H

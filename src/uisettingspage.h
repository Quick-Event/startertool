#ifndef UISETTINGSPAGE_H
#define UISETTINGSPAGE_H

#include "settingspage.h"

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

protected:
	void load() override;
	void save() override;
private:
	Ui::UiSettingsPage *ui;
};

#endif // UISETTINGSPAGE_H

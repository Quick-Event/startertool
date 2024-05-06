#pragma once

#include "settingspage.h"

namespace Ui {
class StageSettingsPage;
}

class StageSettingsPage : public SettingsPage
{
	Q_OBJECT
using Super = SettingsPage;
public:
	explicit StageSettingsPage(QWidget *parent = nullptr);
	~StageSettingsPage();

	void load() override;
	void save() override;
private:
	Ui::StageSettingsPage *ui;
};


#pragma once

#include "settingspage.h"

namespace Ui {
class AndroidSerialPortSettingsPage;
}

class AndroidSerialPortSettingsPage : public SettingsPage
{
	Q_OBJECT
	using Super = SettingsPage;
public:
	explicit AndroidSerialPortSettingsPage(QWidget *parent = nullptr);
	~AndroidSerialPortSettingsPage();

	void load() override {}
	void save() override {}
private:
	Ui::AndroidSerialPortSettingsPage *ui;
};


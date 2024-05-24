#pragma once

#include "settingspage.h"

namespace Ui {
class SerialPortSettingsPage;
}

class SerialPortSettingsPage : public SettingsPage
{
	Q_OBJECT
	using Super = SettingsPage;
public:
	explicit SerialPortSettingsPage(QWidget *parent = nullptr);
	~SerialPortSettingsPage();

	void load() override;
	void save() override;
private:
	Ui::SerialPortSettingsPage *ui;
};


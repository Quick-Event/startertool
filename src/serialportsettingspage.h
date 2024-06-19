#pragma once

#include "settingspage.h"

namespace Ui {
class SerialPortSettingsPage;
}

struct SerialPortSettings;

class SerialPortSettingsPage : public SettingsPage
{
	Q_OBJECT
	using Super = SettingsPage;
public:
	explicit SerialPortSettingsPage(QWidget *parent = nullptr);
	~SerialPortSettingsPage();

	static SerialPortSettings loadSettings();
	Q_SIGNAL void serialPortSettingsChanged();

	void load() override;
	void save() override;
private:
	void loadPorts();
private:
	Ui::SerialPortSettingsPage *ui;
};


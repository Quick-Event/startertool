#pragma once

#include <QDialog>

class QButtonGroup;


class SettingsPage;

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
	Q_OBJECT

public:
	explicit SettingsWidget(QWidget *parent = nullptr);
	~SettingsWidget();

	void addPage(SettingsPage *page);
private:
	SettingsPage* page(int page_index);
private:
	Ui::SettingsWidget *ui;
	QButtonGroup *m_buttonGroup;
};




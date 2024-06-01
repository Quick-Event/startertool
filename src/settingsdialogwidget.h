#pragma once

#include <QDialog>

class QButtonGroup;


class SettingsPage;

namespace Ui {
class SettingsDialogWidget;
}

class SettingsDialogWidget : public QWidget
{
	Q_OBJECT

public:
	explicit SettingsDialogWidget(QWidget *parent = nullptr);
	~SettingsDialogWidget();

	void addPage(SettingsPage *page);
private:
	SettingsPage* page(int page_index);
private:
	Ui::SettingsDialogWidget *ui;
	QButtonGroup *m_buttonGroup;
};




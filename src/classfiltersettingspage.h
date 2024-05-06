#ifndef CLASSFILTERSETTINGSPAGE_H
#define CLASSFILTERSETTINGSPAGE_H

#include "settingspage.h"

namespace Ui {
class ClassFilterSettingsPage;
}

class ClassFilterSettingsPage : public SettingsPage
{
	Q_OBJECT
	using Super = SettingsPage;
public:
	explicit ClassFilterSettingsPage(QWidget *parent = nullptr);
	~ClassFilterSettingsPage();

	struct ClassFilter
	{
		bool enabled = false;
		QStringList checkedClasses;
	};
	static ClassFilter checkedClasses();
protected:
	void load() override;
	void save() override;
private:
	void loadClassFilter();
private:
	Ui::ClassFilterSettingsPage *ui;
	QStringList m_definedClasses;
};

#endif // CLASSFILTERSETTINGSPAGE_H

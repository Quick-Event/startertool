#ifndef CORE_SETTINGSPAGEWIDGET_H
#define CORE_SETTINGSPAGEWIDGET_H

#include <QWidget>


class SettingsPage : public QWidget
{
	Q_OBJECT

public:
	explicit SettingsPage(const QString &caption, QWidget *parent = nullptr);

	QString caption() const { return m_caption; }

	virtual void load() = 0;
	virtual void save() = 0;
protected:
	QString m_caption;
	//QString m_settingsDir;
};


#endif // CORE_SETTINGSPAGEWIDGET_H

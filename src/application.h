#pragma once

#include <QApplication>
#include <QTimeZone>
#include <QUrl>
#include <QFont>

class AppCliOptions;
class Controller;
class Document;
class QWidget;

class Application : public QApplication
{
	Q_OBJECT
	using Super = QApplication;

public:
	static const QString DEFAULT_LOCALE;

	Application(int &argc, char **argv, AppCliOptions* cli_opts);
	~Application() override;

	static Application *instance() { return qobject_cast<Application*>(Super::instance()); }
	static void applyCssStyleClass(QWidget *widget, const QString &css_class_name);

private:
	void loadStyle();

	AppCliOptions *m_cliOptions = nullptr;
};


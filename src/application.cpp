#include "application.h"
#include "appclioptions.h"

#include <shv/coreqt/log.h>

#ifdef Q_OS_WIN
	#include <QStyleFactory>
	#include "shlwapi.h"
#endif

#include <QFile>
#include <QStyle>

Application::Application(int &argc, char **argv, AppCliOptions* cli_opts)
	: Super(argc, argv)
	, m_cliOptions(cli_opts)
{
#ifdef Q_OS_WIN
		// set default style to fusion to overcome ugly look on some Windows installations
	for(auto key : QStyleFactory::keys()) {
		if(key == QLatin1String("Fusion")) {
			setStyle(QStyleFactory::create(key));
			break;
		}
	}
#endif

	loadStyle();
}

Application::~Application()
{

}

void Application::loadStyle()
{
	QFile file(":/style/style.css");
	if(!file.open(QIODevice::ReadOnly)) {
		shvWarning() << "Failed to load style.css";
	}
	else {
		setStyleSheet(file.readAll());
	}

	file.close();
}

#include "application.h"
#include "appclioptions.h"
#include "mainwindow.h"
#include "version.h"

#include <shv/chainpack/rpcmessage.h>
#include <shv/coreqt/log.h>

#include <QTranslator>
#include <QUrl>

#include <iostream>

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("quickbox");
	QCoreApplication::setOrganizationDomain("quickbox.cz");
	QCoreApplication::setApplicationName("StarterTool");
	QCoreApplication::setApplicationVersion(APP_VERSION);

	//NecroLog::registerTopic("MouseSelection", "Graph mouse selection");

	std::vector<std::string> shv_args = NecroLog::setCLIOptions(argc, argv);

	AppCliOptions cli_opts;

	cli_opts.parse(shv_args);
	if(cli_opts.isParseError()) {
		for(const std::string &err : cli_opts.parseErrors())
			shvError() << err;
		return EXIT_FAILURE;
	}
	if(cli_opts.isAppBreak()) {
		if(cli_opts.isHelp()) {
			cli_opts.printHelp(std::cout);
		}
		return EXIT_SUCCESS;
	}
	for(const std::string &s : cli_opts.unusedArguments()) {
		shvWarning() << "Undefined argument:" << s;
	}

	if(!cli_opts.loadConfigFile()) {
		return EXIT_FAILURE;
	}

	shv::chainpack::RpcMessage::registerMetaTypes();

	shvInfo() << "======================================================================================";
	shvInfo() << "Starting application" << QCoreApplication::applicationName()
			  << "ver:" << QCoreApplication::applicationVersion();
#ifdef GIT_COMMIT
	shvInfo() << "GIT commit:" << SHV_EXPAND_AND_QUOTE(GIT_COMMIT);
#endif
	shvInfo() << QDateTime::currentDateTime().toString(Qt::ISODate).toStdString() << "UTC:" << QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
	shvInfo() << "======================================================================================";
	shvInfo() << "Log tresholds:" << NecroLog::thresholdsLogInfo();
	shvInfo() << "--------------------------------------------------------------------------------------";

	Application app(argc,argv, &cli_opts);

	MainWindow w;
	w.show();
	return app.exec();
}

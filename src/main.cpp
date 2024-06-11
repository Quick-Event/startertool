#include "application.h"
#include "appclioptions.h"
#include "mainwindow.h"
#include "version.h"

#include <shv/chainpack/rpcmessage.h>
#include <shv/coreqt/log.h>

#include <QTranslator>
#include <QUrl>

#include <iostream>

#ifdef __ANDROID__
#include <android/log.h>
#include <necrolog.h>

constexpr int to_android_log_level(const NecroLog::Level level)
{
	switch (level) {
	case NecroLogLevel::Invalid: return ANDROID_LOG_UNKNOWN;
	case NecroLogLevel::Fatal: return ANDROID_LOG_FATAL;
	case NecroLogLevel::Error: return ANDROID_LOG_ERROR;
	case NecroLogLevel::Warning: return ANDROID_LOG_WARN;
	case NecroLogLevel::Info: return ANDROID_LOG_INFO;
	case NecroLogLevel::Message: return ANDROID_LOG_VERBOSE;
	case NecroLogLevel::Debug: return ANDROID_LOG_DEBUG;
	}
	return ANDROID_LOG_UNKNOWN;
}

#define ANDROID_LOG(level, format, ...) __android_log_print(to_android_log_level(level), "flatline", format __VA_OPT__(,) __VA_ARGS__)

#else
#define ANDROID_LOG
#endif

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

#ifdef __ANDROID__
	NecroLog::MessageHandler old_message_handler;
	old_message_handler = NecroLog::setMessageHandler([&old_message_handler] (NecroLog::Level level, const NecroLog::LogContext &context, const std::string &msg) {
		std::ostringstream oss;
		NecroLog::writeWithDefaultFormat(oss, true, level, context, msg);
		// To see this log, use `adb logcat -T1 -s eyasmobile:D`.
		ANDROID_LOG(level, "%s", oss.str().c_str());
		old_message_handler(level, context, msg);
	});
#endif

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

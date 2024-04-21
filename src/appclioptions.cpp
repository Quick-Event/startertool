#include "appclioptions.h"

namespace cp = shv::chainpack;

AppCliOptions::AppCliOptions()
{
	serverHost_optionRef().setDefaultValue("nirvana.elektroline.cz:3756");
	//serverPort_optionRef().setDefaultValue(3756);
	loginType_optionRef().setDefaultValue("sha1");

	//addOption("app.predefinedHosts").setType(cp::RpcValue::Type::String).setNames("--predefined-hosts").setComment("")
	//		.setDefaultValue(
	//			"Nirvana;tcp://nirvana.elektroline.cz:3756,"
	//			"Nirvana-ssl;ssl://nirvana.elektroline.cz:37555"
	//		);
}

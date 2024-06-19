#include "appclioptions.h"

namespace cp = shv::chainpack;

AppCliOptions::AppCliOptions()
{
	//serverHost_optionRef().setDefaultValue("nirvana.elektroline.cz:3756");
	//serverPort_optionRef().setDefaultValue(3756);
	//loginType_optionRef().setDefaultValue("sha1");

	addOption("debug.fakeStageStartTime").setType(cp::RpcValue::Type::Bool).setNames("--fst", "--fake-stage-start-time")
			.setComment("Set stage start time to current time to enable development.")
			.setDefaultValue( false );
}

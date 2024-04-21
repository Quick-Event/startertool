#pragma once
#include <shv/iotqt/rpc/clientappclioptions.h>

class AppCliOptions : public shv::iotqt::rpc::ClientAppCliOptions
{
	using Super = shv::iotqt::rpc::ClientAppCliOptions;
public:
	AppCliOptions();

	//CLIOPTION_GETTER_SETTER2(std::string, "app.predefinedHosts", p, setP, redefinedHosts)
};

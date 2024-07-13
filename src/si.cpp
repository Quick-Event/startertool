#include "si.h"

#include <shv/coreqt/log.h>
#include <stdexcept>

namespace si {

std::tuple<unsigned, Command> parseDetectMessageData(QByteArrayView data)
{
	for (auto b : data) {
		shvDebug() << "\t" << QStringLiteral("%1").arg(static_cast<uint8_t>(b), 2, 16, QChar('0'));
	}
	if (data.isEmpty()) {
		throw std::runtime_error("Empty data");
	}
	if (data[0] != si::STX) {
		throw std::runtime_error("STX not received");
	}
	if (data[data.size() - 1] != si::ETX) {
		throw std::runtime_error("ETX not received");
	}
	auto bytes_to_uint = [](QByteArrayView ba) {
		unsigned ret = 0;
		for(auto b : ba) {
			auto u = static_cast<uint8_t>(b);
			ret = 256 * ret + u;
		}
		return ret;
	};
	unsigned card_serie = 0;
	unsigned siid = 0;
	auto cmd = static_cast<si::Command>(static_cast<uint8_t>(data[1]));
	switch (cmd) {
	case si::Command:: SICard5Detected:
		card_serie = bytes_to_uint(data.mid(6, 1));
		siid = bytes_to_uint(data.mid(7, 2));
		if(card_serie > 1) {
			siid += 100000 * card_serie;
		}
		return {siid, cmd};
	case si::Command:: SICard6Detected:
	case si::Command:: SICard8Detected:
		card_serie = bytes_to_uint(data.mid(5, 1));
		siid = bytes_to_uint(data.mid(6, 3));
		//siid += 1000000 * card_serie;
		return {siid, cmd};
	case si::Command:: SICardRemoved:
		return {0, cmd};
	default:
		throw std::runtime_error("Invalid command received");
	}
}

}

#pragma once

#include <QByteArrayView>

#include <tuple>

namespace si {

static constexpr char STX = 0x02;
static constexpr char ETX = 0x03;

enum Command {
	Invalid=0,
	//SICardDetectedOrRemoved='F', /// next byte shoul be 'I' (detect) or 'O' (removed)
	GetBackupMemory=0x81,
	GetSystemData=0x83,
	SICard5Detected=0xE5,
	SICard6Detected=0xE6,
	SICard8Detected=0xE8,
	SICardRemoved=0xE7,
	GetSICard5=0xB1,
	GetSICard6=0xE1,
	GetSICard8=0xEF,
	//GetPunch2=0x53, /// autosend only (ie. punch)
	SetDirectRemoteMode=0xF0,
	//TimeSend=0x54, /// autosend only (ie. trigger data)
	TransmitRecord=0xD3, /// autosend only (transmit punch or trigger data)

	//DriverInfo=0x1000 /// Driver info (SI commands are only 8 bit long)
};

std::tuple<unsigned int, Command> parseDetectMessageData(QByteArrayView data);

}

#include "EqType.h"

namespace CVG
{
	EQType ConvertToEqType(const std::string& str)
	{
		if (str == "lamp")
			return EQType::Lamp;
		if (str == "camera")
			return EQType::Camera;
		if (str == "sonic")
			return EQType::Sonic;
		if (str == "mic")
			return EQType::Mic;
		if (str == "sensor")
			return EQType::Sensor;
		if (str == "cut")
			return EQType::Cut;
		if (str == "input")
			return EQType::Input;
		if (str == "event")
			return EQType::Event;
		if (str == "actuator")
			return EQType::Actuator;
		if (str == "display")
			return EQType::Display;
		if (str == "spectator")
			return EQType::Spectator;
		if (str == "system")
			return EQType::System;

		return EQType::Null;
	}

	std::string ConvertToString(EQType dt)
	{
		switch (dt)
		{
		case EQType::Lamp:
			return "lamp";
		case EQType::Camera:
			return "camera";
		case EQType::Sonic:
			return "sonic";
		case EQType::Mic:
			return "mic";
		case EQType::Sensor:
			return "sensor";
		case EQType::Cut:
			return "cut";
		case EQType::Input:
			return "input";
		case EQType::Event:
			return "event";
		case EQType::Actuator:
			return "actuator";
		case EQType::Display:
			return "display";
		case EQType::Spectator:
			return "spectator";
		case EQType::System:
			return "system";

		}
		return "null";
	}
}
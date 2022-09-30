
#include "LaserSys.h"
#include <dcmtk/dcmdata/dcdeftag.h>
#include <algorithm>
#include <iostream>
#include "../Utils/cvgAssert.h"

// Something is defining macros for min and max, 
// which we'll need to undo.
#undef min
#undef max

#if IS_RPI

// The input is a value between 0 and 255
// to go from no input, to maximum
void WritePot(short input)
{
	unsigned char transBytes[2] = 
	{
		input >> 8,
		input & 0xFF
	};
	wiringPiSPIDataRW(SPI_CHANNEL, transBytes, 2);
}
#endif

// The current version of C++ we're using doesn't
// have std::clamp().
inline float fclamp(float v, float vmin, float vmax)
{ 
	return std::min(std::max(vmin, v), vmax); 
}

inline float fclamp01(float v)
{
	return fclamp(v, 0.0f, 1.0f);
}

std::string LaserSys::HWName() const
{
	return "LaserSys";
}

void LaserSys::ShowLight(Light l, bool onoff)
{
	switch(l)
	{
	case Light::NIR:
		if(!onoff)
		{	// Turn off
#if IS_RPI 
			std::cout << "Turning NIR OFF." << std::endl;
#else
			std::cout <<"Unhandled request to turn NIR OFF." << std::endl;
#endif
			this->SetNIRIntensity(0.0f);
		}
		else
		{	// Turn on

#if IS_RPI
			std::cout << "Turning NIR ON." << std::endl;
#else
			std::cout <<"Unhandled request to turn NIR ON." << std::endl;
#endif

			std::cout << "Unhandled request to turn NIR to analog power." << std::endl;

			if(this->intensityNIR == 0.0f)
				this->SetNIRIntensity(this->defIntensityNIR);
		}
		break;

	case Light::White:
		if(!onoff)
		{ 
			this->SetWhiteIntensity(0.0f);
			std::cout << "Unhandled message to turn white light off" << std::endl;
		}
		else
		{
			std::cout << "Unhandled message to turn white light to analog power" << std::endl;

			if(this->intensityWhite == 0.0f)
				this->SetWhiteIntensity(this->defIntensityWhite);
		}
		break;
	}
}

void LaserSys::SetLight(Light l, float intensity)
{
	intensity = fclamp01(intensity);


	switch(l)
	{
	case Light::NIR:
		this->intensityNIR = intensity;
#if IS_RPI
		WritePot((short)(intensity * DIGIPOT_NIR_MAX));
#endif
		break;

	case Light::White:
		// TODO: Implement
		this->intensityWhite = intensity;
		break;
	}
}

void LaserSys::SetDefault(Light l, float intensity, DefaultSetMode setMode)
{
	if(l == Light::NIR)
	{
		this->defIntensityNIR = intensity;

		switch(setMode)
		{
		case DefaultSetMode::Set:
			this->SetNIRIntensity(intensity);
			break;

		case DefaultSetMode::OnlyIfOn:
			if(this->intensityNIR != 0.0f)
				this->SetNIRIntensity(intensity);
			break;
		}
	}
	else if(l == Light::White)
	{
		this->defIntensityWhite = intensity;

		switch(setMode)
		{
		case DefaultSetMode::Set:
			this->SetWhiteIntensity(intensity);
			break;

		case DefaultSetMode::OnlyIfOn:
			if(this->intensityWhite != 0.0f)
				this->SetWhiteIntensity(intensity);
			break;
		}
	}
	else
	{
		cvgAssert(false, "Unknown default light mode.");
	}
}

bool LaserSys::Initialize()
{ 

	for(int i = 0; i < (int)Light::Totalnum; ++i)
		this->ShowLight((Light)i, false);

#if IS_RPI
	this->digiPotHandle = wiringPiSPISetup(SPI_CHANNEL, 500000);
#endif

	return true; 
}

bool LaserSys::Validate()
{ 
#if IS_RPI
	if(this->digiPotHandle != -1)
	{
		std::cout << "Laser system validated as connected to SPI." << std::endl;
		return true;
	}
	else
	{
		std::cout << "ERROR: Laser system not connected to SPI." << std::endl;
		return false;
	}
#endif
	return true;
}

bool LaserSys::Shutdown()
{ 
#if IS_RPI
	if(this->digiPotHandle != -1)
	{ 
		int retCode = close(this->digiPotHandle);
		this->digiPotHandle = -1;

		return retCode != -1;
	}
	else
		return true;
#endif
	return true; 
}

DicomInjector* LaserSys::GetInjector()
{
	return this;
}

void LaserSys::InjectIntoDicom(DcmDataset* dicomData)
{
	// TODO: Sample dicom injection
	dicomData->putAndInsertFloat32(
		DCM_ExcitationFrequency, 720.0f);

	DcmItem* deviceInfo;
	if(!dicomData->findOrCreateSequenceItem(DCM_DeviceSequence, deviceInfo, -2).good())
		return;

	float nirPercent = this->intensityNIR * 100.0f;
	std::string strPercent = std::to_string(nirPercent) + "%";
	deviceInfo->putAndInsertString(DCM_OutputPower, strPercent.c_str());

	deviceInfo->putAndInsertString(DCM_CodeMeaning, "Laser System for IR viewing of contrast dye.");
	deviceInfo->putAndInsertString(DCM_DeviceDescription, "Laser System");
	deviceInfo->putAndInsertString(DCM_Manufacturer, "__Manufacturer_TBD__");
	deviceInfo->putAndInsertString(DCM_DeviceID, "__DeviceID_TBD__");
}
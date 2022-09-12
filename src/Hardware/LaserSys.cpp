
#include "LaserSys.h"
#include <dcmtk/dcmdata/dcdeftag.h>
#include <algorithm>
#include <iostream>

#define PIN_NIR_BCM_PIN 25
#define PIN_WHITE_BCM_PIN 6

// Something is defining macros for min and max, 
// which we'll need to undo.
#undef min
#undef max

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
			//
			// Line below doesn't work for unknown reasons, so
			// instead the pin mode is changed.
			//
			// digitalWrite(PIN_NIR_BCM_PIN, 1);
			//
			pinMode(PIN_NIR_BCM_PIN, INPUT);
#else
			std::cout <<"Unhandled request to turn NIR OFF." << std::endl;
#endif

			this->SetNIRIntensity(0.0f);
		}
		else
		{	// Turn on

#if IS_RPI
			std::cout << "Turning NIR ON." << std::endl;
			pinMode(PIN_NIR_BCM_PIN, OUTPUT);
			digitalWrite(PIN_NIR_BCM_PIN, 1);
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
		// TODO: Implement
		this->intensityNIR = intensity;
		break;

	case Light::White:
		// TODO: Implement
		this->intensityWhite = intensity;
		break;
	}
}

bool LaserSys::Initialize()
{ 
	cvg::multiplatform::InitGPIO();

	for(int i = 0; i < (int)Light::Totalnum; ++i)
		this->ShowLight((Light)i, false);

	return true; 
}

bool LaserSys::Validate()
{ return true; }

bool LaserSys::Shutdown()
{ return true; }

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

	deviceInfo->putAndInsertString(DCM_CodeMeaning, "Laser System for IR viewing of contrast dye.");
	deviceInfo->putAndInsertString(DCM_DeviceDescription, "Laser System");
	deviceInfo->putAndInsertString(DCM_Manufacturer, "__Manufacturer_TBD__");
	deviceInfo->putAndInsertString(DCM_DeviceID, "__DeviceID_TBD__");
}
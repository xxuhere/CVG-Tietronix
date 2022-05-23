#include "LaserSys.h"
#include <algorithm>

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
			this->SetNIRIntensity(0.0f);
		else
		{ 
			if(this->intensityNIR == 0.0f)
				this->SetNIRIntensity(this->defIntensityNIR);
		}
		break;

	case Light::White:
		if(!onoff)
			this->SetWhiteIntensity(0.0f);
		else
		{
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
{ return true; }

bool LaserSys::Validate()
{ return true; }

bool LaserSys::Shutdown()
{ return true; }
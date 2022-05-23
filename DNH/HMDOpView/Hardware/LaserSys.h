#pragma once
#include "IHardware.h"

/// <summary>
/// The laser/lamp system.
/// </summary>
class LaserSys : public IHardware
{
public:
	/// <summary>
	/// The types of lights to manipulate in 
	/// ShowLight() and SetLight().
	/// </summary>
	enum class Light
	{
		White,
		NIR
	};

	// For now (until there's more direction), the
	// intensity will be on a scale from 0.0 to 1.0
	const float defIntensityWhite = 0.5f;
	const float defIntensityNIR = 0.5f;

	/// <summary>
	/// The current power of the White light. The value
	// will be between [0.0, 1.0].
	/// </summary>
	float intensityWhite	= 0.0f;

	/// <summary>
	/// The current power of the NIR light. The value
	/// will be between [0.0, 1.0].
	/// </summary>
	float intensityNIR		= 0.0f;

public:

	/// <summary>
	/// Turn a light on and off. This currently just deffers
	/// to SetLight(), 
	/// - with 0.0f for turning a light off
	/// - with defIntensity* for turning a light on.
	/// If a light is already on, it is left unmodified.
	/// </summary>
	/// <param name="l">The light to manipulate.</param>
	/// <param name="onoff">If true, turn light on. Else, off.</param>
	void ShowLight(Light l, bool onoff = true);

	/// <summary>
	/// Set the intensity of a light.
	/// </summary>
	/// <param name="l">The light type to manipulate.</param>
	/// <param name="intensity">A power level between [0.0, 1.0].</param>
	void SetLight(Light l, float intensity);

	inline void SetWhiteIntensity(float intensity)
	{ this->SetLight(Light::White, intensity); }

	inline void SetNIRIntensity(float intensity)
	{ this->SetLight(Light::NIR, intensity); }

	inline void HideLight(Light l)
	{ this->ShowLight(l, false); }

	inline void ShowNIR(bool onoff = true)
	{ return this->ShowLight(Light::NIR, true);}

	inline void HideNIR()
	{ return this->ShowLight(Light::NIR, false);}

	inline void ShowWhite(bool onoff = true)
	{ return this->ShowLight(Light::White, true);}

	inline void ShowWhite()
	{ return this->ShowLight(Light::White, false);}

public:
	std::string HWName() const override;
	bool Initialize() override;
	bool Validate() override;
	bool Shutdown() override;
};
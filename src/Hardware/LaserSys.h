#pragma once
#include "IHardware.h"
#include "../Utils/multiplatform.h"
#include "../DicomUtils/DicomInjector.h"

#if IS_RPI
	#include <wiringPi.h>
	#include <wiringPiSPI.h>

	// The SPI channel to use, assuming the use of a 
	// 4151 digital potentiometer. 
	// 
	// This can either be 0 or 1, and its setting will 
	// dictate which GPIO pins the digital pot hooks up to.
	// 
	// For example, a value of 0 will refer to the CE0 pin.
	const int SPI_CHANNEL = 0;

	// The max value to send to the digipot for the NIR - 
	// used to map the 5vcc output of the RPi GPIO to the
	// [0, 2]v range.
	const int DIGIPOT_NIR_MAX = 101;
#endif

/// <summary>
/// The laser/lamp system.
/// </summary>
class LaserSys : 
	public IHardware,
	public DicomInjector
{
public:

	/// <summary>
	/// Options on what to do with the current value if the 
	/// default is changed.
	/// </summary>
	enum class DefaultSetMode
	{
		/// <summary>
		/// If the default power value is changed, change the current value to match
		/// </summary>
		Set,

		/// <summary>
		/// If the default power value is changed, leave the current value alone.
		/// </summary>
		DontSet,

		/// <summary>
		/// Set the current value, but only if the lamp is currently on.
		/// </summary>
		OnlyIfOn
	};

	/// <summary>
	/// The types of lights to manipulate in 
	/// ShowLight() and SetLight().
	/// </summary>
	enum class Light
	{
		White,
		NIR,
		Totalnum
	};

	// For now (until there's more direction), the
	// intensity will be on a scale from 0.0 to 1.0
	float defIntensityWhite = 0.5f;
	float defIntensityNIR = 0.5f;

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

#if IS_RPI
	int digiPotHandle = -1;
#endif

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

	void SetDefault(Light l, float intensity, DefaultSetMode setMode);

public:
	std::string HWName() const override;
	bool Initialize() override;
	bool Validate() override;
	bool Shutdown() override;

public:
	DicomInjector* GetInjector() override;

	//////////////////////////////////////////////////
	//
	//	DicomInjector FUNCTIONS
	//
	//////////////////////////////////////////////////
	void InjectIntoDicom(DcmDataset* dicomData) override;
};
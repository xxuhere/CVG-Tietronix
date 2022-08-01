#pragma once
#include <string>

class DicomInjector;

/// <summary>
/// Abstraction of various hardware
/// </summary>
class IHardware
{
public:
	/// <summary>
	/// The name of the hardware. This should be a hard-coded
	/// string unique from any other possible hardware.
	/// </summary>
	/// <returns></returns>
	virtual std::string HWName() const = 0;

	/// <summary>
	/// Initialize the core part of the hardware. 
	/// 
	/// If there's any initialization involves other hardware,
	/// defer that to the Validate() phase.
	/// </summary>
	/// <returns>True if success. False if error detected.</returns>
	virtual bool Initialize() = 0;

	/// <summary>
	/// Validate the hardware status. This is called during another
	/// pass right after all hardware has been initialized, allowing
	/// the hardware to validate itself against the system.
	/// </summary>
	/// <returns>True if success. False if error detected.</returns>
	virtual bool Validate() = 0;

	/// <summary>
	/// 
	/// </summary>
	/// <returns>
	/// True if success. 
	/// False if error detected.
	/// There's not much that can be done with a failed shutdown,
	/// the return value is more for clerical than functional
	/// purpose.
	/// </returns>
	virtual bool Shutdown() = 0;

	virtual DicomInjector* GetInjector();

	// Virtual destructor
	virtual ~IHardware();
};
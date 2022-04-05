#pragma once
#include <vector>
#include <ostream>

class IHardware;

/// <summary>
/// Management of hardware in the system that are
/// implemented through IHardware.
/// </summary>
class HardwareMgr
{
private:
	/// <summary>
	/// Track if the system has been initialized. If so,
	/// hardware should not be added since the rest of the
	/// previous hardware was already validated.
	/// </summary>
	bool _initialized = false;

	/// <summary>
	/// The collection of hardware.
	/// </summary>
	std::vector<IHardware*> hws;

public:
	/// <summary>
	/// Remove all the hardware and set the manager as
	/// uninitialized.
	/// </summary>
	/// <param name="del">
	/// If true, delete all hardware removed.
	/// </param>
	void ClearHardware(bool del = true);

	/// <summary>
	/// Get a hardware from its hardware name.
	/// </summary>
	/// <param name="name">The name to find the hardware for.</param>
	/// <returns>
	/// The found hardware, or nullptr if not found.
	/// </returns>
	IHardware* GetFromName(const std::string& name);

	/// <summary>
	/// Add a new hardware to be managed. Things should only
	/// be added while the manager is uninitialized.
	/// </summary>
	/// <param name="hw">The hardware to add.</param>
	/// <returns>
	/// True, if successful. Else, false.
	/// </returns>
	bool Add(IHardware* hw);

	/// <summary>
	/// Remove a hardware from the system.
	/// </summary>
	/// <param name="hw">The hardware to remove.</param>
	/// <param name="del">Delete the removed hardware.</param>
	/// <returns>True if hw was found and removed.</returns>
	bool Remove(IHardware* hw, bool del = true);

	/// <summary>
	/// Initialize the system.
	/// 
	/// It's assumed after the system is initialized, 
	/// </summary>
	/// <param name="errStream">
	/// Output stream to stream errors if any issues were detected.
	/// </param>
	/// <returns>
	/// True if all hardware was successfully initialized.
	/// Else, false.
	/// </returns>
	bool Initialize(std::ostream& errStream);

	/// <summary>
	/// Validate the system.
	/// </summary>
	/// <param name="errStream">
	/// Output stream to stream errors if any issues were detected.
	/// </param>
	/// <returns>
	/// True if all hardware was successfully validated. 
	/// Else, false.
	/// </returns>
	bool Validate(std::ostream& errStream);

	~HardwareMgr();
};
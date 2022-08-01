#pragma once
#include <mutex>
#include <vector>
#include "DicomInjector.h"

/// <summary>
/// A thread-safe collection of DicomInjectors. 
/// 
/// This allows multiple injects to be stored into a single collection
/// and allow them to add add their dicom data at once.
/// 
/// While nothing is stopping multiple DicomInjectorSets
/// from being instanced, the main way its intended to be
/// used is with the singleton (see DicomInjectorSet::GetSingleton()).
/// </summary>
class DicomInjectorSet
{
	/// <summary>
	/// Mutex for initializing the singleton instance. 
	/// 
	/// This is only important for the first time GetSingleton() is called,
	/// and the singleton is initialized.
	/// </summary>
	static std::mutex singletonMutex;

protected:
	/// <summary>
	/// Instance singleton for read/write access of injectors.
	/// </summary>
	std::mutex accessMutex;

	/// <summary>
	/// The injectors in the set.
	/// </summary>
	std::vector<DicomInjectorPtr> injectors;

public:

	/// <summary>
	/// Check if an injector belongs in the set.
	/// 
	/// This varies from AddInjectorRet in that the Ref checks
	/// the injector object.
	/// </summary>
	/// <param name="ptr"></param>
	/// <returns></returns>
	bool Contains(DicomInjector* ptr);

	/// <summary>
	/// Add a shared pointer of a DicomInjector.
	/// </summary>
	/// <param name="ptr">The injector to add.</param>
	/// <returns>True if successful.</returns>
	bool AddInjectorPtr(DicomInjectorPtr ptr);

	/// <summary>
	/// Add a DicomInjector.
	/// 
	/// The specified DicomInject will be automatically
	/// wrapped with a DicomInjectorRef.
	/// </summary>
	/// <param name="ptr">The injector to add.</param>
	/// <returns>True if successful.</returns>
	bool AddInjectorRef(DicomInjector* ptr);

	/// <summary>
	/// Remobe a DicomInjector.
	/// </summary>
	/// <param name="ptr">The injector to remove.</param>
	/// <param name="objRef">If true, identify the injector to remove by its reference.</param>
	/// <returns>True if successful.</returns>
	bool RemoveInjector(DicomInjector* ptr, bool objRef = true);

	/// <summary>
	/// Have all contained injectors insert their data into a dicom dataset.
	/// </summary>
	/// <param name="ds">The dicom dataset to add data into.</param>
	void InjectDataInto(DcmDataset* ds);

	/// <summary>
	/// Clear all injectors in the set.
	/// </summary>
	void Clear();

public:
	/// <summary>
	/// Get a thread-safe singleton DicomInjectorSet.
	/// 
	/// For instances of DicomInjectorSet where only one instance is
	/// shared across all places and threads in an application, use this
	/// singleton instance.
	/// </summary>
	/// <returns>The singleton instance of a DicomInjectorSet.</returns>
	static DicomInjectorSet& GetSingleton();
};


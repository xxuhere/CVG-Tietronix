#pragma once
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <memory>

/// <summary>
/// A base class for an object that can insert data about itself
/// into a dicom dataset.
/// </summary>
class DicomInjector
{
public:
public:
	/// <summary>
	/// Get a pointer to the DicomInject object that has the data.
	/// While it will usually be the DicomInject object iself 
	/// (in which case `return this;` can be used) in same cases,
	/// such as the DicomInjectorRef, the object is something else
	/// that's being referenced.
	/// </summary>
	/// <returns></returns>
	virtual void* GetInjectorObject();

	/// <summary>
	/// Add the object's dicom data into a dataset.
	/// </summary>
	/// <param name="dicomData">The dicom dataset to insert data into.</param>
	virtual void InjectIntoDicom(DcmDataset* dicomData) = 0;

	virtual ~DicomInjector();
};

/// <summary>
/// A reference to a DicomInjector.
/// 
/// For DicomInjectSet, shared pointers are used for references and
/// management of DicomInjector objects; but often other things are
/// responsible for managing their memory, so another level of indirection
/// is required to:
/// - keep using shared pointers
/// - reference the DicomInjector object
/// - not be in charge to the object's memory.
/// </summary>
class DicomInjectorRef : public DicomInjector
{
	/// <summary>
	/// The reference to the actual DicomInject object.
	/// </summary>
	DicomInjector* ref;

public:
	
	DicomInjectorRef(DicomInjector* ref);

	void* GetInjectorObject() override;

	// Delegates to ref->InjectIntoDicom().
	void InjectIntoDicom(DcmDataset* dicomData) override;
};

typedef std::shared_ptr<DicomInjector> DicomInjectorPtr;
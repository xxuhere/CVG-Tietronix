#pragma once
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <memory>

class DicomInjector
{
public:
public:
	virtual void* GetInjectorObject();
	virtual void InjectIntoDicom(DcmDataset* dicomData) = 0;
	virtual ~DicomInjector();
};

class DicomInjectorRef : public DicomInjector
{
	DicomInjector* ref;

public:
	DicomInjectorRef(DicomInjector* ref);

	void* GetInjectorObject() override;
	void InjectIntoDicom(DcmDataset* dicomData) override;
};

typedef std::shared_ptr<DicomInjector> DicomInjectorPtr;
#include "DicomInjector.h"

void* DicomInjector::GetInjectorObject()
{
	return this;
}

DicomInjector::~DicomInjector()
{}

DicomInjectorRef::DicomInjectorRef(DicomInjector* ref)
{
	this->ref = ref;
}

void* DicomInjectorRef::GetInjectorObject()
{
	return this->ref;
}

void DicomInjectorRef::InjectIntoDicom(DcmDataset* dicomData)
{
	this->ref->InjectIntoDicom(dicomData);
}

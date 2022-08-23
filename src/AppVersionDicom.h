#pragma once
#include "DicomUtils/DicomInjector.h"

/// <summary>
/// A Dicom injector that provides information on hardcoded
/// properties of the application.
/// </summary>
class AppVersionDicom : public DicomInjector
{
public:
	void InjectIntoDicom(DcmDataset* dicomData) override;

	static AppVersionDicom& GetInstance();
};
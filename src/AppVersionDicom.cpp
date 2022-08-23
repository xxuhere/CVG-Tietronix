#include "AppVersionDicom.h"
#include <dcmtk/dcmdata/dcdeftag.h>
void AppVersionDicom::InjectIntoDicom(DcmDataset* dicomData)
{
	// For a given build of an application, the stuff InjectIntoDicom
	// manages in here should never change.

	// https://dicom.innolitics.com/ciods/cr-image/general-series/00080060
	dicomData->putAndInsertString(DcmTagKey(DCM_Modality), "XC");

	// https://dicom.innolitics.com/ciods/rt-plan/general-equipment/00080070
	dicomData->putAndInsertString(DcmTagKey(DCM_Manufacturer), "UTSW, WUSTL, Tietronix");

	// https://dicom.innolitics.com/ciods/rt-plan/general-equipment/00081090
	dicomData->putAndInsertString(DcmTagKey(DCM_ManufacturerModelName), "Cancer Vision Goggle Gen4");

	// This is a placeholder for now, but this should probably 
	// eventually pull a value from the machine that uniquely
	// identifies it, such as a CPU/Mobo serial number, or device MAC 
	// address.
	// https://dicom.innolitics.com/ciods/rt-plan/general-equipment/00181000
	dicomData->putAndInsertString(DcmTagKey(DCM_DeviceSerialNumber), "CVG2022Unit#1");

	// https://dicom.innolitics.com/ciods/12-lead-ecg/sop-common/0018a001/00181020
	dicomData->putAndInsertString(DcmTagKey(DCM_SoftwareVersions), "HMDOpView V1.0");
}

AppVersionDicom& AppVersionDicom::GetInstance()
{
	static AppVersionDicom instance;
	return instance;
}
#pragma once
#include <string>
#include "DicomUtils/DVRPersonName.h"
#include "DicomUtils/DicomInjector.h"
#include "nlohmann/json.hpp"
#include "tomlplusplus/toml.hpp"

using json = nlohmann::json;

/// <summary>
/// Operation session data.
/// </summary>
struct OpSession : public DicomInjector
{
public:
	/// <summary>
	/// Return values for loading a sessions file.
	/// See LoadFromFile() for more info.
	/// </summary>
	enum LoadRet
	{
		/// <summary>
		/// File was successfully loaded.
		/// </summary>
		Success,

		/// <summary>
		/// There was an error while parsing the file.
		/// </summary>
		ParseError,

		/// <summary>
		/// There was an error while opening the file.
		/// </summary>
		OpenError
	};

	// https://dicom.innolitics.com/ciods/general-ecg/patient/00100040
	enum Gender
	{
		Male,
		Female,
		Other,

		// Error code on our end, not a real Dicom standard value.
		Unset 
	};

public:
	DVRPersonName patientName;

	// Session name metadata
	std::string sessionName = "__unset_sessionname";

	// Patient gender
	Gender gender = Gender::Unset;

	// https://dicom.innolitics.com/ciods/rt-plan/rt-series/00081072/00080080
	std::string institution = "";

	// https://dicom.innolitics.com/ciods/rt-plan/rt-series/00081072/00080081
	std::string institutionAddr = "";

	// https://dicom.innolitics.com/ciods/cr-image/general-series/00081050
	DVRPersonName physicianName;

	std::string studyDescription = "";

	// https://dicom.innolitics.com/ciods/rt-plan/patient-study/00101020
	float patientSize = 0.0f;

	// https://dicom.innolitics.com/ciods/rt-plan/patient-study/00101030
	float patientWeight = 0.0f;

	// https://dicom.innolitics.com/ciods/rt-plan/patient-study/00101010
	int patientAge = 0;

	void SetSession(const std::string& session);

	/// <summary>
	/// Generate a unique string that can represent the session
	/// as a name valid on file/folder names.
	/// </summary>
	/// <returns>
	/// The valid string, or _invalid_ if there are no valid options.
	/// </returns>
	std::string GenerateSessionPrefix() const;

	LoadRet LoadFromFile(const std::string& filepath, bool throwOnParseErr);

	void Clear();

	void Default();

	bool LoadFromToml(toml::table& inToml);

public:
	static std::string GenerateBlankTOMLTemplate();

public:
	//////////////////////////////////////////////////
	//
	//	DicomInjector FUNCTIONS
	//
	//////////////////////////////////////////////////
	void InjectIntoDicom(DcmDataset* dicomData) override;
};

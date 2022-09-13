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


public:

	// Session name metadata
	std::string sessionName = "__unset_sessionname";

	//////////////////////////////////////////////////
	//
	//	PREOP DATA
	//
	//	https://app.box.com/file/1000185405920
	//	See page 4
	//
	//////////////////////////////////////////////////

	// UPN
	// https://dicom.innolitics.com/ciods/us-image/patient/00100020
	std::string patientid = "";

	// StudyID
	// https://dicom.innolitics.com/ciods/segmentation/general-study/00200010
	std::string studyID = "";
	//
	// Study Description
	// https://dicom.innolitics.com/ciods/segmentation/general-study/00081030
	std::string studyDescription = "";

	// Date of surgery
	// https://dicom.innolitics.com/ciods/segmentation/general-series/00080021	(Series)
	// https://dicom.innolitics.com/ciods/segmentation/general-study/00080020	(Study)
	std::string surgeryDate;

	// Contrast agent, dose, time of administration
	// https://dicom.innolitics.com/ciods/mr-image/contrast-bolus/00180010
	std::string contrastAgent = "";
	//
	// https://dicom.innolitics.com/ciods/mr-image/contrast-bolus/00181041
	float contrastMilliliters = 0.0f;
	//
	// https://dicom.innolitics.com/ciods/mr-image/contrast-bolus/00181042
	// https://dicom.innolitics.com/ciods/mr-image/contrast-bolus/00181043
	std::string contrastInjectionTime;

	// Surgeon, CVG operator
	// NOTE: There may be a better dicom tag for this
	// https://dicom.innolitics.com/ciods/mr-image/patient/00104000
	std::string patientComments = "";

	// https://dicom.innolitics.com/ciods/segmentation/general-series/00080031
	std::string surgeryStartTime;
	

public:
	void SetSession(const std::string& session);

	/// <summary>
	/// Generate a unique string that can represent the session
	/// as a name valid on file/folder names.
	/// </summary>
	/// <returns>
	/// The valid string, or _invalid_ if there are no valid options.
	/// </returns>
	std::string GenerateSessionPrefix() const;

	/// <summary>
	/// Load information from a specified TOML file.
	/// </summary>
	/// <param name="filepath">The filepath to load from.</param>
	/// <param name="throwOnParseErr">
	/// If true, throw an exception on error.
	/// Else, return an error code on error.
	/// </param>
	/// <returns>The return result.</returns>
	LoadRet LoadFromFile(const std::string& filepath, bool throwOnParseErr);

	/// <summary>
	/// Set the object's values to the default constructor's value.
	/// </summary>
	void SetToDefault();

	/// <summary>
	/// Load information from a TOML datastructure.
	/// </summary>
	/// <param name="inToml">The TOML datastructure to load from.</param>
	/// <returns>
	/// True if required values were loaded successfuly. 
	/// False if there was an error parsing the TOML, or if required values were not found.
	/// </returns>
	bool LoadFromToml(toml::table& inToml);

public:
	//////////////////////////////////////////////////
	//
	//	DicomInjector FUNCTIONS
	//
	//////////////////////////////////////////////////
	void InjectIntoDicom(DcmDataset* dicomData) override;
};

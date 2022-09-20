#include "OpSession.h"

#include <iostream>
#include <istream>
#include <fstream>
#include <iomanip>
#include <dcmtk/dcmdata/libi2d/i2d.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include "../DicomUtils/DicomMiscUtils.h"

static const char* szToml_Key_Session			= "session";

// Patient info
static const char* szToml_Header_Patient		= "patient";
static const char* szToml_Key_PatientId			= "upn";
static const char* szToml_Key_PatientCom		= "comments";

// Injection info
static const char* szToml_Header_Contrast		= "contrast";
static const char* szToml_Key_ContrastAgent		= "agent";
static const char* szToml_Key_ContrastVol		= "milliliters";
static const char* szToml_Key_ContrastTime		= "time";

// UNUSED:
// Parts of a name
static const char* szToml_Key_NameLast			= "last";
static const char* szToml_Key_NameMiddle		= "middle";
static const char* szToml_Key_NameFirst			= "first";
static const char* szToml_Key_NamePrefix		= "prefix";
static const char* szToml_Key_NameSuffix		= "suffix";

static const char* szToml_Header_Surgery		= "surgery";
static const char* szToml_Key_SurgeryDate		= "date";
static const char* szToml_Key_SurgeryTime		= "time";
static const char* szToml_Key_SurgeryStudyID	= "study_id";
static const char* szToml_Key_SurgeryDescr		= "study_descr";

void OpSession::SetSession(const std::string& session)
{
	this->sessionName = session;
}

/// <summary>
/// A function to convert arbitrary strings to valid prefixes
/// for file/folder paths.
/// 
/// Currently this function is a simple placeholder.
/// </summary>
/// <param name="str">The string to sanitize.</param>
/// <returns>The sanitized version of the input.</returns>
static std::string _SanitizePrefix(const std::string& str)
{
	return str;
}

/// <summary>
/// Given the known data of the session, generate a valid folder
/// name for where generated artifacts of the operation would
/// be placed.
/// </summary>
/// <returns>The directory path where surgery files would go.</returns>
std::string OpSession::GenerateSessionPrefix() const
{
	if(!this->sessionName.empty())
		return _SanitizePrefix(this->sessionName);

	if(!this->studyID.empty())
		return _SanitizePrefix(this->studyID);

	return "_invalid_";
}

OpSession::LoadRet OpSession::LoadFromFile(const std::string& filepath, bool throwOnParseErr)
{
	std::ifstream ifs(filepath);
	if(!ifs.is_open())
	{ 
		std::cerr << "Could not find session file " << filepath << std::endl;
		return LoadRet::OpenError;
	}

	try
	{ 
		toml::table parsedToml = toml::parse(ifs);
		this->SetToDefault();
		this->LoadFromToml(parsedToml);
	}
	catch(std::exception& ex)
	{
		std::cerr << "Failed to load session from " << filepath << " : " << ex.what() << std::endl;

		if(throwOnParseErr)
			throw ex;

		return LoadRet::ParseError;
	}
	return LoadRet::Success;
}


void OpSession::SetToDefault()
{
	// Use the default constructor and transfer those default
	// member values to ourself.
	*this = OpSession();
}

const toml::v3::table* EnsureSingleTable(const toml::v3::table* parent, const std::string& key)
{
	const toml::v3::node* tomlFind = parent->get(key);
	if(!tomlFind)
		return nullptr;

	if(tomlFind->is_table())
		return tomlFind->as_table();

	return nullptr;
}

template <typename ty>
bool TransferOptional(std::optional<ty>& optVal, ty& dstVal)
{
	if(!optVal.has_value())
		return false;

	dstVal = optVal.value();
	return true;
}

bool LoadTOMLIntoName(const toml::v3::table* tomlName, DVRPersonName& dstname)
{
	bool anySet = false;

	dstname.Clear();

	std::optional<std::string> lastName		= (*tomlName)[szToml_Key_NameLast].value<std::string>();
	std::optional<std::string> middleName	= (*tomlName)[szToml_Key_NameMiddle].value<std::string>();
	std::optional<std::string> firstName	= (*tomlName)[szToml_Key_NameFirst].value<std::string>();
	std::optional<std::string> prefixName	= (*tomlName)[szToml_Key_NamePrefix].value<std::string>();
	std::optional<std::string> suffixName	= (*tomlName)[szToml_Key_NameSuffix].value<std::string>();

	anySet = TransferOptional(lastName,		dstname.lastName	) || anySet;
	anySet = TransferOptional(middleName,	dstname.middleName	) || anySet;
	anySet = TransferOptional(firstName,	dstname.firstName	) || anySet;
	anySet = TransferOptional(prefixName,	dstname.prefix		) || anySet;
	anySet = TransferOptional(suffixName,	dstname.suffix		) || anySet;

	return anySet;
}

bool OpSession::LoadFromToml(toml::table& inToml)
{
	////////////////////////////////////////////////////////////
	//
	//		LOAD SESSION DATA
	//
	////////////////////////////////////////////////////////////
	std::optional<std::string> session = inToml[szToml_Key_Session].value<std::string>();
	if(session)
	{
		if(session.has_value())
			this->sessionName = session.value();
	}
	else
	{
		std::cerr << "Did not detect expected session name in TOML file" << std::endl;
		return false;
	}

	////////////////////////////////////////////////////////////
	//
	//		LOAD PATIENT DATA
	//
	////////////////////////////////////////////////////////////

	// Patient data must exist.
	const toml::v3::table* patient = EnsureSingleTable(&inToml, szToml_Header_Patient);
	if(patient == nullptr)
	{
		std::cerr << "Error, TOML missing patient data" << std::endl;
		return false;
	}

	// PATIENT ID
	std::optional<std::string> id = (*patient)[szToml_Key_PatientId].value<std::string>();
	if(id.has_value())
		this->patientid = id.value();

	// PATIENT COMMENTS
	std::optional<std::string> com = (*patient)[szToml_Key_PatientCom].value<std::string>();
	if(com.has_value())
		this->patientComments = com.value();

	////////////////////////////////////////////////////////////
	//
	//		LOAD SURGERY INFO
	//
	////////////////////////////////////////////////////////////

	const toml::v3::table* surgery = EnsureSingleTable(&inToml, szToml_Header_Surgery);
	if(surgery == nullptr)
	{
		std::cerr << "Error, TOML missing surgery data" << std::endl;
		return false;
	}

	std::optional<std::string> surgeryDate = (*surgery)[szToml_Key_SurgeryDate].value<std::string>();
	if(surgeryDate.has_value())
		this->surgeryDate = surgeryDate.value();

	std::optional<std::string> surgeryTime = (*surgery)[szToml_Key_SurgeryTime].value<std::string>();
	if(surgeryTime.has_value())
		this->surgeryStartTime = surgeryTime.value();


	std::optional<std::string> studyID = (*surgery)[szToml_Key_SurgeryStudyID].value<std::string>();
	if(studyID.has_value())
		this->studyID = studyID.value();

	std::optional<std::string> studyDescr = (*surgery)[szToml_Key_SurgeryDescr].value<std::string>();
	if(studyDescr.has_value())
		this->studyDescription = studyDescr.value();

	////////////////////////////////////////////////////////////
	//
	//		LOAD CONTRAST DYE INFO
	//
	////////////////////////////////////////////////////////////
	const toml::v3::table* contrast = EnsureSingleTable(&inToml, szToml_Header_Contrast);
	if(contrast)
	{
		std::optional<std::string> conAgent = (*contrast)[szToml_Key_ContrastAgent].value<std::string>();
		if(conAgent.has_value())
			this->contrastAgent = conAgent.value();

		std::optional<float> conVol = (*contrast)[szToml_Key_ContrastVol].value<float>();
		if(conVol.has_value())
			this->contrastMilliliters = conVol.value();

		std::optional<std::string> conTime = (*contrast)[szToml_Key_ContrastTime].value<std::string>();
		if(conTime.has_value())
			this->contrastInjectionTime = conTime.value();
	}

	return true;
}

void OpSession::InjectIntoDicom(DcmDataset* dicomData)
{

	//////////////////////////////////////////////////
	// A few macros automate the code a bit and make it 
	// simpler to read.
	//////////////////////////////////////////////////
	//
	//
	#define INSERT_DCM_STRING(_tag, _str, _addIfEmpty) \
		DicomMiscUtils::InsertDicomString(dicomData, _tag, _str, _addIfEmpty);
	//
	#define INSERT_DCM_INT2STR(_tag, _int) \
		DicomMiscUtils::InsertDicomString(dicomData, _tag, std::to_string(_int), true);
	//
	//
	//////////////////////////////////////////////////

	INSERT_DCM_STRING(DCM_PatientID,				this->patientid,			true);
	INSERT_DCM_STRING(DCM_StudyID,					this->studyID,				true);
	INSERT_DCM_STRING(DCM_StudyDescription,			this->studyDescription,		true);
	INSERT_DCM_STRING(DCM_StudyDate,				this->surgeryDate,			true);
	INSERT_DCM_STRING(DCM_SeriesDate,				this->surgeryDate,			true);

	if(!this->contrastAgent.empty())
	{
		INSERT_DCM_STRING(DCM_ContrastBolusAgent,	this->contrastAgent,		true);
		INSERT_DCM_INT2STR(DCM_ContrastBolusVolume,	this->contrastMilliliters);
	}
	if(!this->contrastInjectionTime.empty())
	{
		INSERT_DCM_STRING(DCM_ContrastBolusStartTime,this->contrastInjectionTime, true);
		INSERT_DCM_STRING(DCM_ContrastBolusStopTime, this->contrastInjectionTime, true);
	}

	INSERT_DCM_STRING(DCM_PatientComments,			this->patientComments,		true);
	INSERT_DCM_STRING(DCM_SeriesTime,				this->surgeryStartTime,		true);
}
#include "OpSession.h"

#include <iostream>
#include <istream>
#include <fstream>
#include <iomanip>
#include <dcmtk/dcmdata/libi2d/i2d.h>
#include <dcmtk/dcmdata/dcdeftag.h>

static const char* szToml_Header_Patient	= "patient";
static const char* szToml_Header_Physician	= "physician";
static const char* szToml_Header_Name		= "name";
static const char* szToml_Header_Info		= "info";
static const char* szToml_Header_Session	= "session";
static const char* szToml_Header_Institution= "institution";
static const char* szToml_Header_Contrast	= "contrast";
static const char* szToml_Key_NameLast		= "last";
static const char* szToml_Key_NameMiddle	= "middle";
static const char* szToml_Key_NameFirst		= "first";
static const char* szToml_Key_NamePrefix	= "prefix";
static const char* szToml_Key_NameSuffix	= "suffix";
static const char* szToml_Key_Age			= "age";
static const char* szToml_Key_Height		= "height";
static const char* szToml_Key_Weight		= "weight";
static const char* szToml_Key_Gender		= "gender";
static const char* szToml_Key_SessStudy		= "study";
static const char* szToml_Key_InstName		= "name";
static const char* szToml_Key_InstAddr		= "addr";
static const char* szToml_Key_PatientId		= "id";
static const char* szToml_Key_PatientCom	= "comments";
static const char* szToml_Key_EthnicityGroup= "ethnicity";
static const char* szToml_Key_ContrastAgent	= "agent";
static const char* szToml_Key_ContrastVol	= "milliliters";
static const char* szToml_Key_ContrastTime	= "time";

void OpSession::SetSession(const std::string& session)
{
	this->sessionName = session;
}

static std::string _SanitizePrefix(const std::string& str)
{
	return str;
}

std::string OpSession::GenerateSessionPrefix() const
{
	if(!this->sessionName.empty())
		return _SanitizePrefix(this->sessionName);

	if(!this->patientName.lastName.empty())
	{
		if(!this->patientName.firstName.empty())
		{
			return _SanitizePrefix(this->patientName.lastName + "_" + this->patientName.firstName);
		}
		return _SanitizePrefix(this->patientName.lastName);
	}
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
		this->Clear();
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

void OpSession::Clear()
{
	this->patientName.Clear();
	this->sessionName.clear();
	this->gender = Gender::Unset;
	this->institution.clear();
	this->institutionAddr.clear();
	this->physicianName.Clear();
	this->studyDescription.clear();
	this->patientSize = 0.0f;
	this->patientWeight = 0.0f;
	this->patientAge = 0;
	this->patientComments.clear();
}

void OpSession::Default()
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

	// PATIENT NAME
	//
	// Patient name must exist.
	bool hasName = false;
	const toml::v3::table* patientName = EnsureSingleTable(patient, szToml_Header_Name);
	if(patientName)
		hasName = LoadTOMLIntoName(patientName, this->patientName);

	if(!hasName)
	{
		std::cerr << "Error, TOML missing patient name" << std::endl;
		return false;
	}

	// PATIENT INFO
	const toml::v3::table* patientInfo = EnsureSingleTable(patient, szToml_Header_Info);
	if(patientInfo)
	{
		// PATIENT AGE
		std::optional<int> age = (*patientInfo)[szToml_Key_Age].value<int>();
		if(age.has_value())
			this->patientAge = age.value();

		// PATIENT HEIGHT
		std::optional<float> height = (*patientInfo)[szToml_Key_Height].value<float>();
		if(height.has_value())
			this->patientSize = height.value();

		// PATIENT WEIGHT
		std::optional<float> weight = (*patientInfo)[szToml_Key_Weight].value<float>();
		if(weight.has_value())
			this->patientWeight = weight.value();

		// PATIENT GENDER
		std::optional<std::string> gender = (*patientInfo)[szToml_Key_Gender].value<std::string>();
		if(gender.has_value())
		{
			std::string genderVal = gender.value();
			// Make all lowercase for case insensitivity comparison
			std::transform(genderVal.begin(), genderVal.end(), genderVal.begin(), tolower);
			if(genderVal == "m")
				this->gender = Gender::Male;
			else if(genderVal == "f")
				this->gender = Gender::Female;
			else
				this->gender = Gender::Other;
		}

		// PATIENT ID
		std::optional<std::string> id = (*patientInfo)[szToml_Key_PatientId].value<std::string>();
		if(id.has_value())
			this->patientid = id.value();

		// PATIENT ETHNIC GROUP
		std::optional<std::string> ethnicity = (*patientInfo)[szToml_Key_EthnicityGroup].value<std::string>();
		if(ethnicity.has_value())
			this->ethnicGroup = ethnicity.value();

		// PATIENT COMMENTS
		std::optional<std::string> com = (*patientInfo)[szToml_Key_PatientCom].value<std::string>();
		if(id.has_value())
			this->patientComments = com.value();
	}
	else
		std::cout << "Warning: Missing patient info in session" << std::endl;

	////////////////////////////////////////////////////////////
	//
	//		LOAD PHYSICIAN DATA
	//
	////////////////////////////////////////////////////////////
	const toml::v3::table* physician = EnsureSingleTable(&inToml, szToml_Header_Physician);
	if(physician)
	{
		const toml::v3::table* physicianName = EnsureSingleTable(physician, szToml_Header_Name);
		if(physicianName)
			LoadTOMLIntoName(physicianName, this->physicianName);
	}

	////////////////////////////////////////////////////////////
	//
	//		LOAD SESSION DATA
	//
	////////////////////////////////////////////////////////////
	const toml::v3::table* session = EnsureSingleTable(&inToml, szToml_Header_Session);
	if(session)
	{
		std::optional<std::string> studyDescr = (*session)[szToml_Key_SessStudy].value<std::string>();
		if(studyDescr.has_value())
			this->studyDescription = studyDescr.value();
	}

	////////////////////////////////////////////////////////////
	//
	//		LOAD INSTITUTION DATA
	//
	////////////////////////////////////////////////////////////
	const toml::v3::table* institution = EnsureSingleTable(&inToml, szToml_Header_Institution);
	if(institution)
	{
		std::optional<std::string> instName = (*institution)[szToml_Key_InstName].value<std::string>();
		if(instName.has_value())
			this->institution = instName.value();

		std::optional<std::string> instAddr = (*institution)[szToml_Key_InstAddr].value<std::string>();
		if(instAddr.has_value())
			this->institutionAddr = instAddr.value();
	}

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

std::string OpSession::GenerateBlankTOMLTemplate()
{
	return 
	"[patient]\n"
	"\n"
	"# Set the patient name, first and last should be filled out.\n"
	"# If other parts of the name are not relevant, comment them out.\n"
	"[patient.name]\n"
	"first 	= \"_unset_first_name_\"\n"
	"#middle = \"_unset_middle_name_\"\n"
	"last 	= \"_unset_last_name_\"\n"
	"#prefix = \"_Sr_\"\n"
	"#suffix = \"_Phd_\"\n"
	"\n"
	"[patient.info]\n"
	"age = 50\n"
	"height = 1.6\n"
	"weight = 150\n"
	"gender = \"M\"\n"
	"id = \"__ID__\"\n"
	"ethnicity = \"__ETHNICITY__\"\n"
	"comments = \"\"\n"
	"\n"
	"[physician]\n"
	"[physician.name]\n"
	"first 	= \"P_unset_first_name_\"\n"
	"middle = \"P_unset_middle_name_\"\n"
	"last 	= \"P_unset_last_name_\"\n"
	"prefix = \"P_Sr_\"\n"
	"suffix = \"P_Phd_\"\n"
	"\n"
	"[session]\n"
	"study = \"_empty_\"\n"
	"\n"
	"[institution]\n"
	"name = \"_institution_\"\n"
	"addr = \"_addr_\"\n"
	"[contrast]\n"
	"agent = \"LS301\"\n"
	"milliliters = 20.0\n"
	"time = \"120000\"\n";

}

void OpSession::InjectIntoDicom(DcmDataset* dicomData)
{
	dicomData->putAndInsertString(
		DCM_PatientName, 
		this->patientName.ToDicomForm().c_str());

	dicomData->putAndInsertString(
		DCM_InstitutionName,
		this->institution.c_str());

	if(!this->patientComments.empty())
	{
		dicomData->putAndInsertString(
			DCM_PatientComments,
			this->patientComments.c_str());
	}

	dicomData->putAndInsertString(
		DCM_InstitutionAddress,
		this->institutionAddr.c_str());

	dicomData->putAndInsertString(
		DCM_PerformingPhysicianName, 
		this->physicianName.ToDicomForm().c_str());

	dicomData->putAndInsertString(
		DCM_StudyDescription,
		this->studyDescription.c_str());

	dicomData->putAndInsertFloat32(
		DCM_PatientSize,
		this->patientSize);

	dicomData->putAndInsertFloat32(
		DCM_PatientWeight,
		this->patientWeight);

	dicomData->putAndInsertSint32(
		DCM_PatientAge,
		this->patientAge);

	if(!this->patientid.empty())
	{
		dicomData->putAndInsertString(
			DCM_PatientID,
			this->patientid.c_str());
	}

	if(!this->ethnicGroup.empty())
	{
		dicomData->putAndInsertString(
			DCM_EthnicGroup,
			this->ethnicGroup.c_str());
	}

	switch(this->gender)
	{
	case Gender::Female:
		dicomData->putAndInsertString(DCM_PatientSex, "F");
		break;

	case Gender::Male:
		dicomData->putAndInsertString(DCM_PatientSex, "M");
		break;

	default:
		dicomData->putAndInsertString(DCM_PatientSex, "O");
	}

	if(!this->contrastAgent.empty())
	{
		dicomData->putAndInsertString(
			DCM_ContrastBolusAgent,
			this->contrastAgent.c_str());

		dicomData->putAndInsertString(
			DCM_ContrastBolusVolume,
			std::to_string(this->contrastMilliliters).c_str());
	}

	if(!this->contrastInjectionTime.empty())
	{
		// Hopefully the ContrastBolus times are being interpreted correctly,
		// It's assuming the injection is on the previous day (there's no Dicom
		// tag for the date either way) and if we're referencing a shot, it's
		// short enough to be considered instantaneous - so we're going to
		// set both the start and end with the same value.
		dicomData->putAndInsertString(
			DCM_ContrastBolusStartTime,
			this->contrastInjectionTime.c_str());

		dicomData->putAndInsertString(
			DCM_ContrastBolusStopTime,
			this->contrastInjectionTime.c_str());
	}
}
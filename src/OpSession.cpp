#include "OpSession.h"

#include <iostream>
#include <istream>
#include <fstream>
#include <iomanip>
#include <dcmtk/dcmdata/libi2d/i2d.h>
#include <dcmtk/dcmdata/dcdeftag.h>

static const char* szKey_Firstname			= "firstname";
static const char* szKey_Middlename			= "middlename";
static const char* szKey_Lastname			= "lastname";
static const char* szKey_Sessionname		= "session";

static const char* szToml_Header_Patient	= "patient";
static const char* szToml_Header_Physician	= "physician";
static const char* szToml_Header_Name		= "name";
static const char* szToml_Header_Info		= "info";
static const char* szToml_Header_Session	= "session";
static const char* szToml_Header_Institution= "institution";
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

json OpSession::RepresentationAsJSON() const
{
	json ret = json::object();
	ret[szKey_Firstname		] = this->patientName.firstName;
	ret[szKey_Middlename	] = this->patientName.middleName;
	ret[szKey_Lastname		] = this->patientName.lastName;
	ret[szKey_Sessionname	] = this->sessionName;
	return ret;
}

void OpSession::Apply(json& data)
{
	if(data.contains(szKey_Firstname) && data[szKey_Firstname].is_string())
		this->patientName.firstName = data[szKey_Firstname];

	if(data.contains(szKey_Middlename) && data[szKey_Middlename].is_string())
		this->patientName.middleName = data[szKey_Middlename];

	if(data.contains(szKey_Lastname) && data[szKey_Lastname].is_string())
		this->patientName.lastName = data[szKey_Lastname];

	if(data.contains(szKey_Sessionname) && data[szKey_Sessionname].is_string())
		this->sessionName = data[szKey_Sessionname];
}

bool OpSession::LoadFromFile(const std::string& filepath)
{
	std::ifstream ifs(filepath);
	if(!ifs.is_open())
		return false;

	try
	{ 
		toml::table parsedToml = toml::parse(ifs);
		this->Clear();
		this->LoadFromToml(parsedToml);
		//this->Default();
		//this->Apply(loaded);
	}
	catch(std::exception& ex)
	{
		std::cerr << "Failed to load session from " << filepath << " : " << ex.what() << std::endl;
		return false;
	}
	return true;
}

bool OpSession::SaveToFile(const std::string& filepath) const
{
	std::ofstream ofs(filepath);
	if(!ofs.is_open())
		return false;

	json jsRepr = this->RepresentationAsJSON();

	// Printing a beautified json output with nhloman
	// https://stackoverflow.com/q/47834320/2680066
	//
	// I don't think the std::endl is required for its newline, 
	// but to finalize flushing the buffer.
	ofs << std::setw(4) << jsRepr << std::endl;
	return true;
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
}

void OpSession::Default()
{
	// Use the default constructor and transfer those default
	// member values to ourself.
	*this = OpSession();
}

bool OpSession::SaveIntoToml(toml::table& inToml)
{
	return true;
}

const toml::v3::table* EnsureSingleTable(const toml::v3::table* parent, const std::string& key)
{
	const toml::v3::node* tomlFind = parent->get(key);
	if(!tomlFind)
		return nullptr;

	if(tomlFind->is_table())
		return tomlFind->as_table();

	if(!tomlFind->is_array())
		return nullptr;

	// This function requires there to be EXACTLY one instance, that's where
	// the "Single" comes from in this function's name.
	const toml::v3::array* tomlArray = tomlFind->as_array();
	if(tomlArray->size() != 1)
		return nullptr;

	const toml::v3::node* singleFindEle = tomlArray->get(0);
	if(!singleFindEle)
		return nullptr;

	return singleFindEle->as_table();
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
	
	if(lastName.has_value() && !lastName.value().empty())
	{
		dstname.lastName = lastName.value();
		anySet = true;
	}

	if(middleName.has_value() && !middleName.value().empty())
	{
		dstname.middleName = middleName.value();
		anySet = true;
	}

	if(firstName.has_value() && !firstName.value().empty())
	{
		dstname.firstName = firstName.value();
		anySet = true;
	}

	if(prefixName.has_value() && !prefixName.value().empty())
	{
		dstname.prefix = prefixName.value();
		anySet = true;
	}

	if(suffixName.has_value() && !suffixName.value().empty())
	{
		dstname.suffix = suffixName.value();
		anySet = true;
	}

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

	// PATIENT AGE
	const toml::v3::table* patientInfo = EnsureSingleTable(patient, szToml_Header_Info);
	if(patientInfo)
	{
		std::optional<int> age = (*patientInfo)[szToml_Key_Age].value<int>();
		if(age.has_value())
			this->patientAge = age.value();

		std::optional<float> height = (*patientInfo)[szToml_Key_Height].value<float>();
		if(height.has_value())
			this->patientSize = height.value();

		std::optional<float> weight = (*patientInfo)[szToml_Key_Weight].value<float>();
		if(weight.has_value())
			this->patientWeight = weight.value();

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

	return true;
}

void OpSession::InjectIntoDicom(DcmDataset* dicomData)
{
	dicomData->putAndInsertString(
		DCM_PatientName, 
		this->patientName.ToDicomForm().c_str());

	dicomData->putAndInsertString(
		DCM_InstitutionName,
		this->institution.c_str());

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
}
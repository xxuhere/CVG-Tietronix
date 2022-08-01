#pragma once
#include <string>
/// <summary>
/// Dicom Value Representation for a Person Name
/// 
/// For more information, see PN in
/// https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
/// </summary>
struct DVRPersonName
{
public:
	std::string lastName;	// aka "family name"
	std::string middleName;
	std::string firstName;	// aka "given name"
	std::string prefix;
	std::string suffix;

public:
	DVRPersonName();

	DVRPersonName(const std::string& dicomForm);

	DVRPersonName(
		const std::string& last, 
		const std::string& middle, 
		const std::string& first,
		const std::string& prefix,
		const std::string& suffix);

	void Set(
		const std::string& last, 
		const std::string& middle, 
		const std::string& first,
		const std::string& prefix,
		const std::string& suffix);

	bool LoadFromDicomForm(const std::string& str);
	std::string ToDicomForm() const;
	void Clear();
};
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

	/// <summary>
	/// Given a name string in dicom form (see link to dicom standard), 
	/// </summary>
	/// <param name="dicomForm"></param>
	DVRPersonName(const std::string& dicomForm);

	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="last">The last name.</param>
	/// <param name="middle">The middle name.</param>
	/// <param name="first">The first name.</param>
	/// <param name="prefix">The name prefix (eg, sr, mr, ms, etc) </param>
	/// <param name="suffix">The name suffix (eg, jr, sr, phd, etc)</param>
	DVRPersonName(
		const std::string& last, 
		const std::string& middle, 
		const std::string& first,
		const std::string& prefix,
		const std::string& suffix);

	/// <summary>
	/// Set the values of the name.
	/// 
	/// See parameters for constructor for more information.
	/// </summary>
	void Set(
		const std::string& last, 
		const std::string& middle, 
		const std::string& first,
		const std::string& prefix,
		const std::string& suffix);

	/// <summary>
	/// Set name object from a dicom formatted name.
	/// </summary>
	/// <param name="str">The dicom formatted name.</param>
	/// <returns>True if success.</returns>
	bool LoadFromDicomForm(const std::string& str);

	/// <summary>
	/// Get the name in a form suitable for storage in a dicom file.
	/// </summary>
	/// <returns>The name, formatted for dicom file storage.</returns>
	std::string ToDicomForm() const;

	/// <summary>
	/// Set all name components to empty strings.
	/// </summary>
	void Clear();
};
#pragma once
#include <mutex>
#include <string>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/ofstd/ofcmdln.h>
#include <dcmtk/dcmdata/libi2d/i2dplsc.h>

/// <summary>
/// Unified Dicom session data; shared across the 
/// application, and across threads.
/// </summary>
class DicomStash
{
private:
	DicomStash();
	static std::mutex mut;
public:
	static DicomStash& GetInstance();
	DcmItem item;

	// Set variable by VR type, 
	// https://dicom.nema.org/dicom/2013/output/chtml/part05/sect_6.2.html
	void SetString(const DcmTag& tag, const std::string& val);
	void SetName(const DcmTag& tag, const std::string& first, const std::string& mid, const std::string& last);

	void DumpStashCloneInto(DcmItem* dumpDst);
	void Clear();
};
#include "DicomStash.h"

std::mutex DicomStash::mut;

DicomStash::DicomStash()
{}

DicomStash& DicomStash::GetInstance()
{
	// Not sure if static variable initialization
	// is thread-safe - going on the assumption that
	// it isn't. Hence the static mutex.
	std::lock_guard<std::mutex> guard(mut);

	static DicomStash singletonInst;
	return singletonInst;
}

void DicomStash::SetString(const DcmTag& tag, const std::string& val)
{
	std::lock_guard<std::mutex> guard(mut);
	this->item.putAndInsertOFStringArray(tag, val.c_str());
}

void DicomStash::SetName(const DcmTag& tag, const std::string& first, const std::string& mid, const std::string& last)
{
	// For now, prefix and suffixes are omitted, see 
	// https://dicom.nema.org/medical/dicom/current/output/html/part05.html#sect_6.2.1
	// for the order and use of '^' delimiter.

	std::lock_guard<std::mutex> guard(mut);
	std::string name = last;

	if(!mid.empty())
	{
		name += "^";
		name += mid;
	}
	name += "^";
	name += first;

	this->item.putAndInsertOFStringArray(tag, name.c_str());
}

void DicomStash::DumpStashCloneInto(DcmItem* dumpDst)
{
	std::lock_guard<std::mutex> guard(mut);
	for (int i = 0; i < this->item.getNumberOfValues(); ++i)
	{
		DcmElement* dceIt = this->item.getElement(i);

		// Delete if it already exists in the destination, or else the
		// insert will fail if we try to double-add.
		dumpDst->findAndDeleteElement(dceIt->getTag());
		dumpDst->insert((DcmElement*)dceIt->clone());
	}
}

void DicomStash::Clear()
{
	std::lock_guard<std::mutex> guard(mut);
	this->item.clear();
}
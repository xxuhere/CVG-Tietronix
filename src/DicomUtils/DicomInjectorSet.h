#pragma once
#include <mutex>
#include <vector>
#include "DicomInjector.h"

class DicomInjectorSet
{
	static std::mutex singletonMutex;
protected:
	std::mutex accessMutex;
	std::vector<DicomInjectorPtr> injectors;

public:
	bool Contains(DicomInjector* ptr);

	bool AddInjectorPtr(DicomInjectorPtr ptr);
	bool AddInjectorRef(DicomInjector* ptr);
	bool RemoveInjector(DicomInjector* ptr, bool objRef = true);

	void InjectDataInto(DcmDataset* ds);

	void Clear();

public:
	static DicomInjectorSet& GetSingleton();
};


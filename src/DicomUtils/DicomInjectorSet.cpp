#include "DicomInjectorSet.h"

std::mutex DicomInjectorSet::singletonMutex;

bool DicomInjectorSet::Contains(DicomInjector* ptr)
{
	for(int i = 0; i < this->injectors.size(); ++i)
	{
		if(this->injectors[i]->GetInjectorObject() == ptr)
			return true;
	}
	return false;
}

bool DicomInjectorSet::AddInjectorPtr(DicomInjectorPtr ptr)
{
	std::lock_guard<std::mutex> guard(this->accessMutex);

	if(this->Contains(ptr.get()))
		return false;

	this->injectors.push_back(ptr);
	return true;
}

bool DicomInjectorSet::AddInjectorRef(DicomInjector* ptr)
{
	std::lock_guard<std::mutex> guard(this->accessMutex);

	if(this->Contains(ptr))
		return false;

	this->injectors.push_back(DicomInjectorPtr(new DicomInjectorRef(ptr)));
	return true;
}

bool DicomInjectorSet::RemoveInjector(DicomInjector* ptr, bool objRef)
{
	std::lock_guard<std::mutex> guard(this->accessMutex);

	std::vector<DicomInjectorPtr> injectors;
	for(
		std::vector<DicomInjectorPtr>::iterator it = this->injectors.begin();
		it != this->injectors.end();
		++it)
	{
		if(it->get()->GetInjectorObject() == ptr)
		{
			this->injectors.erase(it);
			return true;
		}
	}
	return false;
}

void DicomInjectorSet::InjectDataInto(DcmDataset* ds)
{
	std::lock_guard<std::mutex> guard(this->accessMutex);
	
	for(int i = 0; i < this->injectors.size(); ++i)
		this->injectors[i]->InjectIntoDicom(ds);
}

void DicomInjectorSet::Clear()
{
	std::lock_guard<std::mutex> guard(this->accessMutex);
	this->injectors.clear();
}

DicomInjectorSet& DicomInjectorSet::GetSingleton()
{
	std::lock_guard<std::mutex> guard(singletonMutex);

	static DicomInjectorSet singleton;
	return singleton;
}
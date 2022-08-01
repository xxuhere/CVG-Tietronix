#include "IHardware.h"

IHardware::~IHardware()
{}

DicomInjector* IHardware::GetInjector()
{
	return nullptr;
}
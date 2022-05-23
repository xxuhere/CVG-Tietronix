#include "DynSize.h"

DynSize::DynSize(){}

DynSize::DynSize(
	const UIVec2& anchMin, 
	const UIVec2& anchMax, 
	const UIVec2& offsMin, 
	const UIVec2& offsMax)
{
	this->SetAnchors(anchMin, anchMax);
	this->SetOffsets(offsMin, offsMax);
}

DynSize& DynSize::Set(
	const UIVec2& anchMin, 
	const UIVec2& anchMax, 
	const UIVec2& offsMin, 
	const UIVec2& offsMax)
{
	this->SetAnchors(anchMin, anchMax);
	this->SetOffsets(offsMin, offsMax);
	return *this;
}

DynSize& DynSize::SetAnchors(const UIVec2& anchMin, const UIVec2& anchMax)
{
	this->anchMin = anchMin;
	this->anchMax = anchMax;
	return *this;
}

DynSize& DynSize::SetOffsets(const UIVec2& offsMin, const UIVec2& offsMax)
{
	this->offsMin = offsMin;
	this->offsMax = offsMax;
	return *this;
}

DynSize& DynSize::SetAnchors(float minPerX, float minPerY, float maxPerX, float maxPerY)
{
	this->anchMin.x = minPerX;
	this->anchMin.y = minPerY;
	this->anchMax.x = maxPerX;
	this->anchMax.y = maxPerY;
	return *this;
}

DynSize& DynSize::SetOffsets(float minPxX, float minPxY, float maxPxX, float maxPxY)
{
	this->offsMin.x = minPxX;
	this->offsMin.y = minPxY;
	this->offsMax.x = maxPxX;
	this->offsMax.y = maxPxY;
	return *this;
}

DynSize& DynSize::ZeroOffsets()
{
	this->offsMin.x = 0.0f;
	this->offsMin.y = 0.0f;
	this->offsMax.x = 0.0f;
	this->offsMax.y = 0.0f;
	return *this;
}

DynSize& DynSize::AnchorsAll()
{
	this->SetAnchors(UIVec2(0.0f, 0.0f), UIVec2(1.0f, 1.0f));
	return *this;
}

DynSize& DynSize::AnchorsTop()
{
	this->SetAnchors(UIVec2(0.0f, 0.0f), UIVec2(1.0f, 0.0f));
	return *this;
}

DynSize& DynSize::AnchorsBot()
{
	this->SetAnchors(UIVec2(0.0f, 1.0f), UIVec2(1.0f, 1.0f));
	return *this;
}

DynSize& DynSize::AnchorsLeft()
{
	this->SetAnchors(UIVec2(0.0f, 0.0f), UIVec2(0.0f, 1.0f));
	return *this;
}

DynSize& DynSize::AnchorsRight()
{
	this->SetAnchors(UIVec2(1.0f, 0.0f), UIVec2(1.0f, 1.0f));
	return *this;
}
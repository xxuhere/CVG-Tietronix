#pragma once
#include "UIGraphic.h"
#include "UIDefines.h"
#include "UIColor4.h"

/// <summary>
/// Pretty much a usable version of UIGraphic. UIGraphic should not be used
/// directly because it's purpose is not to be used in the UI as a plate, but
/// rather store variables and logic common to a family of UIBase subclasses.
/// </summary>
class UIPlate : public UIGraphic
{

public:
	UIPlate(UIBase* parent, int idx, const UIRect& r);
	UIPlate(UIBase* parent, int idx, const UIRect& r, const UIColor4& staticCol);
	bool IsSelectable() override;
};
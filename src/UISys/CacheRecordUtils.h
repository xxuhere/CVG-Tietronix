#pragma once

#include "UIVec2.h"
#include "UIRect.h"
#include "UIColor4.h"

////////////////////////////////////////////////////////////////////////////////
//
//		Utility functions to help with comparing differences.
//
//		These functions are used to check if a current value has 
//		changed since what was last recorded about it. If so, they
///		will return true, AS WELL AS automatically updating the record.
//
////////////////////////////////////////////////////////////////////////////////

template <class ty>
bool CheckCacheTemplate(ty cur, ty& record)
{
	if(cur == record)
		return false;

	record = cur;
	return true;
}

template <class ty>
bool CheckCacheTemplateCont(const ty& cur, ty& record)
{
	if(cur == record)
		return false;

	record = cur;
	return true;
}

// Overloads that don't require the invoking code to be template
// invokes.
//
// This is done to telegraph what types we intended to support, as
// well as giving those types in intellisense listings.

// Arguably we should take in const references for containers values.

inline bool CheckCache(int cur, int& record)
{
	return CheckCacheTemplate<int>(cur, record);
}

inline bool CheckCache(float cur, float& record)
{
	return CheckCacheTemplate<float>(cur, record);
}

inline bool CheckCache(const UIVec2& cur, UIVec2& record)
{
	return CheckCacheTemplateCont<UIVec2>(cur, record);
}

inline bool CheckCache(const UIRect& cur, UIRect& record)
{
	return CheckCacheTemplateCont<UIRect>(cur, record);
}

inline bool CheckCache(const UIColor4& cur, UIColor4& record)
{
	return CheckCacheTemplateCont<UIColor4>(cur, record);
}

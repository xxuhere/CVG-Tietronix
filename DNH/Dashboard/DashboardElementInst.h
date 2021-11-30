#pragma once

#include "DashboardElement.h"

class IDashEle;
class CVGBridge;
class DashboardGridInst;

class DashboardElementInst
{
public:
	DashboardElement* refEle;

	/// <summary>
	/// The UI implementation.
	/// </summary>
	IDashEle* uiImpl = nullptr;

	/// <summary>
	/// 
	/// </summary>
	CVGBridge* bridge;

	bool UpdateUI();

	DashboardGridInst* instOwner;

public:

	DashboardElementInst(
		DashboardGridInst* instOwner, 
		CVGBridge* bridge, 
		DashboardElement * ele);

	inline CVGBridge* Bridge()
	{ return this->bridge; }

	inline CVG::ParamSPtr Param()
	{ return this->refEle->Param(); }

	inline std::string EqGUID() const
	{ return this->refEle->EqGUID(); }

	inline DashboardElement * RefEle()
	{ return this->refEle; }

	bool LayoutUIImpl();

	bool SwitchUIDefault();

	bool SwitchUIImplementation(const std::string& implName);

	void OnRefreshInstance();

	/// <summary>
	/// Destroy all UI content in the element.
	/// </summary>
	/// <returns>True if successful. Else, there was no UI content to destroy.</returns>
	bool DestroyUIImpl();

	/// <summary>
	/// Redirect to DrawPreview without needing to directly
	/// access uiImpl.
	/// </summary>
	void DrawImplPreview(wxPaintDC& dc, const wxPoint& offset);
};
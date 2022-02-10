#pragma once

#include "DashboardElement.h"
#include "DashboardInst.h"

class IDashEle;
class CVGBridge;
class DashboardGridInst;

class DashboardElementInst : public DashboardInst
{
public:
	DashboardElement* refEle;

	/// <summary>
	/// The UI implementation.
	/// </summary>
	IDashEle* uiImpl = nullptr;


	bool UpdateUI();

public:

	DashboardElementInst(
		DashboardGridInst* instOwner, 
		CVGBridge* bridge, 
		DashboardElement* ele);

	inline CVG::ParamSPtr Param()
	{ return this->refEle->Param(); }

	inline std::string EqGUID() const
	{ return this->refEle->EqGUID(); }

	inline DashboardElement * RefEle()
	{ return this->refEle; }

	bool SwitchUIDefault();

	bool SwitchUIImplementation(const std::string& implName);

	bool LayoutUIImpl() override;
	void OnRefreshInstance() override;
	bool DestroyUIImpl() override;
	void DrawImplPreview(wxPaintDC& dc, const wxPoint& offset) override;
};
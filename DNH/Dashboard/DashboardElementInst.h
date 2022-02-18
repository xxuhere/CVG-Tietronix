#pragma once

#include "TileParam.h"
#include "DashboardInst.h"

class DashParamUIImpl;
class CVGBridge;
class DashboardGridInst;

/// <summary>
/// A UI instance of a Param tile.
/// </summary>
class DashboardElementInst : public DashboardInst
{
public:

	/// <summary>
	/// Reference to the typed tile the DashboardInst.
	/// </summary>
	TileParam* refEle;

	/// <summary>
	/// The UI implementation.
	/// </summary>
	DashParamUIImpl* uiImpl = nullptr;

	/// <summary>
	/// Updates the UI implementation.
	/// </summary>
	/// <returns></returns>
	bool UpdateUI();

public:

	DashboardElementInst(
		DashboardGridInst* instOwner, 
		CVGBridge* bridge, 
		TileParam* ele);

	inline CVG::ParamSPtr Param()
	{ return this->refEle->Param(); }

	inline std::string EqGUID() const
	{ return this->refEle->EqGUID(); }

	inline TileParam * RefEle()
	{ return this->refEle; }

	/// <summary>
	/// Switch the UI implementation to the default.
	/// </summary>
	/// <returns>True if the implementation was successfully changed.</returns>
	bool SwitchUIDefault();

	/// <summary>
	/// Switch the UI implementation.
	/// </summary>
	/// <param name="implName">
	/// The name of the implementation.
	/// 
	/// See DashEleConstantNames.h for a list of common implementation names.
	/// </param>
	/// <returns>True if the implementation was successfully changed.</returns>
	bool SwitchUIImplementation(const std::string& implName);

	bool LayoutUIImpl() override;
	void OnRefreshInstance() override;
	bool DestroyUIImpl() override;
	void DrawImplPreview(wxPaintDC& dc, const wxPoint& offset) override;

	void OnConnect() override;
	void OnDisconnect() override;
};
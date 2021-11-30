#pragma once
#include "DashEleConstantNames.h"
#include "../DashboardElementInst.h"

/// <summary>
/// UI Implementation for an item in the dashboard.
/// </summary>
class IDashEle
{
public:
	/// <summary>
	/// The element container that defined the Param to represent and
	/// the bounds in the dashboard.
	/// </summary>
	DashboardElementInst* eleInst;

protected:
	IDashEle(DashboardElementInst* parentElement);

protected:

	inline CVG::ParamSPtr Param()
	{ return this->eleInst->Param();}

	inline CVGBridge* Bridge()
	{ return this->eleInst->Bridge();}

public:

	/// <summary>
	/// Called when the containing element is connected to a 
	/// new dashboard element.
	/// </summary>
	/// <returns>
	/// Reconnect to the parent's parameter. If this is called, there
	/// should be an assumption that the Param being referenced has changed.
	/// </returns>
	virtual bool Reattach() = 0;

	/// <summary>
	/// The name of the IDashEle subclass.
	/// </summary>
	/// <returns>The name of the implementation.</returns>
	virtual std::string ImplName() = 0;

	/// <summary>
	/// Realign the UI contents.
	/// </summary>
	virtual void Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize) = 0;

	/// <summary>
	/// Called when the underlying value has been changed and the UI need to
	/// be updated to reflect the change.
	/// </summary>
	virtual void OnParamValueChanged() = 0;

	/// <summary>
	/// Destroy UI elements in the dashboard.
	/// </summary>
	virtual void DestroyWindow() = 0;

	/// <summary>
	/// A Refresh operation was performed.
	/// </summary>
	/// <param name="differentParam">
	/// If true, the param is different than before the refresh. Else, the Param
	/// has not changed.
	/// </param>
	virtual void OnRefreshInstance();

	/// <summary>
	/// Draw a preview of the UI on the canvas. Used for when the UI cannot be 
	/// shown (because they might be inadvertently interacted with) but the user
	/// should know where they would be placed and how it will look.
	/// </summary>
	/// <param name="dc">The paint context.</param>
	/// <param name="offset">
	/// A drag offset amount. Will account for the scroll offset as well as any
	/// other offsets.
	/// </param>
	virtual void DrawPreview(wxPaintDC& dc, const wxPoint& offset) = 0;

	/// <summary>
	/// Toggle the visibility state of the GUI element.
	/// 
	/// This does not enable/disable the entire dashboard, just the
	/// embedded (wxWidget) UI element.
	/// </summary>
	/// <param name="show">If true, show UI element. Else, false.</param>
	virtual void Toggle(bool show) = 0;

	virtual ~IDashEle();
};
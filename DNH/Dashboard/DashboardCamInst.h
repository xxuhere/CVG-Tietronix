#pragma once

#include "TileCam.h"
#include "DashboardInst.h"
#include "StreamMgr/StreamCon.h"

class IDashCam;
class CVGBridge;
class DashboardGridInst;
class DashboardCamInstUI;

wxDECLARE_EVENT(_MYEVT_THREADIMGCHANGE, wxCommandEvent);

/// <summary>
/// The tile UI instance for camera streams.
/// </summary>
class DashboardCamInst : public DashboardInst
{
protected:
	/// <summary>
	/// The dirty flag for if the camera stream should be
	/// updated. This is handled as a dirty flag to signal
	/// redraw commands via threads - and to make sure redraws
	/// don't happen more than needed.
	/// </summary>
	bool dirty = false;

public:
	/// <summary>
	/// Camera stream whose UI is being implemented.
	/// </summary>
	TileCam* refCam;

	/// <summary>
	/// The wxWidget implementation.
	/// </summary>
	DashboardCamInstUI * uiWindow	= nullptr;
	
	/// <summary>
	/// The parent grid instance of this tile instance.
	/// </summary>
	DashboardGridInst* instOwner;

	/// <summary>
	/// Token to the stream, which also updates the
	/// video feed.
	/// </summary>
	StreamCon::Ptr streamCon;

public:

	DashboardCamInst(
		DashboardGridInst* instOwner,
		CVGBridge* bridge,
		TileCam* cam);

	inline TileCam* Cam()
	{ return this->refCam; }

	// !TODO: Check if used, delete if it's not.
	bool UpdateUI();

	bool LayoutUIImpl() override;
	void OnRefreshInstance() override;
	bool DestroyUIImpl() override;
	void DrawImplPreview(wxPaintDC& dc, const wxPoint& offset) override;

	/// <summary>
	/// Initialize UI and systems.
	/// 
	/// This will also initialize the entire Camera system the first
	/// time it's called.
	/// </summary>
	void Initialize();

	/// <summary>
	/// Shutdown the entire Camera systems. Note this will shutdown the
	/// system used by ALL DashboardCamInst objects and should not be
	/// called until the application is closing.
	/// </summary>
	/// <returns>True if success.</returns>
	static bool ShutdownSystem();

	/// <summary>
	/// Set the URI to connect to.
	/// </summary>
	/// <param name="uri">The new URI.</param>
	void SetURI(const std::string & uri);

	/// <summary>
	/// Connection callback for when the camera updates
	/// the video frame.
	/// </summary>
	void ConOnUpdate();

	/// <summary>
	/// Connection callback for when the camera stream
	/// is disconnected (an anticipated disconnection).
	/// </summary>
	void ConOnDisconnect();

	/// <summary>
	/// Connection callback for when the camera stream
	/// encounters an error (which also implies an 
	/// unanticipated disconnect)
	/// </summary>
	void ConOnError();

	bool IsDirty() {return this->dirty;}

	/// <summary>
	/// Set the redraw dirty flag.
	/// </summary>
	/// <returns>
	/// Returns true if the flag was changed. Returns false
	/// if the flag was already set.</returns>
	bool FlagDirty();

	/// <summary>
	/// Clear the redraw dirty flag.
	/// </summary>
	void ClearDirty();
};
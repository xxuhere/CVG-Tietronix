#pragma once

#include "DashboardCam.h"
#include "DashboardInst.h"
#include "StreamMgr/StreamCon.h"

class IDashCam;
class CVGBridge;
class DashboardGridInst;
class DashboardCamInstUI;

wxDECLARE_EVENT(_MYEVT_THREADIMGCHANGE, wxCommandEvent);

class DashboardCamInst : public DashboardInst
{
protected:
	bool dirty = false;

public:
	DashboardCam* refCam;

	DashboardCamInstUI * uiWindow	= nullptr;
	
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
		DashboardCam* cam);

	inline DashboardCam* Cam()
	{ return this->refCam; }

	bool UpdateUI();
	bool LayoutUIImpl() override;
	void OnRefreshInstance() override;
	bool DestroyUIImpl() override;
	void DrawImplPreview(wxPaintDC& dc, const wxPoint& offset) override;

	void Initialize();

	static bool ShutdownSystem();

	void SetURI(const std::string & uri);

	void ConOnUpdate();
	void ConOnDisconnect();
	void ConOnError();

	bool IsDirty() {return this->dirty;}
	bool FlagDirty();
	void ClearDirty();
};
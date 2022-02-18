#include "DashboardCamInst.h"
#include "DashboardGridInst.h"
#include "StreamMgr/StreamMgr.h"
#include "DashboardCamInstUI.h"

wxDEFINE_EVENT(_MYEVT_THREADIMGCHANGE, wxCommandEvent);

DashboardCamInst::DashboardCamInst(
	DashboardGridInst* instOwner,
	CVGBridge* bridge,
	TileCam* cam)
	: DashboardInst(instOwner, bridge, cam)
{
	this->refCam = cam;

}

bool DashboardCamInst::LayoutUIImpl()
{
	// wxScrolledWindow does something weird where the 
	// position when manipulating children is offset by
	// the scroll amount - so we'll need to compensate.
	wxPoint offset;
	wxScrolledWindow * scr = dynamic_cast<wxScrolledWindow*>(this->GridInst()->GridWindow());
	//if(scr != nullptr)
	//	offset = scr->GetViewStart();

	wxPoint pos = this->refCam->UIPos() - offset;
	wxSize size = this->refCam->UISize();

	this->uiWindow->SetSize(pos.x, pos.y, size.x, size.y);

	return true;
}

void DashboardCamInst::OnRefreshInstance()
{
	std::string guid = this->Cam()->EqGUID();
	CVG::BaseEqSPtr eq = this->Bridge()->CVGB_GetEquipment(guid);

	if(eq != nullptr)
	{
		TileCam* cam = this->Cam();
		std::string curURI = cam->URI();
		std::string newURI = cam->URIFromHostname(eq->Hostname()); // Misnomer, may not be actual URI (yet)

		if(curURI != newURI)
		{ 
			this->Cam()->UpdateURIFromHostname(eq->Hostname());
			this->SetURI(newURI);
			this->streamCon->Reconnect(false);
		}
	}
}

bool DashboardCamInst::DestroyUIImpl()
{
	delete this->uiWindow;
	this->uiWindow = nullptr;

	if(this->streamCon != nullptr)
		this->streamCon->Disconnect();

	return DashboardInst::DestroyUIImpl();
}

void DashboardCamInst::DrawImplPreview(wxPaintDC& dc, const wxPoint& offset)
{
	dc.SetPen(*wxBLUE_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);

	wxRect r = this->uiWindow->GetRect();
	dc.DrawRectangle(r);
}

void DashboardCamInst::Initialize()
{
	if(this->uiWindow != nullptr)
		return;

	wxWindow* canvasWin = this->GridCanvas();
	this->uiWindow = new DashboardCamInstUI(canvasWin, this);
}

bool DashboardCamInst::ShutdownSystem()
{
	return DashboardCamInstUI::ShutdownSystem();
}

void DashboardCamInst::SetURI(const std::string& uri)
{
	std::string useStr = StreamMgr::CanonicalizeURI(uri);
	this->streamCon = StreamMgr::Instance().ConnectToURL(useStr);

	if(this->streamCon.get() == nullptr)
		return;

	// For now, all updating
	this->streamCon->fnOnChange = [this](){ this->ConOnUpdate(); };
	this->streamCon->fnOnDC		= [this](){ this->ConOnDisconnect(); };
	this->streamCon->fnOnErr	= [this](){ this->ConOnError(); };
}

void DashboardCamInst::ConOnUpdate()
{
	if(this->FlagDirty())
	{ 
		// !TODO: This can fail the window is closing, or has closed
		// but the session hasn't fully shut down yet.
		// This should probably be mutex protected.
		wxCommandEvent event(_MYEVT_THREADIMGCHANGE, -1);
		wxPostEvent(this->uiWindow, event);
	}
}

void DashboardCamInst::ConOnDisconnect()
{
	this->OnRefreshInstance();
}

void DashboardCamInst::ConOnError()
{
	this->OnRefreshInstance();
}

bool DashboardCamInst::FlagDirty()
{
	if(this->dirty)
		return false;

	this->dirty = true;
	return true;
}

void DashboardCamInst::ClearDirty()
{
	this->dirty = false;
}
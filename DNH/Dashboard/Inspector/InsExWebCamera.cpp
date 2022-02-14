#include "InsExWebCamera.h"
#include "../GFXs/video_25x23.xpm"

#include "../CVGBridge.h"

BEGIN_EVENT_TABLE(InsExtWebCamera, wxWindow)
	EVT_MOTION		( InsExtWebCamera::OnMotion			)
	EVT_LEFT_DOWN	( InsExtWebCamera::OnLeftButtonDown	)
	EVT_LEFT_UP		( InsExtWebCamera::OnLeftButtonUp	)
END_EVENT_TABLE()

InsExtWebCamera::InsExtWebCamera(wxWindow * parent, const std::string & eqGUID, CVGBridge * bridge, const CamChannel & cc)
	: wxWindow(parent, -1)
{
	this->bridge = bridge;
	this->eqGUID = eqGUID;
	this->camChan = cc;

	this->staticText = new wxStaticText(this, -1, this->camChan.label);

	wxBitmap bmp = wxBitmap(video_25x23);
	this->icon = new wxStaticBitmap(this, -1, bmp);
	
	// Stop the child UI elements from blocking mouse events
	// If we don't, their presence will block the ability to 
	wxWindow* rdisableInputs[] = {this->staticText, this->icon};
	for(wxWindow * rdiswin : rdisableInputs)
	{
		//rdiswin->GetEventHandler()->Connect(wxEVT_RIGHT_DOWN,				(wxObjectEventFunction)&InsExtWebCamera::OnRightMouseDown,		nullptr, this);
		rdiswin->GetEventHandler()->Connect(wxEVT_MOTION,					(wxObjectEventFunction)&InsExtWebCamera::OnMotion,				nullptr, this);
		rdiswin->GetEventHandler()->Connect(wxEVT_LEFT_DOWN,				(wxObjectEventFunction)&InsExtWebCamera::OnLeftButtonDown,		nullptr, this);
		rdiswin->GetEventHandler()->Connect(wxEVT_LEFT_UP,					(wxObjectEventFunction)&InsExtWebCamera::OnLeftButtonUp,		nullptr, this);
		rdiswin->GetEventHandler()->Connect(wxEVT_MOUSE_CAPTURE_CHANGED,	(wxObjectEventFunction)&InsExtWebCamera::OnMouseCaptureChanged,	nullptr, this);
		rdiswin->GetEventHandler()->Connect(wxEVT_MOUSE_CAPTURE_LOST,		(wxObjectEventFunction)&InsExtWebCamera::OnMouseCaptureLost,	nullptr, this);
	}

	wxBoxSizer * sizer = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(sizer);
	sizer->Add(this->icon, 0, wxALIGN_CENTER|wxALL, 5);
	sizer->Add(10, 0, 0); // horizontal padding
	sizer->Add(this->staticText, 1, wxALIGN_CENTER);

	this->origBGColor = this->GetBackgroundColour();
}

void InsExtWebCamera::OnMotion(wxMouseEvent& evt)
{
	if(this->GetCapture() == this)
		this->bridge->Param_OnDragMotion(this->eqGUID, this->camChan);
}

void InsExtWebCamera::OnLeftButtonDown(wxMouseEvent& evt)
{
	this->CaptureMouse();
	this->bridge->Param_OnDragStart(this->eqGUID, this->camChan);
	this->SetDraggedBGColor();
}

void InsExtWebCamera::OnLeftButtonUp(wxMouseEvent& evt)
{
	if(this->GetCapture() == this)
	{ 
		this->bridge->Param_OnDragEnd(this->eqGUID, this->camChan);
		this->ReleaseMouse();
	}

	this->SetOriginalBGColor();
}

void InsExtWebCamera::OnMouseCaptureChanged(wxMouseCaptureChangedEvent& evt)
{
	this->bridge->Param_OnDragCancel();
	this->SetOriginalBGColor();
}

void InsExtWebCamera::OnMouseCaptureLost(wxMouseCaptureLostEvent& evt)
{
	this->bridge->Param_OnDragCancel();
	this->SetOriginalBGColor();
}
#include "InspBarParam.h"
#include "InspUIImplParam.h"
#include "InspUIImplBool.h"
#include "InspUIImplInt.h"
#include "InspUIImplFloat.h"
#include "InspUIImplString.h"
#include "InspUIImplButton.h"
#include "InspUIImplEnum.h"

#include "../PaneInspector.h"

wxBEGIN_EVENT_TABLE(InspBarParam, wxWindow)
	EVT_MENU( MenuID::Toggle, InspBarParam::OnMenuToggle)

	EVT_RIGHT_DOWN				( InspBarParam::OnRightMouseDown )
	EVT_MOTION					( InspBarParam::OnMotion )
	EVT_LEFT_DOWN				( InspBarParam::OnLeftButtonDown )
	EVT_LEFT_UP					( InspBarParam::OnLeftButtonUp )
	EVT_MOUSE_CAPTURE_CHANGED	( InspBarParam::OnMouseCaptureChanged )
	EVT_MOUSE_CAPTURE_LOST		( InspBarParam::OnMouseCaptureLost )

wxEND_EVENT_TABLE()

InspBarParam::InspBarParam(wxWindow * parent, PaneInspector* owner, CVGBridge* bridge, std::string eqGUID, CVG::ParamSPtr param)
	: wxWindow(parent, -1)
{
	this->eqGUID = eqGUID;
	this->param = param;

	this->bridge = bridge;
	this->owner = owner;

	std::string label = param->GetLabel();
	if(label.empty())
		label = param->GetID();

	this->nameText = new wxStaticText(this, -1, label);
	// Stop it from blocking mouse events.
	this->nameText->GetEventHandler()->Connect(wxEVT_RIGHT_DOWN,			(wxObjectEventFunction)&InspBarParam::OnRightMouseDown,			nullptr, this);
	this->nameText->GetEventHandler()->Connect(wxEVT_MOTION,				(wxObjectEventFunction)&InspBarParam::OnMotion,					nullptr, this);
	this->nameText->GetEventHandler()->Connect(wxEVT_LEFT_DOWN,				(wxObjectEventFunction)&InspBarParam::OnLeftButtonDown,			nullptr, this);
	this->nameText->GetEventHandler()->Connect(wxEVT_LEFT_UP,				(wxObjectEventFunction)&InspBarParam::OnLeftButtonUp,			nullptr, this);
	this->nameText->GetEventHandler()->Connect(wxEVT_MOUSE_CAPTURE_CHANGED, (wxObjectEventFunction)&InspBarParam::OnMouseCaptureChanged,	nullptr, this);
	this->nameText->GetEventHandler()->Connect(wxEVT_MOUSE_CAPTURE_LOST,	(wxObjectEventFunction)&InspBarParam::OnMouseCaptureLost,		nullptr, this);

	this->SetDefaultWidget();

	this->origBGColor = this->GetBackgroundColour();
}

void InspBarParam::SetDefaultWidget()
{
	if(this->widgetImpl != nullptr)
	{
		this->widgetImpl->DestroyUI();
		delete this->widgetImpl;
		this->widgetImpl = nullptr;
	}

	switch(this->param->Type())
	{
	case CVG::DataType::Bool:
		this->widgetImpl = new InspUIImplBool(this, this->bridge);
		break;

	case CVG::DataType::Enum:
		this->widgetImpl = new InspUIImplEnum(this, this->bridge);
		break;

	case CVG::DataType::Event:
		this->widgetImpl = new InspUIImplButton(this, this->bridge);
		break;

	case CVG::DataType::Float:
		this->widgetImpl = new InspUIImplFloat(this, this->bridge);
		break;

	case CVG::DataType::Int:
		this->widgetImpl = new InspUIImplInt(this, this->bridge);
		break;

	case CVG::DataType::String:
		this->widgetImpl = new InspUIImplString(this, this->bridge);
		break;

	default:
		wxMessageBox("Unhandled widget type", "Application Error");
	}

	this->RebuildSizer();
}

void InspBarParam::RebuildSizer()
{
	this->sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(this->sizer);
	this->sizer->Add(this->nameText,0, wxGROW|wxLEFT|wxRIGHT|wxTOP|wxBOTTOM, 2);

	if(this->widgetImpl != nullptr)
		this->sizer->Add(this->widgetImpl->GetWindow(),1, wxGROW|wxLEFT|wxRIGHT|wxTOP|wxBOTTOM, 2);
}

void InspBarParam::UpdateWidgetValue()
{
	if(this->widgetImpl != nullptr)
		this->widgetImpl->UpdateDisplay();
}

void InspBarParam::OnRightMouseDown(wxMouseEvent& evt)
{
	evt.Skip();

	wxMenu menu;

	menu.Append(MenuID::Toggle, "Hide");

	this->PopupMenu(&menu);
}

void InspBarParam::OnMenuToggle(wxCommandEvent& evt)
{
	this->visible = !this->visible;
	this->Show(this->visible);
	this->owner->LayoutEquipmentScrollView();
}

void InspBarParam::ClearReferences()
{
	this->param.reset();
}

void InspBarParam::OnMotion(wxMouseEvent& evt)
{
	if(this->GetCapture() == this)
		this->bridge->Param_OnDragMotion(this->eqGUID, this->param);
}

void InspBarParam::OnLeftButtonDown(wxMouseEvent& evt)
{
	this->CaptureMouse();
	this->bridge->Param_OnDragStart(this->eqGUID, this->param);
	this->SetDraggedBGColor();
}

void InspBarParam::OnLeftButtonUp(wxMouseEvent& evt)
{
	if(this->GetCapture() == this)
	{ 
		this->bridge->Param_OnDragEnd(this->eqGUID, this->param);
		this->ReleaseMouse();
	}

	this->SetOriginalBGColor();
}

void InspBarParam::OnMouseCaptureChanged(wxMouseCaptureChangedEvent& evt)
{
	this->bridge->Param_OnDragCancel();
	this->SetOriginalBGColor();
}

void InspBarParam::OnMouseCaptureLost(wxMouseCaptureLostEvent& evt)
{
	this->bridge->Param_OnDragCancel();
	this->SetOriginalBGColor();
}

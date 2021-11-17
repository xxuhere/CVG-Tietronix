#include "InspectorParam.h"
#include "InsWidgetParam.h"
#include "InsWidgetBool.h"
#include "InsWidgetInt.h"
#include "InsWidgetFloat.h"
#include "InsWidgetString.h"
#include "InsWidgetEnum.h"

#include "../PaneInspector.h"

wxBEGIN_EVENT_TABLE(InspectorParam, wxWindow)
	EVT_MENU( MenuID::Toggle, InspectorParam::OnMenuToggle)

	EVT_RIGHT_DOWN	( InspectorParam::OnRightMouseDown)
	EVT_MOTION		( InspectorParam::OnMotion)
	EVT_LEFT_DOWN	( InspectorParam::OnLeftButtonDown)
	EVT_LEFT_UP		( InspectorParam::OnLeftButtonUp)

wxEND_EVENT_TABLE()

InspectorParam::InspectorParam(wxWindow * parent, PaneInspector* owner, CVGBridge* bridge, std::string eqGUID, CVG::ParamSPtr param)
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
	this->nameText->GetEventHandler()->Connect(
		wxEVT_RIGHT_DOWN, 
		(wxObjectEventFunction)&InspectorParam::OnRightMouseDown, 
		nullptr, 
		this);

	this->SetDefaultWidget();
}

void InspectorParam::SetDefaultWidget()
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
		this->widgetImpl = new InsWidgetBool(this, this->bridge);
		break;

	case CVG::DataType::Enum:
		this->widgetImpl = new InsWidgetEnum(this, this->bridge);
		break;

	case CVG::DataType::Float:
		this->widgetImpl = new InsWidgetFloat(this, this->bridge);
		break;

	case CVG::DataType::Int:
		this->widgetImpl = new InsWidgetInt(this, this->bridge);
		break;

	case CVG::DataType::String:
		this->widgetImpl = new InsWidgetString(this, this->bridge);
		break;

	default:
		wxMessageBox("Unhandled widget type", "Application Error");
	}

	this->RebuildSizer();
}

void InspectorParam::RebuildSizer()
{
	this->sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(this->sizer);
	this->sizer->Add(this->nameText,0, wxGROW|wxLEFT|wxRIGHT|wxTOP|wxBOTTOM, 2);

	if(this->widgetImpl != nullptr)
		this->sizer->Add(this->widgetImpl->GetWindow(),1, wxGROW|wxLEFT|wxRIGHT|wxTOP|wxBOTTOM, 2);
}

void InspectorParam::UpdateWidgetValue()
{
	if(this->widgetImpl != nullptr)
		this->widgetImpl->UpdateDisplay();
}

void InspectorParam::OnRightMouseDown(wxMouseEvent& evt)
{
	evt.Skip();

	wxMenu menu;

	menu.Append(MenuID::Toggle, "Hide");

	this->PopupMenu(&menu);
}

void InspectorParam::OnMenuToggle(wxCommandEvent& evt)
{
	this->visible = !this->visible;
	this->Show(this->visible);
	this->owner->LayoutEquipmentScrollView();
}

void InspectorParam::ClearReferences()
{
	this->param.reset();
}

void InspectorParam::OnMotion(wxMouseEvent& evt)
{
	if(this->GetCapture() == this)
		this->bridge->Param_OnDragMotion(this->eqGUID, this->param);
}

void InspectorParam::OnLeftButtonDown(wxMouseEvent& evt)
{
	this->CaptureMouse();
	this->bridge->Param_OnDragStart(this->eqGUID, this->param);
}

void InspectorParam::OnLeftButtonUp(wxMouseEvent& evt)
{
	if(this->GetCapture() == this)
		this->ReleaseMouse();

	this->bridge->Param_OnDragEnd(this->eqGUID, this->param);
}

void InspectorParam::OnMouseCaptureChanged(wxMouseCaptureChangedEvent& evt)
{
	this->bridge->Param_OnDragCancel();
}

void InspectorParam::OnMouseCaptureLost(wxMouseCaptureLostEvent& evt)
{
	this->bridge->Param_OnDragCancel();
}

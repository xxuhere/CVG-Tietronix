#include "DashboardElementInst.h"
#include "DashboardGridInst.h"
#include "DashElements/DashParamUIImpl.h"
#include "PaneDashboard.h"

#include "DashElements/DashParamUIImplButton.h"
#include "DashElements/DashParamUIImplFloat.h"
#include "DashElements/DashParamUIImplFloatSlider.h"
#include "DashElements/DashParamUIImplInt.h"
#include "DashElements/DashParamUIImplPulldown.h"
#include "DashElements/DashParamUIImplSingleString.h"
#include "DashElements/DashParamUIImplCheckbox.h"

DashboardElementInst::DashboardElementInst(
	DashboardGridInst* instOwner, 
	CVGBridge* bridge, 
	TileParam * ele)
	: DashboardInst(instOwner, bridge, ele)
{
	this->refEle = ele;
}

bool DashboardElementInst::UpdateUI()
{
	if(this->uiImpl == nullptr)
		return false;

	this->uiImpl->OnParamValueChanged();
	return true;
}

bool DashboardElementInst::LayoutUIImpl()
{
	if(this->uiImpl == nullptr)
		return false;

	// wxScrolledWindow does something weird where the 
	// position when manipulating children is offset by
	// the scroll amount - so we'll need to compensate.
	wxPoint offset;
	wxScrolledWindow * scr = dynamic_cast<wxScrolledWindow*>(this->GridInst()->GridWindow());
	if(scr != nullptr)
		offset = scr->GetViewStart();

	this->uiImpl->Layout(
		this->refEle->UIPos() - offset,
		this->refEle->UISize(),
		this->GridInst()->GridCellSize());

	return true;
}

bool DashboardElementInst::SwitchUIDefault()
{
	switch(this->refEle->Param()->Type())
	{
	case CVG::DataType::Bool:
		return this->SwitchUIImplementation(DASHELENAME_DEFBOOL);

	case CVG::DataType::Enum:
		return this->SwitchUIImplementation(DASHELENAME_DEFENUM);

	case CVG::DataType::Event:
		return this->SwitchUIImplementation(DASHELENAME_DEFEVENT);

	case CVG::DataType::Float:
		return this->SwitchUIImplementation(DASHELENAME_DEFFLOAT);

	case CVG::DataType::Int:
		return this->SwitchUIImplementation(DASHELENAME_DEFINT);

	case CVG::DataType::String:
		return this->SwitchUIImplementation(DASHELENAME_DEFSTRING);
	}

	return false;
}

bool DashboardElementInst::SwitchUIImplementation(const std::string& implName)
{
	wxWindow * canvasWin = this->GridCanvas();

	if(implName == DASHELENAME_DEFBOOL)
	{
		this->DestroyUIImpl();
		this->uiImpl = new DashParamUIImplCheckbox(canvasWin, this);
	}
	else if(implName == DASHELENAME_DEFEVENT)
	{
		this->DestroyUIImpl();
		this->uiImpl = new DashParamUIImplButton(canvasWin, this);
	}
	else if(implName == DASHELENAME_DEFINT)
	{
		this->DestroyUIImpl();
		this->uiImpl = new DashParamUIImplInt(canvasWin, this);
	}
	else if(implName == DASHELENAME_DEFFLOAT)
	{
		this->DestroyUIImpl();
		this->uiImpl = new DashParamUIImplFloat(canvasWin, this);
	}
	else if (implName == DASHELENAME_DEFENUM)
	{
		this->DestroyUIImpl();
		this->uiImpl = new DashParamUIImplPulldown(canvasWin, this);
	}
	else if(implName == DASHELENAME_DEFSTRING)
	{
		this->DestroyUIImpl();
		this->uiImpl = new DashParamUIImplSingleString(canvasWin, this);
	}
	else
		return false;
	
	this->refEle->SetUIImplName(implName);
	return true;
}

void DashboardElementInst::OnRefreshInstance()
{
	if(this->uiImpl != nullptr)
		this->uiImpl->OnRefreshInstance();
}

bool DashboardElementInst::DestroyUIImpl()
{
	if (this->uiImpl == nullptr)
		return false;

	this->uiImpl->DestroyWindow();
	this->uiImpl = nullptr;
	
	return DashboardInst::DestroyUIImpl();
}

void DashboardElementInst::DrawImplPreview(wxPaintDC& dc, const wxPoint& offset)
{
	if(this->uiImpl == nullptr)
		return;

	this->uiImpl->DrawPreview(dc, offset);
}

void DashboardElementInst::OnConnect()
{}

void DashboardElementInst::OnDisconnect()
{}
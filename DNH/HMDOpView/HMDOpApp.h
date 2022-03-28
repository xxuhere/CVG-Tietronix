#pragma once

#include <wx/wx.h>

class HMDOpApp: public wxApp
{
public:
    bool OnInit() override;
    int OnExit() override;
};

DECLARE_APP(HMDOpApp)
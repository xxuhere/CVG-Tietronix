#pragma once

#include <wx/wx.h>
#include <string>

class HMDOpApp: public wxApp
{
public:
    std::string appOptionsLoc = "AppOptions.json";

public:
    bool OnInit() override;
    int OnExit() override;
};

DECLARE_APP(HMDOpApp)
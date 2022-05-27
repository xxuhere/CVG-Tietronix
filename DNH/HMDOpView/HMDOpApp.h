#pragma once

#include <wx/wx.h>
#include <string>

class HMDOpApp: public wxApp
{
public:
    /// <summary>
    /// The filepath of the options json to load.
    /// 
    /// This can be changed via command line parameters 
    /// (in HMDOpApp::OnInit())
    /// </summary>
    std::string appOptionsLoc = "AppOptions.json";

public:
    bool OnInit() override;
    int OnExit() override;
};

DECLARE_APP(HMDOpApp)
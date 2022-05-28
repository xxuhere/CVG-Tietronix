#include "HMDOpApp.h"
#include "MainWin.h"

#include "CamVideo/CamStreamMgr.h"
#include "FontMgr.h"
#include <iostream>

bool HMDOpApp::OnInit()
{
    // Custom AppOptions.json load location
    wxArrayString cmdArgs = this->argv.GetArguments();
    for (int i = 1; i < cmdArgs.size(); ++i)
    {
        if(cmdArgs[i].starts_with("-"))
            continue;

        if(!cmdArgs[i].Lower().EndsWith(".json"))
            continue;

        this->appOptionsLoc = cmdArgs[i];
        break;
    }

    MainWin *frame = 
        new MainWin( 
            "CVG HMD Operator View", 
            wxPoint(50, 50), 
            wxSize(450, 340) );

    return true;
}

int HMDOpApp::OnExit()
{
    CamStreamMgr::ShutdownMgr();
    FontMgr::ShutdownMgr();

    return this->wxApp::OnExit();
}

wxIMPLEMENT_APP(HMDOpApp);
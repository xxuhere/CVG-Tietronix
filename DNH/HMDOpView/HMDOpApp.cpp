#include "HMDOpApp.h"
#include "MainWin.h"

#include "CamVideo/CamStreamMgr.h"
#include "FontMgr.h"
#include <iostream>
#include "Utils/cvgOptions.h"
#include "OpSession.h"

bool HMDOpApp::OnInit()
{
    bool createOptionsFile = false;
    bool createSessionFile = false;

    // Custom AppOptions.json load location
    wxArrayString cmdArgs = this->argv.GetArguments();
    for (int i = 1; i < cmdArgs.size(); ++i)
    {
        if(cmdArgs[i].Lower() == "--create-options")
        {
            createOptionsFile = true;
            continue;
        }

        if(cmdArgs[i].Lower() == "--create-session")
        {
            createSessionFile = true;
            continue;
        }

        // Any other flags are unknown and ignored.
        if(cmdArgs[i].starts_with("-"))
            continue;


        if(!cmdArgs[i].Lower().EndsWith(".json"))
            continue;

        this->appOptionsLoc = cmdArgs[i];
        break;
    }

    // If a create document param was found, the don't run the UI at all,
    // we just create the requested documents and exit.
    if(createOptionsFile || createSessionFile)
    {
        if(createOptionsFile)
        {
            cvgOptions cvgDefault(2, true);
            cvgDefault.SaveToFile(this->appOptionsLoc);
        }

        if(createSessionFile)
        {
            OpSession os;
            os.SaveToFile("Session.json");
        }

        this->Exit();
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
#include "HMDOpApp.h"
#include "MainWin.h"

#include "CamVideo/CamStreamMgr.h"
#include "FontMgr.h"
#include <iostream>
#include <fstream>
#include "Utils/cvgOptions.h"
#include "OpSession.h"
#include "Session_Toml.h"

bool HMDOpApp::OnInit()
{
    bool createOptionsFile = false;
    bool createSessionFile = false;
    bool showHelp = false;

    // Custom AppOptions.json load location
    wxArrayString cmdArgs = this->argv.GetArguments();
    for (int i = 1; i < cmdArgs.size(); ++i)
    {
        if(cmdArgs[i] == "--create-options")
        {
            createOptionsFile = true;
            continue;
        }

        if(cmdArgs[i] == "--create-session")
        {
            createSessionFile = true;
            continue;
        }

        if(cmdArgs[i] == "--help")
        {
            showHelp = true;
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

    if(showHelp == true)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "    hmdopapp --help" << std::endl;
        std::cout << "        Show CLI help." << std::endl;
        std::cout << "    hmdopapp --create-options [optsfile]" << std::endl;
        std::cout << "        Create a new default AppOptions JSON file." << std::endl;
        std::cout << "    hmdopapp --create-session [sessfile]" << std::endl;
        std::cout << "        Create a new default TOML file." << std::endl;
        std::cout << "    hmdopapp [optsfile]" << std::endl;
        std::cout << "        Open the GUI with a specific AppOptions file." << std::endl;
        std::cout << std::endl << std::endl;
        std::cout << "Params:" << std::endl;
        std::cout << "    optsfile" << std::endl;
        std::cout << "        The AppOptions json file. Defaulted to AppOptions.json." << std::endl;
        std::cout << "    sessfile" << std::endl;
        std::cout << "        The sessions toml file. Defaulted to Session.toml." << std::endl;

        exit(1);
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
            std::ofstream outSessionToml(wxGetApp().sessionLoc);
            outSessionToml << GetSessionTOMLString();
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
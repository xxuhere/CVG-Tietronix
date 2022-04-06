#include "HMDOpApp.h"
#include "MainWin.h"

#include "CamVideo/CamStreamMgr.h"
#include "FontMgr.h"
#include <iostream>

bool HMDOpApp::OnInit()
{
    MainWin *frame = new MainWin( "CVG HMD Operator View", wxPoint(50, 50), wxSize(450, 340) );
    return true;
}

int HMDOpApp::OnExit()
{
    CamStreamMgr::ShutdownMgr();
    FontMgr::ShutdownMgr();

    return this->wxApp::OnExit();
}

wxIMPLEMENT_APP(HMDOpApp);
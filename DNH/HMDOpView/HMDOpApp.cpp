#include "HMDOpApp.h"
#include "MainWin.h"

#include "CamStreamMgr.h"
#include "FontMgr.h"
#include <iostream>

bool HMDOpApp::OnInit()
{
    std::cout << "hello!!!!!!!!!!!!!!!!!!!!!" << std::endl;

    MainWin *frame = new MainWin( "CVG HMD Operator View", wxPoint(50, 50), wxSize(450, 340) );

    std::cout << "hello!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    return true;
}

int HMDOpApp::OnExit()
{
    CamStreamMgr::ShutdownMgr();
    FontMgr::ShutdownMgr();

    return this->wxApp::OnExit();
}

wxIMPLEMENT_APP(HMDOpApp);
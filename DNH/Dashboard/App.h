#pragma once
#include <wx/wx.h>

/// <summary>
/// Main application.
/// 
/// Not much happens here except spawning the RootWindow class.
/// </summary>
class App : public wxApp
{
public:
    virtual bool OnInit();
};

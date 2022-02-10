#include "RootWindow.h"
#include "App.h"

wxIMPLEMENT_APP(App);
bool App::OnInit()
{
    RootWindow* frame = new RootWindow("CVG Dashboard", wxPoint(50, 50), wxSize(450, 340));
    frame->Show(true);

    return true;
}
#include "DashboardCamInstUI.h"
#include "DashboardCamInst.h"
#include "StreamMgr/StreamSession.h"

#include "glext.h" // For the OpenGL API define GL_GRB

wxGLContext * DashboardCamInstUI::sharedContext = nullptr;

BEGIN_EVENT_TABLE(DashboardCamInstUI, wxWindow)
	EVT_COMMAND(-1, _MYEVT_THREADIMGCHANGE, DashboardCamInstUI::OnConnectionDirty )
	EVT_MENU( (int)Cmds::Menu_StopStream,		DashboardCamInstUI::OnMenu_StopStream )
	EVT_MENU( (int)Cmds::Menu_ReconnectStream,	DashboardCamInstUI::OnMenu_RestartStream )
END_EVENT_TABLE()

DashboardCamInstUI::DashboardCamInstUI(wxWindow* parent, DashboardCamInst* instOwner)
	: wxWindow(parent, -1)
{
	this->instOwner = instOwner;
	this->displayWin = new wxGLCanvas(this, wxID_ANY);

	if(sharedContext == nullptr)
	{
		static wxGLContextAttrs attrs;
		attrs.CoreProfile().Robust().EndList();
		sharedContext = new wxGLContext(this->displayWin, nullptr, &attrs);
		sharedContext->SetCurrent(*this->displayWin);
	}

	wxBoxSizer * sz = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(sz);
	sz->Add(this->displayWin, 1, wxGROW);
	this->displayWin->SetSize(0, 0, 300, 300);

	this->displayWin->Bind(
		wxEVT_RIGHT_DOWN, 
		&DashboardCamInstUI::OnRightDown, 
		this);

	this->Redraw();
}

void DashboardCamInstUI::Redraw()
{
	// This whole series of dereferencing is a sign that
	// there needs to be some kind of refactoring.

	this->displayWin->SetCurrent(*sharedContext);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	int matW = -1;
	int matH = -1;
	GLuint texHandle = (GLuint)-1;
	{
		if(this->instOwner->streamCon == nullptr)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			return;
		}

		StreamSession * pss = this->instOwner->streamCon->parentSession;
		std::lock_guard<std::mutex> guard(pss->imageDataMut);

		if(
			pss->imageData.get() == nullptr || 
			pss->imageData->cols == 0 ||	// sanity check
			pss->imageData->rows == 0)		// sanity check
		{
			glClear(GL_COLOR_BUFFER_BIT);
			return;
		}

		// Copy the values so we can keep the data around, but also
		// allow pss to go out of scope (and unlock the mutex) ASAP.
		matW = pss->imageData->cols;
		matH = pss->imageData->rows;

		if(pss->textureDirty)
		{
			if(pss->glTextureID == (GLuint)-1)
			{ 
				pss->textureDirty = false;
				glGenTextures(1, &pss->glTextureID);
				glBindTexture(GL_TEXTURE_2D, pss->glTextureID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			}
			if(!pss->imageData->empty())
			{
				glBindTexture(GL_TEXTURE_2D, pss->glTextureID);
				glTexImage2D(
					GL_TEXTURE_2D, 
					0, 
					GL_RGB, 
					pss->imageData->cols, 
					pss->imageData->rows, 
					0, 
					GL_BGR, 
					GL_UNSIGNED_BYTE,
					pss->imageData->ptr());
			}
		}

		// Make a copy and get rid of the mutex and the pointer
		// to the StreamSession.
		texHandle = pss->glTextureID;
	}

	if(texHandle == (GLuint)-1)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		return;
	}

	wxSize dispClient = this->displayWin->GetClientSize();
	if(dispClient.x == 0 || dispClient.y == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		return;
	}
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texHandle);

	glViewport(0, 0, dispClient.x, dispClient.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (double)dispClient.x, (double)dispClient.y, 0.0, -1.0, 1.0);

	// Find the correct size to take up as much of the GL screen, 
	// but still have the correct aspect ratio.
	//
	// This happens in two steps
	//	- Scale up the image while matching the viewport dimension.
	//		- while maintaining the aspect ratio.
	//	- Take the enlarged version, and scale it down while matching
	//	the viewport dimension.
	//		- while maintaining the aspect ratio. 
	//
	// It may seem counter-intuitive to scale it up, just to scale it down,
	// but this takes into account both the X and Y axis and handles all
	// edge cases of the fitting process.
	float width = matW;
	float height = matH;
	//
	// Scale up
	if(width < dispClient.x)
	{
		float scaleUp = dispClient.x / width;
		width *= scaleUp;
		height *= scaleUp;
	}
	if(height < dispClient.y)
	{
		float scaleUp = dispClient.y / height;
		width *= scaleUp;
		height *= scaleUp;
	}
	// Fit inside (scale down)
	if(width > dispClient.x)
	{
		float scaleDown = dispClient.x / width;
		width *= scaleDown;
		height *= scaleDown;
	}
	if(height > dispClient.y)
	{
		float scaleDown = dispClient.y / height;
		width *= scaleDown;
		height *= scaleDown;
	}

	// Calculate the top-left offset to where the image will 
	// be centered.
	float offsX = (dispClient.x - width) * 0.5f;
	float offsY = (dispClient.y - height) * 0.5f;

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.0f, 1.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, texHandle);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 			1.0f);
		glVertex2f(offsX,			offsY + height);
		//
		glTexCoord2f(0.0f,			0.0f);
		glVertex2f(offsX,			offsY);
		//
		glTexCoord2f(1.0f,			0.0f);	
		glVertex2f(offsX + width,	offsY);
		//
		glTexCoord2f(1.0f,			1.0f);
		glVertex2f(offsX + width,	offsY + height);
	glEnd();

	this->displayWin->SwapBuffers();
}

bool DashboardCamInstUI::ShutdownSystem()
{
	if(sharedContext != nullptr)
	{ 
		delete sharedContext;
		sharedContext = nullptr;
		return true;
	}
	return false;
}

void DashboardCamInstUI::OnConnectionDirty(wxCommandEvent& evt)
{
	// Allow another redraw to be queued as soon as we start
	// drawing this one.
	this->instOwner->ClearDirty();
	this->Redraw();
}

void DashboardCamInstUI::OnRightDown(wxMouseEvent& evt)
{
	if(!this->instOwner->Bridge()->IsDNHConnected())
	{
		// If not connected to DNH, don't give the user
		// any controls.
		wxMenu rcCaveatMenu;
		wxMenuItem * itm = rcCaveatMenu.Append(-1, "Not connected to DNH.");
		itm->Enable(false);
		this->PopupMenu(&rcCaveatMenu);
		return;
	}
	

	// Delegated from this->displayWin
	wxMenu rcMenu;
	rcMenu.Append( (int)Cmds::Menu_StopStream,		"Stop Stream" );
	rcMenu.Append( (int)Cmds::Menu_ReconnectStream, "Reconnect Stream" );

	this->PopupMenu(&rcMenu);
}

void DashboardCamInstUI::OnMenu_StopStream(wxCommandEvent& evt)
{
	this->instOwner->streamCon->Halt();
}

void DashboardCamInstUI::OnMenu_RestartStream(wxCommandEvent& evt)
{
	this->instOwner->streamCon->Reconnect(true);
}
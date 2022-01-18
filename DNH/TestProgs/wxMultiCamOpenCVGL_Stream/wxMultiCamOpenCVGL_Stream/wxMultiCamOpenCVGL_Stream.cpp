// This demo is a wxWidget sample to show and test some 
// elements involving multiple camera views in wxWidgets.
//
// It uses OpenGL as the drawing canvas to transfer the 
// pixel data from the OpenCV matrix to a wxWindow. 
//
// Camera streams are handled on seperate threads, but 
// the OpenCV images need to be converted to OpenGL images 
// and drawn to the UI on the main thread.
//
// To run this, make sure two web cameras are plugged in,
// DIRECTLY into the machine (i.e., not through USB hubs).
//
// ☐ Uses wxWidgets
// ☐ Uses OpenGL
// ☐ Multiple (2) OpenCV camera streams
// ☐ Decent framerate
// ☐ Draws to a wxWindow
// ☐ Uses standard threads
// ☐ Compiled and runs on both Windows and RaspberryPi Linux

#ifndef WIN32
	// XInitThreads() required for Linux's GTK
	// https://stackoverflow.com/a/18690540/2680066
	#include <X11/Xlib.h>
#endif

#include <thread>
#include <memory>

#include <wx/wx.h>
#include <wx/glcanvas.h>

#include <GL/gl.h> 
#include "glext.h" // For the OpenGL API define GL_GRB

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// The custom mesage we define to tell the UI to refresh
// the OpenGL display with the cached OpenCV image.
wxDECLARE_EVENT(_MYEVT_THREADIMGCHANGE, wxCommandEvent);
wxDEFINE_EVENT(_MYEVT_THREADIMGCHANGE, wxCommandEvent);

/// <summary>
/// The wxWidgets application
/// </summary>
class App : public wxApp
{
public:
    virtual bool OnInit();
};

/// <summary>
/// The toplevel wxWidgets window of the app
/// </summary>
class OpenCVTest : public wxFrame
{
public:
	OpenCVTest();

	/// <summary>
	/// Data for each of the OpenCV camera and output display. 
	/// </summary>
	struct Output
	{
		/// <summary>
		/// The OpenCV stream
		/// </summary>
		cv::VideoCapture cap;

		/// <summary>
		/// The cached OpenCV image of the last camera frame.
		/// </summary>
		std::shared_ptr<cv::Mat> matRef;

		/// <summary>
		/// The mutex for thread safe access to matRef.
		/// </summary>
		std::mutex matMutex;

		/// <summary>
		/// The handle of the texture where 
		/// </summary>
		GLuint texID = -1;

		/// <summary>
		/// The OpenGL enabled window
		/// </summary>
		wxGLCanvas * glCanvas;

		/// <summary>
		/// The drawing context for the wxGLCanvas
		/// </summary>
		wxGLContext * glContext;

		/// <summary>
		/// Set to true when the running thread should end.
		/// </summary>
		bool threadEnd = false;

		bool dirty = false;

		/// <summary>
		/// The thread running the polling loop to pull the
		/// camera stream frames.
		/// </summary>
		std::thread threadUpdate;
	};

	/// <summary>
	/// The two cameras.
	/// </summary>
	Output cameras[2];

	void OnClose(wxCloseEvent& evt);

	/// <summary>
	/// A message on the main UI thread that's invoked from a signal
	/// from ThreadFn_UpdateCap1() to update the OpenGL view with the
	/// latest cached camera frame.
	/// 
	/// Use the evt param's command ID to figure out which Output to
	/// use.
	/// </summary>
	/// <param name="evt"></param>
	void OnUpdateMat(wxCommandEvent& evt);

	/// <summary>
	/// The running thread function to initialize a camera stream
	/// and poll its frames.
	/// </summary>
	/// <param name="idx">The OpenCV camera ID.</param>
	void ThreadFn_UpdateCap1(int idx);

	DECLARE_EVENT_TABLE();
};

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
#ifndef WIN32
	XInitThreads();
#endif

    OpenCVTest * oct = new OpenCVTest();
    oct->Show(true);

    return true;
}

BEGIN_EVENT_TABLE(OpenCVTest, wxFrame)
	EVT_COMMAND(0, _MYEVT_THREADIMGCHANGE, OpenCVTest::OnUpdateMat)
	EVT_COMMAND(1, _MYEVT_THREADIMGCHANGE, OpenCVTest::OnUpdateMat)
	EVT_CLOSE(OpenCVTest::OnClose)
END_EVENT_TABLE()

OpenCVTest::OpenCVTest()
	: wxFrame(nullptr, wxID_ANY, "OpenCV Test")
{
	// Initialize the OpenGL canvas widgets.
	for(int i = 0; i < 2; ++i)
	{
		Output & outp = cameras[i];

		outp.glCanvas = new wxGLCanvas(this, wxID_ANY);
		outp.glCanvas->SetSize(wxSize(500, 200));
		outp.glCanvas->SetPosition(wxPoint(i * 510, 0)); // 10 pixel padding

		wxGLContextAttrs attrs;
		attrs.CoreProfile().Robust().ResetIsolation().EndList();
		outp.glContext = new wxGLContext(outp.glCanvas, nullptr, &attrs);
	}


	this->Show();

	// Initialize OpenCV camera threads.
	for(int i = 0; i < 2; ++i)
	{ 
		cameras[i].threadUpdate = 
			std::thread([this, i](){ this->ThreadFn_UpdateCap1(i); });
	}

}

void OpenCVTest::OnClose(wxCloseEvent& evt)
{
	// Signal for threads to end.
	for(int i = 0; i < 2; ++i)
		this->cameras[i].threadEnd = true;

	// Wait for the threads to actually finish.
	for(int i = 0; i < 2; ++i)
		this->cameras[i].threadUpdate.join();

	// Clean up camera stream resources
	for(int i = 0; i < 2; ++i)
	{ 
		this->cameras[i].cap.release();
		delete this->cameras[i].glContext;
		this->cameras[i].glContext = nullptr;
	}

	this->Destroy();
}

void OpenCVTest::OnUpdateMat(wxCommandEvent& evt)
{
	// The ID store which index in this->cameras we'll use.
	int idx = evt.GetId();
	Output & outp = this->cameras[idx];

	// If we still have updates after we cleanup for 
	// closing the window.
	if(outp.glContext == nullptr)
		return;

	outp.matMutex.lock();
		// Store the variable to clear out the old reference so we can keep
		// using out local version - but also so we don't need to keep the
		// mutex locked longer than we need to.
		std::shared_ptr<cv::Mat> mat = outp.matRef;
		outp.dirty = false;
	outp.matMutex.unlock();

	outp.glCanvas->SetCurrent(*outp.glContext);


	if(outp.texID == -1)
	{
		// If we haven't allocated the texture handle yet,
		// generate one.
		glGenTextures(1, &outp.texID); 
		glBindTexture(GL_TEXTURE_2D, outp.texID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	else
	{
		// Else, we just need to make sure it's binded.
		glBindTexture(GL_TEXTURE_2D, outp.texID);
	}

	// Transfer pixel data from the OpenCV mat to the (binded) OpenGL texture.
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGB, 
		mat->cols, 
		mat->rows, 
		0, 
		GL_BGR, 
		GL_UNSIGNED_BYTE,
		mat->ptr());

	// Arbitrary clear color just to make sure its obviously 
	// clearing correctly.
	glClearColor(0.5f, 1.0f, 0.75f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// We're doing a little cheat here and using the hard
	// coded dimensions of the glCanvas.
	glViewport(0, 0, 500, 200);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 500.0, 200.0, 0.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, outp.texID);

	// Set the modulating color of the texture.
	glColor3f(1.0, 1.0, 1.0);

	// Draw the quad with the texture to draw the camera feed.
	// Right now we're just going to draw a simple square instead
	// of the correct aspect ratio because we're only concern with
	// previewing the image and being assured it can draw to the
	// UI.
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	//
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(200.0f, 0.0f, 0.0f);
	//
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(200.0f, 200.0f, 0.0f);
	//
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 200.0f, 0.0f);
	glEnd();

	// The OpenGL canvas is double buffered, so swap it to show the
	// changes we just drew.
	outp.glCanvas->SwapBuffers();
}

void OpenCVTest::ThreadFn_UpdateCap1(int idx)
{
	const int apiID = cv::CAP_FFMPEG;

	Output & outp = cameras[idx];

	outp.cap.set(cv::CAP_PROP_FPS, 30);
	outp.cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	outp.cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
	outp.cap.set(cv::CAP_PROP_BUFFERSIZE, 3);
	// open selected camera using selected API

	if(idx == 0)
		outp.cap.open("rtsp://192.168.1.112:8555/unicast", apiID);
	else
		outp.cap.open("rtsp://192.168.1.112:8556/unicast", apiID);

	// check if we succeeded
	if(!outp.cap.isOpened())
	{ 
		wxMessageBox("Error 0", "Error 0", wxOK);
		this->Destroy();
		return;
	}

	while(outp.threadEnd == false)
	{
		// Sleep the minimum amount to allow for context 
		// switching, but not for timing, that will be the
		// job of the server to pipe data as fast as it can.
		const int msSleep = 1;
//#ifdef WIN32
//		Sleep(msSleep);
//#else
//		sleep(msSleep);
//#endif

		// Read camera stream frame
		std::shared_ptr<cv::Mat> mat = std::shared_ptr<cv::Mat>(new cv::Mat());
		if(outp.cap.read(*mat.get()) == false)
			continue;

		bool signalRefresh = false;
		// Get authority to write directly to matRef
		outp.matMutex.lock();
			outp.matRef = mat;

			if(outp.dirty == true)
				signalRefresh = false;
			else
			{ 
				outp.dirty = true;
				signalRefresh = true;
			}
		outp.matMutex.unlock();

		//cv::imshow(std::to_string(idx), *mat);

		// Send a message to the app to update the 
		// display with the matrix we just cached.
		//
		// wxPostEvent is thread safe.
		if(signalRefresh)
		{ 
			wxCommandEvent event(_MYEVT_THREADIMGCHANGE, idx);
			wxPostEvent(this, event);
		}
	}
}
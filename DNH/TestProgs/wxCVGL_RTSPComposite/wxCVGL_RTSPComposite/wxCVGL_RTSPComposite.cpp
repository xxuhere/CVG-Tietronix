
#ifndef WIN32
// XInitThreads() required for Linux's GTK
// https://stackoverflow.com/a/18690540/2680066
#include <X11/Xlib.h>
#endif

#include <thread>
#include <memory>

//#include <GL/glew.h>

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

bool keepRunningThreads = true;

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
	/// Data for each of the OpenCV camera feeds.
	/// </summary>
	struct CamFeed
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
		/// The thread running the polling loop to pull the
		/// camera stream frames.
		/// </summary>
		std::thread threadUpdate;

		/// <summary>
		/// Is the OpenGL texture version up to date with the latest 
		/// video stream frame?
		/// </summary>
		bool dirty = true;

		/// <summary>
		/// Cached and thread-atomic query of if the video stream is open.
		/// </summary>
		bool open = false;
	};

	// Check if any of the camera feeds were marked as dirty before
	bool updateDirty = false;

	/// <summary>
	/// The two cameras.
	/// </summary>
	CamFeed cameras[2];

	/// <summary>
	/// The OpenGL enabled window
	/// </summary>
	wxGLCanvas * glCanvas;

	/// <summary>
	/// The drawing context for the wxGLCanvas
	/// </summary>
	wxGLContext * glContext;

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

	void Repaint();

	void OnSize(wxSizeEvent & evt);

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
	EVT_SIZE(OpenCVTest::OnSize)
END_EVENT_TABLE()

OpenCVTest::OpenCVTest()
	: wxFrame(nullptr, wxID_ANY, "OpenCV Test")
{
	// Initialize the OpenGL canvas.
	this->glCanvas = new wxGLCanvas(this, wxID_ANY);
	//
	wxGLContextAttrs attrs;
	attrs.CoreProfile().Robust().ResetIsolation().EndList();
	this->glContext = new wxGLContext(this->glCanvas, nullptr, &attrs);

	this->glContext->SetCurrent(*this->glCanvas);
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
	keepRunningThreads = false;

	// Wait for the threads to actually finish.
	for(int i = 0; i < 2; ++i)
		this->cameras[i].threadUpdate.join();

	// Clean up camera stream resources
	for(int i = 0; i < 2; ++i)
		this->cameras[i].cap.release();

	delete this->glContext;
	this->glContext = nullptr;

	this->Destroy();
}

void OpenCVTest::OnUpdateMat(wxCommandEvent& evt)
{
	// If we still have updates after we cleanup for 
	// closing the window.
	if(this->glContext == nullptr)
		return;

	this->glCanvas->SetCurrent(*this->glContext);

	for(int i = 0; i < 2; ++i)
	{
		CamFeed & outp = this->cameras[i];

		// If any are dirty, update (or create) their OpenGL texture.
		bool wasDirty = false;
		outp.matMutex.lock();
			// Store the variable to clear out the old reference so we can keep
			// using out local version - but also so we don't need to keep the
			// mutex locked longer than we need to.
			std::shared_ptr<cv::Mat> mat = outp.matRef;
			wasDirty = outp.dirty;
			outp.dirty = false;
		outp.matMutex.unlock();

		if(!wasDirty)
			continue;

		if(mat.get() == nullptr || mat->empty())
			continue;

		if(outp.texID == (GLuint)-1)
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
		if (i == 0)
		{
			// Transfer 3 channel RGB
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
		}
		else // if(i == 1)
		{
			// Transfer 1 channel greyscale
			//
			// While the feed is alpha, we do a post process on it
			// that converts it to greyscale.
			glTexImage2D(
				GL_TEXTURE_2D, 
				0, 
				GL_RED, 
				mat->cols, 
				mat->rows, 
				0, 
				GL_RED, 
				GL_UNSIGNED_BYTE,
				mat->ptr());
		}
		
	}

	this->Repaint();
}

void OpenCVTest::Repaint()
{
	// Allow things to get dirty and signal a repaint now that we've 
	// started drawing. It shouldn't affect anything that's going to
	// be drawn this frame, but the processing to queue the next frame
	// can be parallelized by freeing the flag at the beginning of
	// the GL drawing.
	this->updateDirty = false;

	this->glContext->SetCurrent(*this->glCanvas);

	// Arbitrary clear color just to make sure its obviously 
	// clearing correctly.

	//		SETUP THE VIEWPORT AND BASIC DRAWING STATES
	//
	//////////////////////////////////////////////////

	// We don't actually need to clear anything because we're
	// going to redraw over everything indiscriminatly anyways.
	//
	glClearColor(0.5f, 1.0f, 0.75f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	wxSize canvasSz = this->glCanvas->GetSize();
	// We're doing a little cheat here and using the hard
	// coded dimensions of the glCanvas.
	glViewport(0, 0, canvasSz.x, canvasSz.y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, canvasSz.x, canvasSz.y, 0.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	
	// Set the modulating color of the texture.

	//		DRAW THE GEOMETRY
	//
	//////////////////////////////////////////////////
	glDisable(GL_BLEND);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, this->cameras[0].texID);

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
		glVertex3f(canvasSz.x, 0.0f, 0.0f);
		//
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(canvasSz.x, canvasSz.y, 0.0f);
		//
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0.0f, canvasSz.y, 0.0f);
	glEnd();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0, 1.0, 1.0, 0.25f);
	glBindTexture(GL_TEXTURE_2D, this->cameras[1].texID);
	
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		//
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(canvasSz.x/2, 0.0f, 0.0f);
		//
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(canvasSz.x/2, canvasSz.y/2, 0.0f);
		//
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0.0f, canvasSz.y/2, 0.0f);
	glEnd();

	// The OpenGL canvas is double buffered, so swap it to show the
	// changes we just drew.
	this->glCanvas->SwapBuffers();
}

void OpenCVTest::ThreadFn_UpdateCap1(int idx)
{
	const int apiID = cv::CAP_FFMPEG;

	CamFeed & outp = this->cameras[idx];

	// open selected camera using selected API
	// !TODO: Make the host configurable via command line options
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

	outp.open = true;

	while(keepRunningThreads)
	{
		// Aiming for roughtly 30FPS, so sleeping for 30ms
		const int msSleep = 30;
#ifdef WIN32
		Sleep(msSleep);
#else
		usleep(msSleep);
#endif

		// Read camera stream frame
		std::shared_ptr<cv::Mat> mat = std::shared_ptr<cv::Mat>(new cv::Mat());
		this->cameras[idx].cap.read(*mat.get());

		if(mat.get() == nullptr || mat->empty())
			continue;

		if(idx == 1)
		{
			// If the second video feed, do an example of OpenCV processing in the
			// thread. Make sure to do as much as possible in this thread instead
			// of the main UI app-thread.
		
			// This is somewhat a port of what's in the FarPi repo.
		
			cv::Mat matGray;
			cv::cvtColor(*mat, matGray, cv::COLOR_BGR2GRAY);
		
			cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.7, cv::Size(8, 8));
			cv::Mat matClahe; // HACK for now
			clahe->apply(matGray, matClahe);
			
			cv::threshold(matClahe, *mat, 128.0, 255.0, cv::THRESH_BINARY);
			
			// threshhold_yen(...)
			//const int nbins = 256;
			//
			// TODO: Port the rest of the thresholding here
			//
			// End threshold_yen()
		}

		// Get authority to write directly to matRef
		outp.matMutex.lock();
			outp.matRef = mat;
			outp.dirty = true;
		outp.matMutex.unlock();

		if(this->updateDirty)
			continue;

		// Send a message to the app to update the 
		// display with the matrix we just cached.
		//
		// wxPostEvent is thread safe.
		wxCommandEvent event(_MYEVT_THREADIMGCHANGE, idx);
		wxPostEvent(this, event);
	}
}

void OpenCVTest::OnSize(wxSizeEvent & evt)
{
	this->glCanvas->SetPosition(wxPoint(0, 0));
	this->glCanvas->SetSize(this->GetClientSize());

	this->Repaint();
}
// FILE: 
//  OpenCV_Simple.cpp
//
// COPYRIGHT: 
//  University of Washington in St. Louis
//  Part of an opensource project, but the copyright 
//  has not been explicitly decided yet.
//
// DESCRIPTION:
//  Program OCVSimple.
//  
//  A command line tool to test various features of OpenCV when
//  compiled as a C++ application. This was be used to test featuresets
//  and diagnose issues when surveying the requirements of OpenCV
//  for the needs of the CVG tool.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <string>
#include <vector>

/// <summary>
/// The instance of each camera to show.
/// </summary>
struct CameraInst
{
    // The title for imshow
    std::string title;      

    // The camera path. Can be empty, in which case id is used instead.
    std::string path;       

    // The id to use if path is empty.
    int id;

    // The OpenCV video stream
    cv::VideoCapture capture;

    // The (hopefully reused) image buffer
    cv::Mat mat;
};

std::string GetBackendString(int backend);
int GetBackendFromString(const std::string & strBackend);

int main(int argc, char ** argv)
{
    //////////////////////////////////////////////////
    //      
    //      SHOW HELP
    //
    //////////////////////////////////////////////////

    // Check if it's a help request. It gets it own pass because if we show
    // help, then we exit right after (and ignore all other parameters and
    // functionality).
    for(int i = 1; i < argc; ++i)
    {
        std::string strArg = argv[i];
        if(strArg != "--help")
            continue;

        // Use space characters for text alignment... DO NOT USE TABS!
        std::cout << 
            "OCVSimple\n"
            "\n"
            "A testbed application to analyze the capabilities of an OpenCV\n"
            "library build (to be used in CVG C++ applications).\n"
            "\n"
            "usage [options] "
            "  options:\n"
            "    --binfo     Display build information.\n"
            "    --count     Camera count [0-2].\n"
            "    --c1 <S>    Set string path of camera 1.\n"
            "    --c2 <S>    Set string path of camera 2.\n"
            "    --equit     Quit after initialization.\n"
            "    --v         Verbose logging.\n"
            "    --api <BE>  Select a support API backend.\n"
            "\n"
            "  backends:\n"
            "    CAP_ANY     Use default\n"
            "    CAP_FFMPEG  Use FFMPEG (streaming)\n"
            "    CAP_MSMF    Use Microsoft media foundations.\n"
            "    CAP_V4L     Use V4L/V4L2\n"
            "    \n";

        return 0;
    }

    //////////////////////////////////////////////////
    //      
    //      PARSING OPTIONS
    //
    //////////////////////////////////////////////////

    // Example options
    //
    // > OCVSimple
    //  With no parameters, opens a single camera tied to index 0.
    //
    // > OCVSimple --count 2
    //  Opens two cameras: index 0 and index 1.
    //
    // > OCVSimple --count 2 --c1 /dev/video0 --c2 /dev/video2
    //  Opens two cameras: One at path /dev/video0, and another at /dev/video2
    //
    // > OCVSimple --count 0 --binfo
    //  Show the build info, then exit the app.
    //
    // > OCVSimple --count 1 --c1 /dev/video0 --equit
    //  Validate through testing that /dev/video0 can be opened, and then quit.

    // Number of cameras, either 0, 1 or 2. Using 0 doesn't
    // really make sense if showBuild is turned off, because
    // it would just a do-nothing app.
    int camnum = 1;             

    // Spit out the build information?
    bool showBuild = false;     

    // Show lots of diagnostic text?
    bool verbose = false;       

    // Early quit allows us to get as far as making sure stuff
    // works, but won't go an indefinite app loop.
    bool equit = false;

    std::vector<CameraInst> camInsts;

    int apiID = cv::CAP_ANY;

    // Preallocate the max number of cameras. It may be slight overhead
    // if we don't use both, but it's trivial and simplified some things.
    const int maxSupportedCamCount = 2;
    for(int i = 0; i < maxSupportedCamCount; ++i)
    {
        CameraInst ci;
        ci.id = i;
        ci.title = std::string("Camera ") + std::to_string(i + 1);
        // Other members of ci filled in later.
        camInsts.push_back(ci);
    }

    // The actual, parsing
    for(int i = 1; i < argc; ++i)
    {
        std::string strArg = argv[i];
        if( strArg == "--binfo")
            showBuild = true;

        if(strArg == "--count")
        {
            if(i + 1 >= argc)
            { 
                std::cerr << "Missing camera count for --count.";
                return 1;
            }
            ++i;

            try
            { 
                camnum = std::stoi(argv[i]);
            }
            catch( std::exception & ex)
            {
                std::cerr << "Error parsing camera count \"" << argv[i] << "\": " << ex.what();
                return 1;
            }

            if(camnum > 2)
            {
                std::cerr << "The camera count must be between [0, 2].";
                return 1;
            }
        }
        else if(strArg == "--c1")
        {
            if(i + 1 >= argc)
            { 
                std::cerr << "Missing camera path for --c1.";
                return 1;
            }

            ++i;
            camInsts[0].path = argv[i];
        }

        else if(strArg == "--c2")
        {
            if(i + 1 >= argc)
            { 
                std::cerr << "Missing camera path for --c2.";
                return 1;
            }
            ++i;
            camInsts[1].path = argv[i];

            // Using --c2 automatically makes the camera count at least 2
            camnum = std::max(camnum, 2);
        }
        else if(strArg == "--v")
        {
            verbose = true;
            std::cout << "Verbose flag turned on." << std::endl;
        }
        else if(strArg == "--equit")
        {
            equit = true;
        }
        else if(strArg == "--api")
        {
            if(i + 1 >= argc)
            { 
                std::cerr << "Missing backend name for --api.";
                return 1;
            }
            ++i;
            std::string api = argv[i];
            apiID = GetBackendFromString(api);

            if(apiID == -1)
            {
                std::cerr << "Invalid API " << api << std::endl;
                return 1;
            }
        }
    }

    //////////////////////////////////////////////////

    if(showBuild == true)
        std::cout << cv::getBuildInformation();

    if(camnum <= 0)
    {
        if(verbose)
            std::cout << "Camera count is 0, exiting early." << std::endl;

        return 0;
    }

    if(verbose)
    {
        // Yes, there's a double newline at the end. The first one (\n) for
        // ending a line, and the second one because we're using a convention
        // where each end of a line statement uses an endl.
        std::cout << "Camera number: " << camnum << "\n" << std::endl;
    }


    //////////////////////////////////////////////////
    //      
    //      INITIALIZATION
    //
    //////////////////////////////////////////////////

    if(verbose)
    { 
        std::cout << "Initializing cameras: \n" << std::endl;
        std::cout << "\tUsing API: " << GetBackendString(apiID) << std::endl;
    }

    for(int i = 0; i < camnum; ++i)
    {
        if(verbose)
            std::cout << "Camera " << (i+1) << std::endl;

        CameraInst & curCam = camInsts[i];

        if(camInsts[i].path.empty() )
        {
            if(verbose)
            { 
                std::cout << "No named path specificed, using int index." << std::endl;
                std::cout << "Camera_" << (i+1) << " = VideoCapture(" << curCam.id << ", cv::" << GetBackendString(apiID) << ")" << std::endl;
            }
            curCam.capture.open(curCam.id, apiID);
        }
        else
        {
            if(verbose)
            { 
                std::cout << "Named path specificed, " << curCam.path << std::endl;
                std::cout << "Camera2 = VideoCapture(" << curCam.path << ", cv::" << GetBackendString(apiID) << ")" << std::endl;

                curCam.capture.open(curCam.path, apiID);
            }
        }

        if(curCam.capture.isOpened())
        {
            if(verbose)
            { 
                std::cout << "Cameara opened successfully!" << std::endl;
                std::cout << "\tBackend " << curCam.capture.getBackendName() << std::endl;
                std::cout << "\tWidth: " << curCam.capture.get(cv::CAP_PROP_FRAME_WIDTH) << std::endl;
                std::cout << "\tHeight: " << curCam.capture.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;
                std::cout << "\tFPS: " << curCam.capture.get(cv::CAP_PROP_FPS) << std::endl;
                std::cout << "\tFormat: " << curCam.capture.get(cv::CAP_PROP_FORMAT) << std::endl;
            }

        }
        else
        {
            if(verbose)
            { 
                // If we cared for rigor (which we don't) we would close all open VideoCaptures, which
                // would be possible if a second capture failed to open, but a first was successful.
                std::cout << "Failed to open camera! Quitting." << std::endl;
                return 1;
            }
        }
        std::cout << std::endl;
        
    }

    if(equit)
    {
        if(verbose)
            std::cout << "Cameras initialized correctly, but aborting because of --equit option." << std::endl;

        return 0;
    }

    //////////////////////////////////////////////////
    //      
    //      APPLICATION LOOP
    //
    //////////////////////////////////////////////////

    if(verbose)
        std::cout << "Running application loop. Exit terminal application to quit." << std::endl;

    while(true)
    {
        for(int i = 0; i < camnum; ++i)
        {
            CameraInst & curCam = camInsts[i];
            curCam.capture.read(curCam.mat);

            if(curCam.mat.empty())
                continue;

            cv::imshow(curCam.path, curCam.mat);
        }
        cv::waitKey(1);
    }
}

// This function should be coupled with GetBackendFromString() to provide
// bidirectional mapping between OpenCV backend values and names.
//
// I'm not really familiar with an enum-name<->enum mapping functions
// in OpenCV so we're creating our own system for that. And we're only
// supporting relevant names. Add support to more backend enums if needed.
// (wleu 01/17/2022)
std::string GetBackendString(int backend)
{
    switch(backend)
    {
    case cv::CAP_ANY:
        return "CAP_ANY";

    case cv::CAP_FFMPEG:
        return "CAP_FFMPEG";

    case cv::CAP_MSMF:
        return "CAP_MSMF";

    case cv::CAP_V4L: // Shared value with CAP_V4L2
        return "CAP_V4L";

    case cv::CAP_GSTREAMER:
        return "CAP_GSTREAMER";
    }
    return std::string("unknown_") + std::to_string(backend);
}
// Inverse mapping of GetBackendString()
int GetBackendFromString(const std::string & strBackend)
{
    if(strBackend == "CAP_ANY")
        return cv::CAP_ANY;

    if(strBackend == "CAP_FFMPEG")
        return cv::CAP_FFMPEG;

    if(strBackend == "CAP_MSMF")
        return cv::CAP_MSMF;

    if(strBackend == "CAP_V4L" || strBackend == "CAP_V4L2")
        return cv::CAP_V4L;

    if(strBackend == "CAP_GSTREAMER")
        return cv::CAP_GSTREAMER;

    return -1;
}
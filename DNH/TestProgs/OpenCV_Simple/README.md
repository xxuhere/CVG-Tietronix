# OpenCV Simple

Utility application to test features of a C++ OpenCV library. It's used to probe details of OpenCV to better understand its capabilities and limitations for use in the CVG project.

Type ```make``` in the directory for Linux to make the application (OCVSimple). Or use the solution file to make the application for Linux.

## Scripts

OpenCV is designed to be a command line program. While it opens graphical windows using OpenCV's imshow(), the details of how it does that needs to be provided in the command line.



For a list of OpenCV options use

```./OCVSimple --help```

The .sh scripts are short and simple, and also have summaries of their task as script comments.

## Observations

Observations and knowledge that was gathered from testing:

- Linux will use CAP_V4L or CAP_GSTREAMER as its default API implementation for direct camera connections.
- Windows will use CAP_MSMF (Microsoft Media Foundations) as its API implementation for direction camera connections.
- CAP_FFMPEG should be used for RTSP streaming on both Linux and Windows. This should be forced on Windows as it may try to use MSMF which has odd and low-performance behavior.
- The requirement of CAP_FFMPEG makes FFMPEG a dependency.
- Local camera streaming will stutter on the RaspberryPi. This may be a backend issue, as examples used for testing v412rtspserver use low image resolution.

## Notes

* This testbed and the scripts use OpenCV's ```imshow()``` function and is not multithreaded. See  _TestProgs/wxMultiCamOpenCVGL_ and _TestProgs/wxMultiCamOpenCVGL_Stream_ for examples of embedding the graphical content into an OpenGL viewport with multiple threads.
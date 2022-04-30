// VidYUVLumaTester.cpp
// A short application that tests a binary capture of video taken from
// raspividyuv with the hardcoded expectations:
// - YUV luma (greyscale)
// - 1920 x 1080 resolution
// - 30 FPS
// - NO stereo options

// Heavily modified from Script1.cpp of Stereopi-cpp-tutorial,
// https://github.com/realizator/stereopi-cpp-tutorial/blob/master/src/script1.cpp
// Copyright (C) 2019 Eugene a.k.a. Realizator, stereopi.com, virt2real team
// Ported from Python to C++ by Konstantin Ozernov on 10/10/2019.
// ...
// Licensed as GNU, http://www.gnu.org/licenses/

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <windows.h>

int main()
{
    int imgWidth = 1920;
    int imgHeight = 1080; // There seems to always be an extra 8 rows

    std::cout << "Camera resolution: " << imgWidth << " x " << imgHeight << std::endl;

    const char* szFile = "C:\\Users\\Reavenk\\Desktop\\test4.rec";
    std::ifstream ifsReader;
    ifsReader.open(szFile, std::ios::binary);

    if(!ifsReader.is_open())
    {
        std::cout << "Cannot open input file!" << std::endl;
        return 1;
    }

    int bufLen      = imgWidth * imgHeight;
    int borderLen   = imgWidth * 8;     // For some reason, there's an extra 8 pixel rows per frame.

    char* buf = new char[bufLen];
    while (true)
    {
        ifsReader.read(buf, bufLen);    // Read RAW luma image data
        ifsReader.ignore(borderLen);    // Skip excess row

        // Load image into OpenCV and show
        cv::Mat frame(imgHeight, imgWidth, CV_8UC1, buf);
        cv::imshow("video", frame);

        char k = cv::waitKey(1);        // Flush image
        Sleep(33);                      // Naive/approximate timing mechanism

        if(ifsReader.eof())
            break;
    }
    return 0;
}
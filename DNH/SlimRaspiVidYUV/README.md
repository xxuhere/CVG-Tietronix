# Purpose

This is an iterative test to build 

RaspiVidYUV and find what C/C++ elements are crucial to the command

`raspividyuv -3d sbs -w 640 -h 240 -fps 30 --luma -t 0 -n -o –`

Where we're most interested in

* luma 1 channel output. We will be expecting a video stream with a 1 unsigned bit greyscale per pixel.
* Image resolution control, width and height. In the sample above it's a 640x240 image, but we want variable control.

Every other unlisted feature is secondary, or something we will be rolling out our own version for our own needs later.

# Strategy

The plan is to build it and iteratively remove elements until the core elements can be shown, explained and demoed, all while mapping back to the original RaspiVidYUV application. This will be done through a series of commits in its own branch.

# Installing

Before being able to build anything, the userland repo should be pulled and installed,
https://github.com/raspberrypi/userland

Pull the repo into its own repository (NOT inside CVG or any other existing repo), and run the buildme script. This will not only build dependent MMAL libraries, but install them into the correct directories, that this project can then reference and build from in the Makefile.
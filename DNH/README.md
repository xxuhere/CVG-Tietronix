# DNH Instructions and Notes
--------------------------------------------------

The Device Network Hub (DNH) is a central communication piece for different hardware, software and IT elements to comunicate and form the CVG system.

It designed to be as portable as possible. Although the biggest requirements are that is compiles for Raspbian Buster for RaspberryPi, and Windows 10.  Both for 64 bit instruction sets. Instructions for Windows and Linux are as follows.

## Building on Windows

For Windows, only Visual Studio 2019 (aka version 16) is explicitly tested and supported.

**Boost 1.77**
The build expects Boost 1.77. A modern version of the Boost library is required to ensure its build scripts support Visual Studio 2019.
https://www.boost.org/users/history/version_1_77_0.html
Download, unzip and follow its instructions (in the index html files -> getting started guide) to build a 64 bit version.

Most of the setup process should be automated with scripts that come included in the repository. At the end of the process, there should be a {BOOST_DIR}/stage/lib directory with the built *.lib and *.dll files.

Note that at least until, release we will be using the debug variant of Boost so make sure to build it and link to it in .props later.

**OpenSSL**
The process for building OpenSSL can be complex because it involves other dependency libraries. While it can be build from source, it is suggested that pre-built binaries (for Visual Studio 2019) be downloaded.
https://wiki.openssl.org/index.php/Binaries
OpenSSL 1.1.1 is expected.

**OpenCV (and dependencies)**

OpenCV should be obtained with [vcpkg](https://github.com/microsoft/vcpkg). This is because certain features of OpenCV are required that bring in dependencies that vcpkg can help manage. The biggest dependency being FFMPEG for web streaming.

The instructions below for vcpkg are modified from the CVG GAINS repo. vcpkg was used to get OpenCV for the other project, it is recommended that the old vcpkg be deleted in order to avoid issues that may occur with multiple OpenCV checkouts.

```cmd
> git clone https://github.com/microsoft/vcpkg
> cd vcpkg
> .\bootstrap-vcpkg.bat
```

Search for package:

```cmd
> .\vcpkg search [search term]
```

Install OpenCV x64 and dependencies with:

```cmd
> .\vcpkg .\vcpkg install opencv[ffmpeg]:x64-windows-static-md  nlopt:x64-windows-static-md  boost-asio:x64-windows-static-md  ffmpeg[x264]:x64-windows-static-md  opengl-registry[core]:x64-windows-static-md
```
Note, this may take a while


### Setting Up Visual Studio

The DNH project for VisualStudio uses project property sheets. This allows each user to specify their own unique locations for Boost and OpenSSL, as well as any other dependencies that may occur in the future.

1. In the DNH/CVG_DNGBackbone directory, rename the file `CoreDependencies._props` to `CoreDependencies.props`
2. `CoreDependencies.props` Open it in a text editor.
3. Change the entry inside the XML tag `OPENSSL` to the correct location.
4. Change the entry inside the XML tag `BOOST_DIR` to the correct location.
5. Change the entry inside the XML tag `VCPKG` to the correct location
6. Save the CoreDependencies.props file.
7. Open the CVG_DNHBackbone.sln solution file (in the folder DNH) in Visual Studio.

The OpenSSL folder should have a `.lib` files in it, and an `include` directory.
The Boost directory should have a child `boost` directory in it, and a `libs/stage` directory filled with link objects files (dlls and libs).

Note that *.prop files are in the .gitignore. the *_props file is a template, and all machine specific properties should not be in the repo.

**Note that these property sheets are currently only set up in the project files for x64 Debug builds.**

## Building on Linux

The build for Linux is centered around the RaspberryPi, specifically Raspbian Buster for the RaspberryPi.

**Boost 1.67**
To build, make sure to get Boost 1.67. More recent versions are ideal, but may not be available in the package manager for supported Linux systems.

````cmd
sudo apt-get install libboost1.67-all
````

**OpenSSL**
OpenSSL is also required. The expected version is 1.1.1.

````cmd
sudo apt-get install libssl-dev
````

**OpenCV**

```cmd
sudo apt-get install libopencv-dev
```

**OpenGL**

```cmd
sudo apt-get install mesa-common-dev freeglut3-dev
```

**Userland**

For access to MMAL libraries, the userland repo must be built.

1. Download the userland repo to the RaspberryPi.
2. On the RaspberryPi, `./buildme` as specified in the userland repo instructions.
   https://github.com/raspberrypi/userland
3. Then execute the `moves`  target in the DNH/HMDOpView makefile
   `make moves`
   This will move the built userland shared objects to a location the build process can access.

### Compiling

 The best way to ensure the build will run on the RaspberryPi is to actually build it on the RaspberryPi.

 This will mean the repository (or maye just the codebase without Git history) will either need to be on the RaspberryPi storage, or available to the RPi via network drive.

**g++**

If needed, install g++ (the GNU C++ compiler) on the RaspberryPi. To check if g++ is installed, open a terminal and type `whereis g++`. If found, it will show an actual path to a g++ program. Or simply enter `g++` to see if anything runs.

To install g++, enter

```cmd
sudo apt-get install g++
```

**make**

If not installed, install make.

```cmd
sudo apt-get install make
```

make is used to detect compile dependencies and automate the compile and linking process.

**making**

In a terminal, cd to the `DNH` directory. There should be a Makefile contained in it.
* If any Make attempt fails or you want to `make all` after having made just the hub `make clean` first.
* To make just the hub and not the dashboard interface enter `make hub`. 
* If you want to make both the hub and the dashboard install wxWidgets as described in the README_Dashboard.md first and then enter `make all` to being the compile process.  

If successful no errors will be show and, it will create a `DNH` executable in the directory.

## Vendored Libraries

Several libraries are vendored and are contained in the `CVG_DNHBackbone/Vendored_Libs` directory. More information is available in the README.md contained in that folder.

Libraries such as Boost or OpenSSL are not vendored either because of their complexity to maintain with respect to portability, or because of their filesize.

## Using the DNH

The DNH server is a terminal application that will open a HTTP server on port 5700 and a WebSockets server on port 5701.

The API and its documentation is currently a work in development, but there are sample files in the `Python Samples` and `Web Samples` folder.

## Python Samples

Samples on how to connect Python modules to it can be found in the  `DNH/SampleEquipment` folder.

## Web Samples

Samples on how browsers and web technologies can connect to it can be found in the `DNH/WebSamples` folder.

## Coding

The DNH codebase uses C++14. The general rule being applied is that the C++ standard being used should be the most recent one that's at least 5 years  old. While this is somewhat arbitrary, this is done to make sure the project uses a version of C++ that available on all supported platforms, as well as helping to make sure the C++ compilers have a certain level of maturity.

## Utility Scripts

There is quite a bit of setup work that can be involved with running the DNH. Because of that, there are scripts designed to make starting and restarting the DNH and camera servers as trouble-free as possible. They are the files `cvgserverboot.sh` and `cvgclearsession.sh`.

* **cvgserverboot.sh** will stop all Python scripts, and run/restart the DNH and v4l2rtspserver instances. For more information, access its help by running `. cvgserverboot.sh --help`.
* **cvgclearsession.sh** will stop all Python scripts, v4l2rtspserver instances and the DNH server. It will not restart any of those programs. For more information, access its help by running `. cvgclearsession.sh`.

They are designed to be convenient when starting the system from an ssh terminal. An ssh session will start the terminal in the user's home directory. Because of that, the scripts have features to add symlinks of themselves into the user's home directory. To add the symlink references of these scripts to your home directory, in the DNH folder, run `. cvgserverboot.sh --install; . cvgclearsession.sh --install`.

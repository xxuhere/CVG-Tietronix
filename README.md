The HMD Op View is an application worn by a fluorescent guided surgery operator to guide them through visualization and execution via a head mounted display (HMD).



The application is designed to build for Windows 10 (x64), and Raspbian Buster (AArch32) for RaspberryPi.

# Vendored Libraries

Several libraries are vendored and are contained in the `src/Vendored` directory. These are for 3rd party libraries that are embedded directly in the repo. 

Either because: 

* it is more convenient to do so because of their small size - rather than require an extra installation step.
*  a very specific version is required to be used with the codebase, and it's easier to maintain that version directly.

Libraries such as Boost or OpenSSL are not vendored either because of their complexity to maintain with respect to portability, or because of their filesize.

# Dependencies

There are multiple 3rd party libraries that will need to be installed, usually through a package manager. 

For instructions on which libraries are needed for which platforms, and the suggested way to install them, see the building instructions below (see [Building On Windows](#Building on Windows) and [Building On Linux](#Building on Linux) )

* OpenCV, Open computer vision library.
  * Image loading, image processing, and image saving.
* Boost
  * A collection of miscellaneous popular C++ libraries.
* OpenSSL
  * Internet cryptography library
* OpenGL, Open Graphics Library
  * A hardware accelerated graphics rendering API. The application is using an old version of the API, 2.0. In the future this may be bumped up to a more modern version of OpenGL ES if the need arises and if it is confirmed that the RPi supports it and it has the feature-set needed.
* wxWidgets
  * Desktop GUI wrapper library. The library allows a single GUI API that can be used on multiple platforms. It also provides miscellaneous desktop utility functions.
* FTGL
  * Font rendering library for OpenGL 2.0.
* FreeType
  * Dependency for FTGL, for parsing font files.
* DCMTK, Dicom Toolkit
  * Library for handling Dicom. While the library has support for both Dicom internet transmission and Dicom files, this application is only interested in file support.

# Repo Structure

* In the root of the repo, supporting directories that are not relevant for program exectution or the codebase will start with an underscore ('_').
* The root directory will contain runtime files, as well as codebase project files (VS solution files, Makefiles, etc.). Other important miscellaneous files will also be in the root.
* `_DesignAndDocs` will contain reference design files.
* `_Specifications` will contain application specifications and file format descriptions.
* `_SrcAssets` will contain the source file for application graphics and media.
* `_TechDocAssets` will contain technical documentation and diagrams.
* `Assets` will contain runtime assets.
* `src` will contain the codebase.

# Building on Windows

The application is supported for Windows operating systems; specifically Windows 10. This is to leave open the possibility of having a version of the application usuable for x86/x64 Windows systems, as well as giving developers the option to work in a Windows development environment (Visual Studio); both for development and debugging.

Some features may not be available on Windows, such as a RasberryPi cameras and GPIO functionality.

Development for Windows builds expects Visual Studio, specifically Visual Studio 2019 (aka version 16). Other versions may work but are not supported.

## Vcpkg Package Manager

For Windows, the package manager Vcpkg is used to download and compile dependent 3rd party libraries.

Information for getting and setting up the package manager can be found on its [getting started](https://vcpkg.io/en/getting-started.html) webpage. Steps 1 and 2 should be followed to download a copy of the application via `git clone`, as well as setting it up by calling its bootstrap script. Note that from time-to-time, the `vcpkg` repo may need a `git pull` call to update it.

```cmd
> git clone https://github.com/Microsoft/vcpkg.git
> .\vcpkg\bootstrap-vcpkg.bat
```

Search for package:

```cmd
> .\vcpkg search [search term]
```

Install OpenCV x64 and dependencies with:

## 3rd Party Libraries

The list of dependencies to install for Windows. 

* 3rd party depencency repositories should not be cloned into this repository. None of these dependencies are designed to be git submodules, so there should not be any nesting of repositories inside other repositories.
* Any mention of vcpkg assumes the command is being run from the vcpkg repository.

### OpenCV

Install OpenCV x64 and dependencies with:

```cmd
> .\vcpkg .\vcpkg install opencv[ffmpeg]:x64-windows-static-md  nlopt:x64-windows-static-md  boost-asio:x64-windows-static-md  ffmpeg[x264]:x64-windows-static-md  opengl-registry[core]:x64-windows-static-md
```

Note, this may take a while.

Note, ffmpeg may not be needed, this is a holdover from a previous iteration, but this will require testing its removal before changing the README to exclude it.

### wxWidgets

The library can be download from https://www.wxwidgets.org/downloads/. Download and build an x64 version.

Version 3.1.5 is recommended.

### OpenSSL

The process for building OpenSSL can be complex because it involves other dependency libraries. While it can be built from source, it is suggested that pre-built binaries (for Visual Studio 2019) be downloaded.
https://wiki.openssl.org/index.php/Binaries
OpenSSL 1.1.1 is expected.

### Boost 1.77

The build expects Boost 1.77. A modern version of the Boost library is required to ensure its build scripts support Visual Studio 2019.
https://www.boost.org/users/history/version_1_77_0.html
Download, unzip and follow its instructions (in the index html files -> getting started guide) to build a 64 bit version.

Most of the setup process should be automated with scripts that come included in the repository. At the end of the process, there should be a `{BOOST_DIR}/stage/lib directory` with the built `*.lib` and `*.dll` files.

Note that at least until, release we will be using the debug variant of Boost so make sure to build it and link to it in .props later.

### FTGL

```cmd
vcpkg install ftgl:x64-windows opencv:x64-windows
```

### DCMTK

```cmd
vcpkg install dcmtk:x64-windows
```

## Setting Up Visual Studio

The project for VisualStudio uses project property sheets. This allows each user to specify their own unique locations for Boost and OpenSSL, as well as any other dependencies that may occur in the future.

When assigning values in the XML file, note that there are sample paths that can be referenced.

1. In the `src` directory, rename the file `CoreDependencies._props` to `CoreDependencies.props`
2. Open `CoreDependencies.props` in a text editor.
3. Change the entry inside the XML tag `OPENSSL` to the correct location.
4. Change the entry inside the XML tag `BOOST_DIR` to the correct location.
5. Change the entry inside the XML tag `VCPKG` to the correct location.
6. Save the `CoreDependencies.props` file.
7. Open the `HMDOpView.sln` solution file (in the folder DNH) in Visual Studio.

Notes on the path values being set:

* The OpenSSL folder should have a `.lib` files in it, and an `include` directory.
* The Boost directory should have a child `boost` directory in it, and a `libs/stage` directory filled with link objects files (dlls and libs).
* The VCPKG directory will be the vcpkg repo.

Note that `*.prop` files are in the `.gitignore`. the `*_props` file is a template, and all machine specific properties should not be in the repo.

# Building on Linux

The application supports builds on Linux. Specifically for Rasberry Pi4, running Raspbian Buster.

## Apt Package Manager

The Apt package manager is used to obtain required tools and 3rd party libraries.

## Tools

There are some tools that will need to be installed via `apt-get`.

**g++**

If needed, install g++ (the GNU C++ compiler) on the RaspberryPi. To check if g++ is installed, open a terminal and type `whereis g++`. If found, it will show an actual path to a g++ program. Or simply enter `g++` to see if anything runs.

To install g++, enter

```bash
sudo apt-get install g++
```

**make**

If not installed, install make.

```bash
sudo apt-get install make
```

make is used to detect compile dependencies and automate the compile and linking process.

## 3rd Party Libraries

The list of 3rd party libraries to install for RaspberryPi.

* Cloned repositories should not be cloned into this repository.

### OpenCV

```bash
sudo apt-get install libopencv-dev
```

### wxWidgets

wxWidgets is built from source to get a specific version to match with the Windows version.
Download and unpack the wxWidgets on the Linux machine or a location network accessible to the Linux machine.

The instructions below are originally from https://www.binarytides.com/install-wxwidgets-ubuntu/, but have been modified.

1. Download wxWidgets 3.1.5 and unpack it. (Do NOT put it in the CVG repo.)
2. Open a terminal and cd into the wxWidgets directory.
3. `sudo apt-get install libgtk-3-dev build-essential`
4. `mkdir gtk-build`
5. `cd gtk-build/`
6. `../configure --with-opengl --disable-shared --enable-unicode`
7. `make`
8. `sudo make install`

To test that wxWidgets has been installed, type `wx-config`, which should run an installed script.

### OpenSSL

The expected version is 1.1.1.

````cmd
sudo apt-get install libssl-dev
````

### Boost 1.67

Make sure to get Boost 1.67. More recent versions are ideal, but may not be available in the package manager for supported Linux systems.

````bash
sudo apt-get install libboost1.67-all
````

### OpenGL Development Libraries

```
sudo apt-get install mesa-common-dev freeglut3-dev
```

### FTGL and FreeType

``` bash
sudo apt-get install libftgl-dev
sudo apt-get install libfreetype6-dev
```

### DCMTK

To install the Dicom Toolkit (DCMTK), a version of DCMTK at/above version 3.6.7 is required. This is higher than what is currently supported on apt-get, so it will need to be built manually.

1. Clone the repo from https://github.com/DCMTK/dcmtk.git 
   1. Release 3.6.7 is required.  This is because that's what we're targeting, and different versions may require changes to the application makefile.
2. In the repository, follow the instructions in `INSTALL` file under `Unix with CMake`
   1. For the target release, see instructions in the INSTALL starting at line 686.
3. For the install instruction, instead run `sudo make install` to install into the standard directories.

To follow the instructions above, you can use the commands below.

```bash
git clone https://github.com/DCMTK/dcmtk.git 
cd dcmtk
git checkout tags/DCMTK-3.6.7
mkdir build
cd build
cmake ..
make -j8
sudo make install
```

### Userland

For access to MMAL libraries, the userland repo must be built.

1. Download the userland repo to the RaspberryPi.
2. On the RaspberryPi, `./buildme` as specified in the userland repo instructions.
   https://github.com/raspberrypi/userland
3. Then execute the `moves`  target in the DNH/HMDOpView makefile
   `make moves`
   This will move the built userland shared objects to a location the build process can access.

```bash
git clone https://github.com/raspberrypi/userland.git
cd userland
sudo ./buildme

# Afterwards, use the target `moves` in the makefile to copy the built MMAL library files into the expected location for g++
cd <this_repo>
sudo make moves
```



## Building

The best way to ensure the build will run on the RaspberryPi is to actually build it on the RaspberryPi.

 This will mean the repository (or maybe just the codebase without Git history) will either need to be on the RaspberryPi storage, or available to the RPi via network drive.

In a terminal, cd to the repo directory. There should be a `Makefile` contained in it.

```
make clean
```

To clean the current project of any precompiled data.

```
make
```

To compile the project.

If successful, no errors will be shown and it will create a `hmdopapp` executable in the directory.

### Running The Application

Note: This is a graphical application and will fail to execute if it is run headless (i.e., without a display). It is suggested to run the application with a monitor attached; but if running remotely, executing through VNC has also been observed to work.

# C++

The DNH codebase uses C++17. The general rule being applied is that the C++ standard being used should be the most recent one that's at least 5 years  old. While this is somewhat arbitrary, this is done to make sure the project uses a version of C++ that's available on all supported platforms, as well as helping to make sure the C++ compilers have a certain level of maturity.

# Installation

Currently the deployment target is RaspberryPi. To quickly set up to repo, both for development and deployment, see the `RAWINSTALL.sh` script in the repository root. See the top of the script for extra details.

## Codebase

This section will contain a brief overview of the various subsystems in the codebase (./src).

* CamVideo
  * Various systems related to camera streaming and saving content from the streams.
* Carousel
  * Carousel GUI library. This is the UI system used to display the surgery phase.
* DicomUtils
  * Dicom related utilities written for the application.
* Hardware
  * The hardware system, as well as various hardware implementations.
* States
  * The application state machine, as well as sub-state machines (for states that have there own state machines).
* UISys
  * OpenGL UI system. This UI system is a general UI library, but written specifically for this application.
* Utils
  * Miscellaneous utilities.
* Vendored
  * Miscellaneous vendored libraries.



# Coding

* [Camel case](https://en.wikipedia.org/wiki/Camel_case) variable names and function names.
* Variables start with lowercase letters.
* Functions start with uppercase letters.
* Be explicit with the `std` namespaces.
* Don't use `auto` except for exceptionally long type names.
* Vertically align curly braces.
* Favor vertical code over excessively-horizontal code.
* XML docstrings are used when possible on class, member and method declarations.
* Don't apply duplicate docstrings to overloaded functions.
* Favor [return early](https://medium.com/swlh/return-early-pattern-3d18a41bba8) coding.

# License

TDB

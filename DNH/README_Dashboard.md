# Dashboard
--------------------------------------------------

The Dashboard is tool to author UI presets and operation for the DNH.

## Building on Windows

For Windows, only Visual Studio 2019 (aka version 16) is explicitly tested and supported.

The Dashboard requires everything the DNH does, as well as wxWidgets. See the "Building on Window" section in the README.md.

**wxWidgets**
The library can be download from https://www.wxwidgets.org/downloads/. Download and build an x64 version.

### Setting up Visual Studio

For Visual Studio, property sheets are used. See the instructions for "Setting Up Visual Studio" for more information on property sheets. 

Copy and rename DevProperties._props to DevProperties.props. Open up the file in a text editor and set these values for the UserMacros:
- WXWIDGETS_ROOT : The root directory for the wxWidgets codebase.
- BOOST_ROOT : The same Boost location used for DNH.
- DNH_ROOT : Set to {CVG_REPO_ROOT}\DNH
- OPENSSL_ROOT : The same Boost location used for DNH.

### Using CVGData

Before building, make sure the CVGData project (in CVG_DNHBackbone.sln) has been built, as that library is a dependency.

## Building on Linux

Perform the the instructions in README.md.

### wxWidgets
wxWidgets will also need to be installed. 
Download and unpack the wxWidgets on the Linux machine or a location network accessible to the Linux machine.

The instructions below are originally from https://www.binarytides.com/install-wxwidgets-ubuntu/, but have been modified.

1. Download wxWidget and unpack it. (It is recommended to NOT put it in the CVG repo.)
2. Open a terminal and cd into the wxWidget directory.
3. `sudo apt-get install libgtk-3-dev build-essential`
3. `mkdir gtk-build`
4. `cd gtk-build/`
5. `../configure --disable-shared --enable-unicode`
6. `make`
7. `make install`

To test that wxWidgets has been installed, type `wx-config`, which should run an installed script.

### Building

In {CVG_REPO_ROOT}/DNH, run `make dash`. This will build DashboardApp in the same folder.

## Using the Dashboard

Run the program, and in the top left of the application, enter the hostname and port of the running DNH server and press CON (for connect).

## Coding

The DNH codebase uses C++14. The general rule being applied is that the C++ standard being used should be the most recent one that's at least 5 years  old. While this is somewhat arbitrary, this is done to make sure the project uses a version of C++ that available on all supported platforms, as well as helping to make sure the C++ compilers have a certain level of maturity.
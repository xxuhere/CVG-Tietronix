Small scale working tests of various technical issues.


https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html

The projects should compile for both Windows (x64 debug) and Linux.

To compile for Linux, each folder with a C++ project has a Makefile. So type ```make``` to compile the application. Most Linux builds will compile with the application's filename "app".

To compile for Windows, each folder with a C++ project has a Visual Studio *.sln and *.vcproj. Fill in the contents of DNH/DevDependencies.prop before opening the Visual Studio solution files. See DNH/README.md for more information on how to fill in the DeveDependencies.prop file.

Instead of having a VS solution file and a Makefile, using CMake is an alternative that should be considered.
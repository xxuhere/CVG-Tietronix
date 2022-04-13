The HMD Op View is an application worn by a fluorescent guided surgery operator to guide them through visualization and execution via a head mounted display (HMD).



# Building on Windows

Follow the same instruction for building the dashboard, except for the HMD project solution.

This project will also require the use of vcpkg to install ftgl and opencv, used for parsing font files and rendering fonts in OpenGL.

```cmd
vcpkg install ftgl:x64-windows opencv:x64-windows
```



# Building on Linux

This project requires wxWidgets. Follow the same instructions for building wxWidgets in the dashboard readme.

This project will also require the use of package managers to install the library ftgl and its dependencies.

``` bash
sudo apt-get install libftgl-dev
sudo apt-get install libfreetype6-dev
```

The Makefile in the HMDOpView will make the entire application.

Note: It does not like being run headless / from an ssh terminal. It's suggested to run it from the terminal in a graphical setting like a vnc viewer.

# Using the HMD Op View

To use the HMD Op View, make sure the running machine has a web-camera attached, and then run the application.

Relevant shortcut keys during developer testing:

* Press Alt+F4 to quit (Note if using a VNC viewer to run from the pi may require manually killing it from a separate terminal /ssh session terminal
    - "ps -a" to list processes get the process ide of hmdopapp
    - "kill <process id of hmdopapp>"
* Press Win+Shift+Left/Right to move the fullscreen application between monitors.



# Coding

The coding standards uses the same as the dashboard. See the Coding section in README_Dashboard.md for more information.

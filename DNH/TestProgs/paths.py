# A script for Makefiles to use shared compilation paths that 
# are defined in a single place. While it's preferable to use
# more formalized things to get these types of paths (see Makefiles
# that use `wx-config` for example), some libraries don't have
# those - or at least none that we've identified for the way the
# package manager install it.
#
# For those things, we define them here, to get the correct text
# value in a bash or Make command line, use the format:
# `python3 paths.py --<request_path>`
# Where <request_path> is one of strings use in the if/elseif
# statements below.
#

import sys

#OpenCV include directory
if sys.argv[1] == "--ocv_inc":
    print("/usr/include/opencv4/")
#OpenCV library directory
elif sys.argv[1] == "--ocv_lib":
    print("~/sambashare/OpenCV/build/bin")
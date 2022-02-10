##################################################
#
#		GENERAL VARIABLES
#
##################################################
CC=g++
CFLAGS=-Wall

OPENCVINCL		:= -I`python3 ../../paths.py --ocv_inc`
OPENCVLIBS		:= -lopencv_core -lopencv_video -lopencv_imgproc -lopencv_videoio

##################################################
#
#		COMPILE RULES
#
##################################################

# NOTES:
# - the -lGL is needed to link OpenGL.
# - We also need to include glext.h because we used
#		it for extra defines.

all :
	@echo ""
	@echo "Compiling test application." 
	@printf "\n\n" 
	$(CC) $(CFLAGS) -o TestApp wxMultiCamOpenCVGL/wxMultiCamOpenCVGL.cpp `wx-config --cxxflags --libs --gl-libs std,aui` -IwxMultiCamOpenCVGL -I/usr/include/opencv4/ -lGL $(OPENCVINCL) $(OPENCVLIBS)
	
##################################################
#
#		GENERAL VARIABLES
#
##################################################
CC=g++
CFLAGS=-Wall

OPENCV_INC 		:= `python3 ../test.py --inc`
OPENCVCOR_INC 	:= `python3 ../test.py --cor`
OPENCV_LIB 		:= `python3 ../test.py --lib`

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
	$(CC) $(CFLAGS) -o TestApp wxMultiCamOpenCVGL/wxMultiCamOpenCVGL.cpp `wx-config --cxxflags --libs --gl-libs std,aui` -IwxMultiCamOpenCVGL -I/usr/include/opencv4/ -lGL $(OPENCVLIBS)
	
debug_var:
	@echo "     Printing values of evaluated variables"
	@echo "##############################"
	@echo ""
	@echo "OPENCV_INC with a value of $(OPENCV_INC)"
	@echo "OPENCVCOR_LIB with a value of $(OPENCVCOR_INC)"
	@echo "OPENCV_LIB with a value of $(OPENCV_LIB)"
	@echo ""
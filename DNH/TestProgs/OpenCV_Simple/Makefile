##################################################
#
#		GENERAL VARIABLES
#
##################################################
CC=g++
CFLAGS=-Wall

# Right now the path of OpenCV is hard coded with the 
# assumption of being on the Raspberry pi
OPENCVINCL		:= -I`python3 ../paths.py --ocv_inc`
OPENCVLIBS		:= -lopencv_core -lopencv_video -lopencv_imgproc -lopencv_videoio -lopencv_highgui

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
	$(CC) $(CFLAGS) -o OCVSimple OpenCV_Simple/OpenCV_Simple.cpp `wx-config --cxxflags --libs --gl-libs std,aui` -IwxMultiCamOpenCVGL $(OPENCVINCL) -lGL $(OPENCVLIBS)
	
debug_var:
	@echo " Printing values of evaluated variables"
	@echo "##############################"
	@echo ""
	@echo "OPENCVINCL with a value of $(OPENCVINCL)"
	@echo "OPENCVLIBS with a value of $(OPENCVLIBS)"
	@echo ""
##################################################
#
#		GENERAL VARIABLES
#
##################################################

CC=g++
CFLAGS=-Wall

##################################################
#
#		VENDORED HEADERS
#
##################################################

# Vendored Library as Makefile variables
#
# These are coppied from other Makefiles, and may eventually
# be relevant, but if not, feel free to remove!
VEND = ../Vendored
VEND_WS = ../Vendored/Simple-WebSocket-Server
VEND_HTTP = ../Vendored/Simple-Web-Server
VEND_JS = ../Vendored/

# Right now the path of OpenCV is hard coded with the 
# assumption of being on the Raspberry pi
OPENCVINCL		:= -I`python3 ../paths.py --ocv_inc`
OPENCVLIBS		:= -lopencv_core -lopencv_video -lopencv_imgproc -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs

VEND_ALLINCLUDES = -I$(VEND_WS) -I$(VEND_HTTP) -I$(VEND_JS)

# All vendored header files, for listing dependencies
# While these dependencies are not expected to be modified,
# we list them as dependencies to check as a formality.
ALLHEADERS_WS = $(VEND_WS)/asio_compatibility.hpp $(VEND_WS)/client_ws.hpp $(VEND_WS)client_wss.hpp $(VEND_WS)/crypto.hpp $(VEND_WS)/mutex.hpp $(VEND_WS)/server_ws.hpp $(VEND_WS)/server_wss.hpp $(VEND_WS)/status_code.hpp $(VEND_WS)/utility.hpp 
ALLHEADERS_HTTP = $(VEND_HTTP)/crypto.hpp $(VEND_HTTP)/server_http.hpp $(VEND_HTTP)/server_https $(VEND_HTTP)/status_code.hpp $(VEND_HTTP)/utility.hpp
ALLHEADERS_JSON = $(VEND_JS)/json.hpp

ALLHEADERS_ALLVEND = $(ALLHEADERS_WS) $(ALLHEADERS_HTTP) $(ALLHEADERS_JSON)

##################################################
#
#		VENDORED SOURCES
#
##################################################

# EMPTY

##################################################
#
#		PROGRAM OBJECTS
#
##################################################

SUBDIR_MAIN = .
SUBDIR_STATES = States
SUBDIR_LODEPNG = lodePNG
SUBDIR_UTILS = Utils
SUBDIR_HARDWARE = Hardware
SUBDIR_CAMVIDEO = CamVideo
SUBDIR_CAMIMPL = CamVideo/CamImpl

SUBOBJ_MAIN = \
	FontMgr GLWin HMDOpApp MainWin TexObj OpSession
	
SUBOBJ_STATES = \
	BaseState StateHMDOp StateInitCameras StateIntro
	
SUBOBJ_LODEPNG = \
	lodepng
	
SUBOBJ_UTILS = \
	cvgCamFeedSource cvgCamTextureRegistry cvgOptions cvgRect cvgShapes cvgStopwatch cvgStopwatchLeft multiplatform VideoPollType yen_threshold
	
SUBOBJ_HARDWARE = \
	HardwareMgr IHardware LaserSys
	
SUBOBJ_CAMVIDEO = \
	CamStreamMgr ManagedCam SnapRequest VideoRequest
	
SUBOBJ_CAMIMPL = \
	ICamImpl CamImpl_StaticImg CamImpl_OpenCVBase CamImpl_OCV_Web CamImpl_OCV_USB CamImpl_OCV_HWPath

EXPOBJS_MAIN = $(patsubst %,$(SUBDIR_MAIN)/%.o,$(SUBOBJ_MAIN))
EXPOBJS_STATES = $(patsubst %,$(SUBDIR_STATES)/%.o,$(SUBOBJ_STATES))
EXPOBJS_LODEPNG = $(patsubst %,$(SUBDIR_LODEPNG)/%.o,$(SUBOBJ_LODEPNG))
EXPOBJS_UTILS = $(patsubst %,$(SUBDIR_UTILS)/%.o,$(SUBOBJ_UTILS))
EXPOBJS_HARDWARE = $(patsubst %,$(SUBDIR_HARDWARE)/%.o,$(SUBOBJ_HARDWARE))
EXPOBJS_CAMVIDEO = $(patsubst %,$(SUBDIR_CAMVIDEO)/%.o,$(SUBOBJ_CAMVIDEO))
EXPOBJS_CAMIMPL = $(patsubst %,$(SUBDIR_CAMIMPL)/%.o,$(SUBOBJ_CAMIMPL))

##################################################
#
#		TARGETS
#
##################################################

# Set to empty, or comment out, for building release version
DEBUGFLAGS = -g -ggdb

all: hmdopview
	@echo "COMPILING TARGET All"
	@echo "Building HmdViewOp application"
	@echo "--------------------------------------------------"
	$(CC) $(CFLAGS) $(DEBUGFLAGS) `wx-config --cxxflags` hmdopview.a -pthread -I/usr/include/openssl -L/usr/local/aarch64-linux-gnu -lssl -lcrypto -lboost_system -lboost_filesystem -Wall `wx-config --libs std,aui --gl-libs` $(OPENCVLIBS) -lftgl -lGL -o hmdopapp
	echo "Finished build command."
	
hmdopview: objs
	@echo "COMPILING TARGET archive"

	@echo "\n\n\nMaking HMDOpView archive"
	@echo "--------------------------------------------------"
	ar -rsc hmdopview.a $(EXPOBJS_MAIN) $(EXPOBJS_STATES) $(EXPOBJS_LODEPNG) $(EXPOBJS_UTILS) $(EXPOBJS_HARDWARE) $(EXPOBJS_CAMVIDEO) $(EXPOBJS_CAMIMPL)
	
	@echo "\n\n\nFinished archiving"
	@echo "--------------------------------------------------"
	@echo "Copying a copy of dash to parent directory."
	
	@echo "Call 'Make all' on HMDOpView directory to finish application build."
	cp hmdopview.a ../hmdopview.a
	
%.o: %.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAGS) `wx-config --cxxflags --libs --gl-libs std,aui` -c $< -o $@ -I../Vendored -I../CVGData/Src -I/usr/include/freetype2 -lGL $(OPENCVINCL) $(OPENCVLIBS) $(VEND_ALLINCLUDES)

objs: $(EXPOBJS_MAIN) $(EXPOBJS_STATES) $(EXPOBJS_LODEPNG) $(EXPOBJS_UTILS) $(EXPOBJS_HARDWARE) $(EXPOBJS_CAMVIDEO) $(EXPOBJS_CAMIMPL)
	@echo "TARGET objs"

.PHONY: clean
clean :
	@echo "COMPILING TARGET clean"
	find . -type f -name "*.o" -delete
	find . -type f -name "*.a" -delete
	rm ../hmdopview.a
	rm ../hmdopapp
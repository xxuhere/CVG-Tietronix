##################################################
#
#		GENERAL VARIABLES
#
##################################################

CC=g++
CFLAGS=-std=c++17 -Wall -lwiringPi

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
SUBDIR_SUBSTATES_HMDOP = States/HMDOpSubs
SUBDIR_LODEPNG = lodePNG
SUBDIR_UTILS = Utils
SUBDIR_HARDWARE = Hardware
SUBDIR_CAMVIDEO = CamVideo
SUBDIR_CAMIMPL = CamVideo/CamImpl
SUBDIR_UISYS = UISys
SUBDIR_CAROUSEL = Carousel

SUBOBJ_MAIN = \
	FontMgr GLWin HMDOpApp LoadAnim MainWin TexObj OpSession
	
SUBOBJ_STATES = \
	BaseState StateExit StateHMDOp StateInitCameras StateIntro

SUBOBJ_SUBSTATES_HMDOP = \
	HMDOpSub_Base HMDOpSub_Carousel HMDOpSub_Default HMDOpSub_InspNavForm HMDOpSub_MainMenuNav HMDOpSub_TempNavSliderListing HMDOpSub_WidgetCtrl

SUBOBJ_LODEPNG = \
	lodepng
	
SUBOBJ_UTILS = \
	CarouselData cvgCamFeedSource cvgCamTextureRegistry cvgGrabTimer cvgOptions cvgRect cvgShapes cvgStopwatch cvgStopwatchLeft multiplatform VideoPollType ProcessingType yen_threshold 
	
SUBOBJ_HARDWARE = \
	FauxMouse HardwareMgr IHardware LaserSys
	
SUBOBJ_CAMVIDEO = \
	CamStreamMgr IManagedCam ManagedCam ManagedComposite SnapRequest VideoRequest ROIRect
	
SUBOBJ_CAMIMPL = \
	ICamImpl CamImpl_StaticImg CamImpl_MMAL CamImpl_OpenCVBase CamImpl_OCV_Web CamImpl_OCV_USB CamImpl_OCV_HWPath
	
SUBOBJ_UISYS = \
	CacheRecordUtils DynSize NinePatcher UIBase UIButton UIColor4 UIGraphic UIHSlider UIPlate UIRect UISink UISys UIText UIVBulkSlider UIVec2
	
SUBOBJ_CAROUSEL = \
	Carousel
	

EXPOBJS_MAIN = $(patsubst %,$(SUBDIR_MAIN)/%.o,$(SUBOBJ_MAIN))
EXPOBJS_STATES = $(patsubst %,$(SUBDIR_STATES)/%.o,$(SUBOBJ_STATES))
EXPOBJS_SUBSTATES_HMDOP = $(patsubst %,$(SUBDIR_SUBSTATES_HMDOP)/%.o,$(SUBOBJ_SUBSTATES_HMDOP))
EXPOBJS_LODEPNG = $(patsubst %,$(SUBDIR_LODEPNG)/%.o,$(SUBOBJ_LODEPNG))
EXPOBJS_UTILS = $(patsubst %,$(SUBDIR_UTILS)/%.o,$(SUBOBJ_UTILS))
EXPOBJS_HARDWARE = $(patsubst %,$(SUBDIR_HARDWARE)/%.o,$(SUBOBJ_HARDWARE))
EXPOBJS_CAMVIDEO = $(patsubst %,$(SUBDIR_CAMVIDEO)/%.o,$(SUBOBJ_CAMVIDEO))
EXPOBJS_CAMIMPL = $(patsubst %,$(SUBDIR_CAMIMPL)/%.o,$(SUBOBJ_CAMIMPL))
EXPOBJS_UISYS = $(patsubst %,$(SUBDIR_UISYS)/%.o,$(SUBOBJ_UISYS))
EXPOBJS_CAROUSEL = $(patsubst %,$(SUBDIR_CAROUSEL)/%.o,$(SUBOBJ_CAROUSEL))

##################################################
#
#		TARGETS
#
##################################################

# Set to empty, or comment out, for building release version
DEBUGFLAGS = -g -ggdb

.PHONY: all multi
multi:
	$(MAKE) -j8 all
	
all: hmdopview
	@echo "COMPILING TARGET All"
	@echo "Building HmdViewOp application"
	@echo "--------------------------------------------------"
	$(CC) $(CFLAGS) $(DEBUGFLAGS) `wx-config --cxxflags` hmdopview.a -pthread -I/usr/include/openssl `wx-config --libs std,aui --gl-libs` -L/lib -lssl -lcrypto -lboost_system -lboost_filesystem -lstdc++fs -Wall $(OPENCVLIBS) -lmmal_core -lmmal_components -lmmal -lmmal_util -lvcos -lpthread -ldl -lbcm_host -lftgl -lpthread -ldl -o hmdopapp
	echo "Finished build command."
	
hmdopview: objs
	@echo "COMPILING TARGET archive"

	@echo "\n\n\nMaking HMDOpView archive"
	@echo "--------------------------------------------------"
	ar -rsc hmdopview.a $(EXPOBJS_CAROUSEL) $(EXPOBJS_MAIN) $(EXPOBJS_STATES) $(EXPOBJS_SUBSTATES_HMDOP) $(EXPOBJS_LODEPNG) $(EXPOBJS_UTILS) $(EXPOBJS_HARDWARE) $(EXPOBJS_CAMVIDEO) $(EXPOBJS_CAMIMPL) $(EXPOBJS_UISYS)
	
	@echo "\n\n\nFinished archiving"
	@echo "--------------------------------------------------"
	@echo "Copying a copy of dash to parent directory."
	
	@echo "Call 'Make all' on HMDOpView directory to finish application build."
	cp hmdopview.a ../hmdopview.a
	
moves:
	cp /opt/vc/lib/libmmal_core.so /lib/libmmal_core.so
	cp /opt/vc/lib/libmmal_components.so /lib/libmmal_components.so
	cp /opt/vc/lib/libmmal.so /lib/libmmal.so
	cp /opt/vc/lib/libmmal_util.so /lib/libmmal_util.so
	cp /opt/vc/lib/libvcos.so /lib/libvcos.so
	cp /opt/vc/lib/libbcm_host.so /lib/libbcm_host.so
	
%.o: %.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAGS) `wx-config --cxxflags --libs --gl-libs std,aui` -c $< -o $@ -I/opt/vc/include/ -I../Vendored -I../CVGData/Src -I/usr/include/freetype2 -lGL $(OPENCVINCL) $(VEND_ALLINCLUDES)

objs: ${EXPOBJS_CAROUSEL} $(EXPOBJS_MAIN) $(EXPOBJS_SUBSTATES_HMDOP) $(EXPOBJS_STATES) $(EXPOBJS_LODEPNG) $(EXPOBJS_UTILS) $(EXPOBJS_HARDWARE) $(EXPOBJS_CAMVIDEO) $(EXPOBJS_CAMIMPL) $(EXPOBJS_UISYS)
	@echo "TARGET objs"

.PHONY: clean
clean :
	@echo "COMPILING TARGET clean"
	find . -type f -name "*.o" -delete
	find . -type f -name "*.a" -delete
	rm -f ../hmdopview.a
	rm -f ../hmdopapp
	@echo "Finished cleaning precompiled files."

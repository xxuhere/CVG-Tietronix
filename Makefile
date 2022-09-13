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
VEND = src/Vendored

# Right now the path of OpenCV is hard coded with the 
# assumption of being on the Raspberry pi
OPENCVINCL		:= -I`python3 paths.py --ocv_inc`
OPENCVLIBS		:= -lopencv_core -lopencv_video -lopencv_imgproc -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs

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
SUBDIR_APPCOROUTINES = src/AppCoroutines
SUBDIR_CAMIMPL = src/CamVideo/CamImpl
SUBDIR_CAMVIDEO = src/CamVideo
SUBDIR_CAROUSEL = src/Carousel
SUBDIR_DICOMUTILS = src/DicomUtils
SUBDIR_HARDWARE = src/Hardware
SUBDIR_LODEPNG = src/Vendored/lodePNG
SUBDIR_MAIN = src
SUBDIR_STATES = src/States
SUBDIR_SUBSTATES_HMDOP = src/States/HMDOpSubs
SUBDIR_UISYS = src/UISys
SUBDIR_UTILS = src/Utils

SUBOBJ_APPCOROUTINES = \
	CoroutineSnapWithLasers

SUBOBJ_CAMIMPL = \
	ICamImpl CamImpl_StaticImg CamImpl_MMAL CamImpl_OpenCVBase CamImpl_OCV_Web CamImpl_OCV_USB CamImpl_OCV_HWPath
	
SUBOBJ_CAMVIDEO = \
	CamStreamMgr DicomImg_RawJpg IManagedCam ManagedCam ManagedComposite SnapRequest VideoRequest ROIRect	
	
SUBOBJ_CAROUSEL = \
	Carousel

SUBOBJ_DICOMUTILS = \
	DicomInjector DicomInjectorSet DicomMiscUtils DVRPersonName
	
SUBOBJ_HARDWARE = \
	FauxMouse HardwareMgr IHardware LaserSys
	
SUBOBJ_LODEPNG = \
	lodepng
	
SUBOBJ_MAIN = \
	AppVersionDicom FontMgr GLWin HMDOpApp LoadAnim MainWin Session_Toml TexObj OpSession
	
SUBOBJ_STATES = \
	BaseState MousepadUI StateExit StateHMDOp StateInitCameras StateIntro

SUBOBJ_SUBSTATES_HMDOP = \
	HMDOpSub_Base HMDOpSub_Carousel HMDOpSub_Default HMDOpSub_InspNavForm HMDOpSub_MainMenuNav HMDOpSub_TempNavSliderListing HMDOpSub_WidgetCtrl
	
SUBOBJ_UTILS = \
	CarouselData cvgCamFeedSource cvgCamTextureRegistry cvgCoroutine cvgGrabTimer cvgOptions cvgRect cvgShapes cvgStopwatch cvgStopwatchLeft multiplatform VideoPollType ProcessingType TimeUtils yen_threshold 
	
SUBOBJ_UISYS = \
	CacheRecordUtils DynSize NinePatcher UIBase UIButton UIColor4 UIGraphic UIHSlider UIPlate UIRect UISink UISys UIText UIVBulkSlider UIVec2

EXPOBJS_APPCOROUTINES = $(patsubst %,$(SUBDIR_APPCOROUTINES)/%.o,$(SUBOBJ_APPCOROUTINES))	
EXPOBJS_CAMIMPL = $(patsubst %,$(SUBDIR_CAMIMPL)/%.o,$(SUBOBJ_CAMIMPL))	
EXPOBJS_CAMVIDEO = $(patsubst %,$(SUBDIR_CAMVIDEO)/%.o,$(SUBOBJ_CAMVIDEO))
EXPOBJS_CAROUSEL = $(patsubst %,$(SUBDIR_CAROUSEL)/%.o,$(SUBOBJ_CAROUSEL))
EXPOBJS_DICOMUTILS = $(patsubst %,$(SUBDIR_DICOMUTILS)/%.o,$(SUBOBJ_DICOMUTILS))
EXPOBJS_HARDWARE = $(patsubst %,$(SUBDIR_HARDWARE)/%.o,$(SUBOBJ_HARDWARE))
EXPOBJS_LODEPNG = $(patsubst %,$(SUBDIR_LODEPNG)/%.o,$(SUBOBJ_LODEPNG))
EXPOBJS_MAIN = $(patsubst %,$(SUBDIR_MAIN)/%.o,$(SUBOBJ_MAIN))
EXPOBJS_STATES = $(patsubst %,$(SUBDIR_STATES)/%.o,$(SUBOBJ_STATES))
EXPOBJS_SUBSTATES_HMDOP = $(patsubst %,$(SUBDIR_SUBSTATES_HMDOP)/%.o,$(SUBOBJ_SUBSTATES_HMDOP))
EXPOBJS_UISYS = $(patsubst %,$(SUBDIR_UISYS)/%.o,$(SUBOBJ_UISYS))
EXPOBJS_UTILS = $(patsubst %,$(SUBDIR_UTILS)/%.o,$(SUBOBJ_UTILS))

# --start-group and --end-group are used to solve the order the libraries are included,
# because after some testing, I have no idea what order these are supposed to go in
# without invoking linker errors without treating them as cyclic dependencies.
# (wleu 07/28/2022)
DCMLIBS = -Wl,--start-group -ldl -ldcmdata -lofstd -li2d -loflog -licuuc -Wl,--end-group

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
	$(CC) $(CFLAGS) $(DEBUGFLAGS) `wx-config --cxxflags` hmdopview.a -pthread -I/usr/include/openssl `wx-config --libs std,aui --gl-libs` -L/lib -lssl -lcrypto -lboost_system -lboost_filesystem -lstdc++fs -Wall $(OPENCVLIBS) -lmmal_core -lmmal_components -lmmal -lmmal_util -lvcos -lpthread -ldl -lbcm_host -lftgl -lpthread -lxml2 $(DCMLIBS) -o hmdopapp
	echo "Finished build command."
	
hmdopview: objs
	@echo "COMPILING TARGET archive"

	@echo "\n\n\nMaking HMDOpView archive"
	@echo "--------------------------------------------------"
	ar -rsc hmdopview.a $(EXPOBJS_APPCOROUTINES) $(EXPOBJS_CAROUSEL) $(EXPOBJS_DICOMUTILS) $(EXPOBJS_MAIN) $(EXPOBJS_STATES) $(EXPOBJS_SUBSTATES_HMDOP) $(EXPOBJS_LODEPNG) $(EXPOBJS_UTILS) $(EXPOBJS_HARDWARE) $(EXPOBJS_CAMVIDEO) $(EXPOBJS_CAMIMPL) $(EXPOBJS_UISYS)
	
	@echo "\n\n\nFinished archiving"
	@echo "--------------------------------------------------"
	@echo "Copying a copy of dash to parent directory."
	
	@echo "Call 'Make all' on HMDOpView directory to finish application build."
	
moves:
	cp /opt/vc/lib/libmmal_core.so /lib/libmmal_core.so
	cp /opt/vc/lib/libmmal_components.so /lib/libmmal_components.so
	cp /opt/vc/lib/libmmal.so /lib/libmmal.so
	cp /opt/vc/lib/libmmal_util.so /lib/libmmal_util.so
	cp /opt/vc/lib/libvcos.so /lib/libvcos.so
	cp /opt/vc/lib/libbcm_host.so /lib/libbcm_host.so
	
%.o: %.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAGS) `wx-config --cxxflags` -c $< -o $@ -I/opt/vc/include/ -I$(VEND) -I../CVGData/Src -I/usr/include/freetype2 -lGL $(OPENCVINCL)

objs: ${EXPOBJS_CAROUSEL} $(EXPOBJS_APPCOROUTINES) $(EXPOBJS_MAIN) $(EXPOBJS_DICOMUTILS) $(EXPOBJS_SUBSTATES_HMDOP) $(EXPOBJS_STATES) $(EXPOBJS_LODEPNG) $(EXPOBJS_UTILS) $(EXPOBJS_HARDWARE) $(EXPOBJS_CAMVIDEO) $(EXPOBJS_CAMIMPL) $(EXPOBJS_UISYS)
	@echo "TARGET objs"

.PHONY: clean
clean :
	@echo "COMPILING TARGET clean"
	find . -type f -name "*.o" -delete
	find . -type f -name "*.a" -delete
	rm -f ./hmdopview.a
	rm -f ./hmdopapp
	@echo "Finished cleaning precompiled files."

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
VEND = ../Vendored
VEND_WS = ../Vendored/Simple-WebSocket-Server
VEND_HTTP = ../Vendored/Simple-Web-Server
VEND_JS = ../Vendored/

# Right now the path of OpenCV is hard coded with the 
# assumption of being on the Raspberry pi
OPENCVINCL		:= -I`python3 ../paths.py --ocv_inc`
OPENCVLIBS		:= -lopencv_core -lopencv_video -lopencv_imgproc -lopencv_videoio -lopencv_highgui

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

SUBOBJ_MAIN = \
	CamStreamMgr FontMgr GLWin HMDOpApp MainWin TexObj
	
SUBOBJ_STATES = \
	BaseState StateHMDOp StateInitCameras StateIntro

EXPOBJS_MAIN = $(patsubst %,$(SUBDIR_MAIN)/%.o,$(SUBOBJ_MAIN))
EXPOBJS_STATES = $(patsubst %,$(SUBDIR_STATES)/%.o,$(SUBOBJ_STATES))

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
	$(CC) $(CFLAGS) $(DEBUGFLAGS) `wx-config --cxxflags` hmdopview.a -pthread -I/usr/include/openssl -L/usr/local/aarch64-linux-gnu -lssl -lcrypto -lboost_system -Wall `wx-config --libs std,aui --gl-libs` $(OPENCVLIBS) -lftgl -lGL -o hmdopapp
	echo "Finished build command."
	
hmdopview: objs
	@echo "COMPILING TARGET archive"

	@echo "\n\n\nMaking dashboard archive"
	@echo "--------------------------------------------------"
	ar -rsc hmdopview.a $(EXPOBJS_MAIN) $(EXPOBJS_STATES)
	
	@echo "\n\n\nFinished archiving"
	@echo "--------------------------------------------------"
	@echo "Copying a copy of dash to parent directory."
	
	@echo "Call 'Make dash' on parent directory to finish application build with CVG Data library."
	cp hmdopview.a ../hmdopview.a
	
%.o: %.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAGS) `wx-config --cxxflags --libs --gl-libs std,aui` -c $< -o $@ -I../Vendored -I../CVGData/Src -I/usr/include/freetype2 -lGL $(OPENCVINCL) $(OPENCVLIBS) $(VEND_ALLINCLUDES)

objs: $(EXPOBJS_MAIN) $(EXPOBJS_STATES)
	@echo "TARGET objs"

.PHONY: clean
clean :
	@echo "COMPILING TARGET clean"
	find . -type f -name "*.o" -delete
	find . -type f -name "*.a" -delete
	rm ../hmdopview.a
	rm ../hmdopapp
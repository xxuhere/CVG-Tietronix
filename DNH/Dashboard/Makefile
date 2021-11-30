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
SUBDIR_INSPECTOR = Inspector
SUBDIR_DASHELEMENTS = DashElements
SUBDIR_DIALOGS = Dialogs


SUBOBJ_MAIN = App DashboardElement DashboardElementInst DashboardGrid DashboardGridInst DockedCVGPane FullscreenDash PaneBusLog PaneDashboard PaneInspector RootWindow
SUBOBJ_INSPECTOR = InspectorEquipment InspectorParam InsWidgetBool InsWidgetButton InsWidgetEnum InsWidgetFloat InsWidgetInt InsWidgetParam InsWidgetString
SUBOBJ_DASHELEMENTS = DashEleButton DashEleCheckbox DashEleFloat DashEleFloatSlider DashEleInt DashElePulldown DashEleSingleString IDashEle
SUBOBJ_DIALOGS = DlgGridSize

EXPOBJS_MAIN = $(patsubst %,$(SUBDIR_MAIN)/%.o,$(SUBOBJ_MAIN))
EXPOBJS_INSPECTOR = $(patsubst %,$(SUBDIR_INSPECTOR)/%.o,$(SUBOBJ_INSPECTOR))
EXPOBJS_DASHELEMENTS = $(patsubst %,$(SUBDIR_DASHELEMENTS)/%.o,$(SUBOBJ_DASHELEMENTS))
EXPOBJS_DIALOGS = $(patsubst %,$(SUBDIR_DIALOGS)/%.o,$(SUBOBJ_DIALOGS))

all: objs
	@echo "COMPILING TARGET All"
	
	@echo "\n\n\nMaking dashboard archive"
	@echo "--------------------------------------------------"
	ar -rsc dash.a $(EXPOBJS_MAIN) $(EXPOBJS_INSPECTOR) $(EXPOBJS_DASHELEMENTS) $(EXPOBJS_DIALOGS)
	
	@echo "\n\n\nFinished archiving"
	@echo "--------------------------------------------------"
	@echo "Copying a copy of dash to parent directory."
	
	@echo "Call 'Make dash' on parent directory to finish application build with CVG Data library."
	cp dash.a ../dash.a
	

%.o: %.cpp
	$(CC) $(CFLAGS) `wx-config --cxxflags --libs std,aui` -c $< -o $@ -I../Vendored -I../CVGData/Src $(VEND_ALLINCLUDES)

objs: $(EXPOBJS_MAIN) $(EXPOBJS_INSPECTOR) $(EXPOBJS_DASHELEMENTS) $(EXPOBJS_DIALOGS)
	@echo "TARGET objs"

.PHONY: clean
clean :
	@echo "COMPILING TARGET clean"
	find . -type f -name "*.o" -delete
	find . -type f -name "*.a" -delete

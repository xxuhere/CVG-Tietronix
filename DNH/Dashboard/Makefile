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

SUBOBJ_MAIN = App DashboardGrid DashboardElement PaneBusLog PaneDashboard PaneInspector RootWindow
SUBOBJ_INSPECTOR = InspectorEquipment InspectorParam InsWidgetBool InsWidgetEnum InsWidgetFloat InsWidgetInt InsWidgetParam InsWidgetString

EXPOBJS_MAIN = $(patsubst %,$(SUBDIR_MAIN)/%.o,$(SUBOBJ_MAIN))
EXPOBJS_INSPECTOR = $(patsubst %,$(SUBDIR_INSPECTOR)/%.o,$(SUBOBJ_INSPECTOR))

all: objs
	echo "COMPILING TARGET All"
	
	echo "Making dashboard archive"
	ar -rsc dash.a $(EXPOBJS_MAIN) $(EXPOBJS_INSPECTOR)
	
	echo "Copying a copy of dash to parent directory."
	
	echo "Call 'Make dash' on parent directory to finish application build with CVG Data library."
	cp dash.a ../dash.a
	

%.o: %.cpp
	echo $(CC) $(CFLAGS) `wx-config --cxxflags --libs std,aui` -c $< -o $@ -I../Vendored $(VEND_ALLINCLUDES)
	$(CC) $(CFLAGS) `wx-config --cxxflags --libs std,aui` -c $< -o $@ -I../Vendored -I../CVGData/Src $(VEND_ALLINCLUDES)

objs: $(patsubst %,$(SUBDIR_MAIN)/%.o,$(SUBOBJ_MAIN)) $(patsubst %,$(SUBDIR_INSPECTOR)/%.o,$(SUBOBJ_INSPECTOR))
	echo "TARGET objs"

.PHONY: clean
clean :
	echo "COMPILING TARGET clean"
	rm $(SUBDIR_MAIN)/*.o
	rm $(SUBDIR_INSPECTOR)/*.o
	rm ./dash.a
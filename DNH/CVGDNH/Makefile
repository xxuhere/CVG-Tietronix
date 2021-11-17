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
ALLHEADERS_HTTP = $(VEND_HTTP)/asio_compatibility.hpp $(VEND_HTTP)/client_http $(VEND_HTTP)/client_https $(VEND_HTTP)/crypto.hpp $(VEND_HTTP)/mutex.hpp $(VEND_HTTP)/server_http.hpp $(VEND_HTTP)/server_https $(VEND_HTTP)/status_code.hpp $(VEND_HTTP)/utility.hpp
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

SUBDIR_MAIN = Src
SUBDIR_CORE = Src/Core

SUBOBJ_MAIN = mainWin
SUBOBJ_CORE = BackboneHTTP BackboneWS CoreDNH SEquipment SEquipmentList LogSys ResponseUtils CommandsCollection

EXPOBJS_MAIN = $(patsubst %,$(SUBDIR_MAIN)/%.o,$(SUBOBJ_MAIN))
EXPOBJS_CORE = $(patsubst %,$(SUBDIR_CORE)/%.o,$(SUBOBJ_CORE))

all : Src/mainWin.o Core
	echo "COMPILING TARGET All"
	ar -rsc app.a Src/mainWin.o $(EXPOBJS_CORE)
	
	# The DNH project Makefile (one directory up) also expects
	# a copy of this link library in its directory.
	cp app.a ../app.a
	
%.o: %.cpp
	echo "COMPILING TARGET %.o"
	echo $(CC) $(CFLAGS) -c $< -o $@ $(VEND_ALLINCLUDES) -I..
	$(CC) $(CFLAGS) -c $< -o $@ $(VEND_ALLINCLUDES) -I..
	
Core: $(patsubst %,$(SUBDIR_CORE)/%.o,$(SUBOBJ_CORE))
	
.PHONY: clean
clean :
	echo "COMPILING TARGET clean"
	rm $(SUBDIR_MAIN)/*.o
	rm $(SUBDIR_CORE)/*.o
	rm ./app.a
	
	
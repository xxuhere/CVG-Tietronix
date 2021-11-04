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
VEND_WS = ../Vendored_Libs/Simple-WebSocket-Server
VEND_HTTP = ../Vendored_Libs/Simple-Web-Server
VEND_JS = ../Vendored_Libs/json

VEND_ALLINCLUDES = -I$(VEND_WS) -I$(VEND_HTTP) -I$(VEND_JS)

# All vendored header files, for listing dependencies
# While these dependencies are not expected to be modified,
# we list them as dependencies to check as a formality.
ALLHEADERS_WS = $(VEND_WS)/crypto.hpp $(VEND_WS)/server_ws.hpp $(VEND_WS)/server_wss.hpp $(VEND_WS)/status_code.hpp $(VEND_WS)/utility.hpp 
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
SUBDIR_CORE = Core
SUBDIR_PARAMS = Core/Params
SUBDIR_TYPES = Core/Types
SUBDIR_UTILSCLASSES = Core/UtilsClasses

SUBOBJ_MAIN = mainWin
SUBOBJ_CORE = BackboneHTTP BackboneWS CoreDNH Equipment LogSys ParseUtils ResponseUtils
SUBOBJ_PARAMS = Param ParamBool ParamEnum ParamFloat ParamInt ParamString ParamUtils
SUBOBJ_TYPES = DataType EqType ErrorTy
SUBOBJ_UTILSCLASSES = CommandsCollection ParamCache

# Expand the obj files of each dir. They're put into 
# variables before used in the compiler command or else
# it's unweildy to read.
EXPOBJS_PARAMS = $(patsubst %,$(SUBDIR_PARAMS)/%.o,$(SUBOBJ_PARAMS))
EXPOBJS_CORE = $(patsubst %,$(SUBDIR_CORE)/%.o,$(SUBOBJ_CORE))
EXPOBJS_TYPES = $(patsubst %,$(SUBDIR_TYPES)/%.o,$(SUBOBJ_TYPES))
EXPOBJS_UTILSCLASSES = $(patsubst %,$(SUBDIR_UTILSCLASSES)/%.o,$(SUBOBJ_UTILSCLASSES))

##################################################
#
#		COMPILE RULES
#
##################################################
# Reminders:
# https://stackoverflow.com/a/3220288/2680066
# When using wildcards,
# 	$@ is the name of the target being generated.
#	$< is the first prequisite
#	$^ is all the prerequisites

all : mainWin.o Params Types Core UtilsClasses
	echo "COMPILING TARGET All"
	$(CC) $(CFLAGS) mainWin.o $(EXPOBJS_PARAMS) $(EXPOBJS_CORE) $(EXPOBJS_TYPES) $(EXPOBJS_UTILSCLASSES) -pthread -I/usr/include/openssl -L/usr/local/aarch64-linux-gnu -lssl -lcrypto -lboost_system -Wall -o DNH
	
%.o: %.cpp $(ALLHEADERS_WS)
	echo "COMPILING TARGET %.o"
	echo $(CC) -c $< -o $@ $(VEND_ALLINCLUDES) $(CFLAGS)
	$(CC) $(CFLAGS) -c $< -o $@ $(VEND_ALLINCLUDES)
	
Core: $(patsubst %,$(SUBDIR_CORE)/%.o,$(SUBOBJ_CORE)) $(ALLHEADERS_WS)
	echo "COMPILING TARGET Core"
	echo $(CC) -c $< -o $@ $(VEND_ALLINCLUDES) $(CFLAGS)
	$(CC) $(CFLAGS) -c $< -o $@ $(VEND_ALLINCLUDES)
	
Params: $(patsubst %,$(SUBDIR_PARAMS)/%.o,$(SUBOBJ_PARAMS)) $(ALLHEADERS_WS)
	echo "COMPILING TARGET Params"
	echo $(CC) -c $< -o $@ $(VEND_ALLINCLUDES) $(CFLAGS)
	$(CC) $(CFLAGS) -c $< -o $@ $(VEND_ALLINCLUDES)
	
Types: $(patsubst %,$(SUBDIR_TYPES)/%.o,$(SUBOBJ_TYPES)) $(ALLHEADERS_WS)
	echo "COMPILING TARGET Types"
	echo $@
	echo $(CC) -c $< -o $@ $(VEND_ALLINCLUDES) $(CFLAGS)
	$(CC) $(CFLAGS) -c $< -o $@ $(VEND_ALLINCLUDES)
	
UtilsClasses: $(patsubst %,$(SUBDIR_UTILSCLASSES)/%.o,$(SUBOBJ_UTILSCLASSES)) $(ALLHEADERS_WS)
	echo "COMPILING TARGET Types"
	echo $@
	echo $(CC) -c $< -o $@ $(VEND_ALLINCLUDES) $(CFLAGS)
	$(CC) $(CFLAGS) -c $< -o $@ $(VEND_ALLINCLUDES)
	
.PHONY: clean
	echo "COMPILING TARGET .PHONY"
	
clean :
	echo "COMPILING TARGET clean"
	rm $(SUBDIR_MAIN)/*.o
	rm $(SUBDIR_CORE)/*.o
	rm $(SUBDIR_PARAMS)/*.o
	rm $(SUBDIR_TYPES)/*.o
	rm $(SUBDIR_UTILCLASSES)/*.o
	rm ./DNH
	
	
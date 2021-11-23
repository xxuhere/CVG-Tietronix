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
VEND = ../Vendored

VEND_ALLINCLUDES = -I$(VEND)

ALLHEADERS_JSON = $(VEND)/nlohmann/json.hpp

ALLHEADERS_ALLVEND = $(ALLHEADERS_JSON)

##################################################
#
#		PROGRAM OBJECTS
#
##################################################

SUBDIR_MAIN = Src
SUBDIR_PARAMS = Src/Params
SUBDIR_TYPES = Src/Types
SUBDIR_UTILSCLASSES = Src/UtilsClasses

SUBOBJ_MAIN = Equipment ParseUtils
SUBOBJ_PARAMS = Param ParamBool ParamEnum ParamFloat ParamInt ParamString ParamUtils
SUBOBJ_TYPES = DataType EqType ErrorTy
SUBOBJ_UTILSCLASSES = ParamCache

EXPOBJS_PARAMS = $(patsubst %,$(SUBDIR_PARAMS)/%.o,$(SUBOBJ_PARAMS))
EXPOBJS_MAIN = $(patsubst %,$(SUBDIR_MAIN)/%.o,$(SUBOBJ_MAIN))
EXPOBJS_TYPES = $(patsubst %,$(SUBDIR_TYPES)/%.o,$(SUBOBJ_TYPES))
EXPOBJS_UTILSCLASSES = $(patsubst %,$(SUBDIR_UTILSCLASSES)/%.o,$(SUBOBJ_UTILSCLASSES))


##################################################
#
#		COMPILE RULES
#
##################################################

all : Params Types UtilsClasses Src/Equipment.o Src/ParseUtils.o
	echo "COMPILING TARGET All"
	ar -rsc data.a $(EXPOBJS_PARAMS) $(EXPOBJS_MAIN) $(EXPOBJS_TYPES) $(EXPOBJS_UTILSCLASSES) Src/ParseUtils.o Src/Equipment.o
	
	# The DNH project Makefile (one directory up) also expects
	# a copy of this link library in its directory.
	cp data.a ../data.a
	
%.o: %.cpp
	echo "COMPILING TARGET %.o"
	echo $(CC) -c $< -o $@ $(VEND_ALLINCLUDES) $(CFLAGS)
	$(CC) $(CFLAGS) -c $< -o $@ $(VEND_ALLINCLUDES)
	
Params: $(patsubst %,$(SUBDIR_PARAMS)/%.o,$(SUBOBJ_PARAMS))
	echo "COMPILING TARGET Params"
	echo $(CC) -c $< -o $@ $(VEND_ALLINCLUDES) $(CFLAGS)
	$(CC) $(CFLAGS) -c $< -o $@ $(VEND_ALLINCLUDES)
	
Types: $(patsubst %,$(SUBDIR_TYPES)/%.o,$(SUBOBJ_TYPES))
	echo "COMPILING TARGET Types"
	echo $@
	echo $(CC) -c $< -o $@ $(VEND_ALLINCLUDES) $(CFLAGS)
	$(CC) $(CFLAGS) -c $< -o $@ $(VEND_ALLINCLUDES)
	
UtilsClasses: $(patsubst %,$(SUBDIR_UTILSCLASSES)/%.o,$(SUBOBJ_UTILSCLASSES))
	echo "COMPILING TARGET Types"
	echo $@
	echo $(CC) -c $< -o $@ $(VEND_ALLINCLUDES) $(CFLAGS)
	$(CC) $(CFLAGS) -c $< -o $@ $(VEND_ALLINCLUDES)
	
.PHONY: clean
	echo "COMPILING TARGET .PHONY"
	
clean :
	echo "COMPILING TARGET clean"
	find . -type f -name "*.o" -delete
	find . -type f -name "*.a" -delete

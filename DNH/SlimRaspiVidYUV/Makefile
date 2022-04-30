CC=gcc
CPP=g++
CFLAGS=-Wall

all : slim
	# Make both the slimvid and slim
	@echo ""
	@echo "Compiling All" 
	
slim : 
	# Make the active ingredient test of RaspiVidYUV - this will
	# be the most slimmed down version we can create while still getting
	# the core functionality required, even if we have to do more than
	# strip RaspiVidYUV as a starting template.
	@echo ""
	@echo "Compiling test application." 
	@printf "\n\n" 
	$(CPP) $(CFLAGS) -o slim Slim.cpp -I/opt/vc/include/ -L/opt/vc/lib/ -lmmal_core -lmmal_components -lmmal -lmmal_util -lvcos -lpthread -ldl -lbcm_host
	
.PHONY: clean
clean :
	@echo "\n\n"
	@echo "COMPILING TARGET clean"
	@echo "\n\n\nCleaning build files"
	@echo "--------------------------------------------------"
	rm slim
	find . -type f -name "*.o" -delete
	find . -type f -name "*.a" -delete
	
CC=gcc
CFLAGS=-Wall

all : 
	@echo ""
	@echo "Compiling test application." 
	@printf "\n\n" 
	$(CC) $(CFLAGS) -o SlimYUVTest RaspiVidYUV.c RaspiCommonSettings.c RaspiCamControl.c RaspiCLI.c RaspiHelpers.c RaspiPreview.c -I/opt/vc/include/ -L/opt/vc/lib/ -lmmal_core -lmmal_components -lmmal -lmmal_util -lvcos -lpthread -ldl -lbcm_host
	
.PHONY: clean
clean :
	@echo "\n\n"
	@echo "COMPILING TARGET clean"
	@echo "\n\n\nCleaning build files"
	@echo "--------------------------------------------------"
	find . -type f -name "*.o" -delete
	find . -type f -name "*.a" -delete
	
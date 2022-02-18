

all :
	$(MAKE) -C OpenCV_Simple
	$(MAKE) -C wxCVGL_RTSPComposite
	$(MAKE) -C wxMultiCamOpenCVGL
	$(MAKE) -C wxMultiCamOpenCVGL_Stream
	
.PHONY: clean
clean :
	@echo "\n\n"
	@echo "COMPILING TARGET clean"
	@echo "\n\n\nCleaning build files"
	@echo "--------------------------------------------------"
	find . -type f -name "*.o" -delete
	find . -type f -name "*.a" -delete
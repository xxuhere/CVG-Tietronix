##################################################
#
#		GENERAL VARIABLES
#
##################################################

CC=g++
CFLAGS=-Wall

OPENCVLIBS := -lopencv_core -lopencv_video -lopencv_imgproc -lopencv_videoio -lopencv_highgui

##################################################
#
#		COMPILE RULES
#
##################################################

all : hub dash

hub : data.a app.a
	@echo "COMPILING TARGET hub"
	@echo "Building DNH application"
	@echo "--------------------------------------------------"
	$(CC) $(CFLAGS) app.a data.a -pthread -I/usr/include/openssl -L/usr/local/aarch64-linux-gnu -lssl -lcrypto -lboost_system -Wall -o DNH
	echo "Finished build command."
	
dash : data.a dash.a
	@echo "\n\n"
	@echo "COMPILING TARGET dash"
	@echo "Building Dashboard application"
	@echo "--------------------------------------------------"
	$(CC) $(CFLAGS) `wx-config --cxxflags` dash.a data.a -pthread -I/usr/include/openssl -L/usr/local/aarch64-linux-gnu -lssl -lcrypto -lboost_system -Wall `wx-config --libs std,aui --gl-libs` $(OPENCVLIBS) -lGL -o DashboardApp
	@echo "Finished build command."
	
app.a : 
	@echo "COMPILING TARGET app.a"
	# Make all for CVGDNH to make the static link 
	# object for the server application.
	cd CVGDNH && make all

data.a : 
	@echo "COMPILING TARGET data.a"
	# Make all for the CVGData to make the static link 
	# objects for the basic datastructure and utils.
	cd CVGData && make all
	
dash.a :
	cd Dashboard && make all

.PHONY: clean
clean :
	@echo "\n\n"
	@echo "COMPILING TARGET clean"
	@echo "\n\n\nCleaning build files"
	@echo "--------------------------------------------------"
	find . -type f -name "*.o" -delete
	find . -type f -name "*.a" -delete
	rm -f DNH
	rm -f DashboardApp
	# Just in case the subprojects have extra clean logic, 
	# run them too.
	
	@echo "\n\n\nRunning subproject cleans"
	@echo "--------------------------------------------------"
	cd CVGDNH && make clean
	cd CVGData && make clean
	cd Dashboard && make clean

##################################################
#
#		GENERAL VARIABLES
#
##################################################

CC=g++
CFLAGS=-Wall

##################################################
#
#		COMPILE RULES
#
##################################################

all : hub dash

hub : data.a app.a
	echo "Building DNH application"
	$(CC) $(CFLAGS) app.a data.a -pthread -I/usr/include/openssl -L/usr/local/aarch64-linux-gnu -lssl -lcrypto -lboost_system -Wall -o DNH
	echo "Finished build command."
	
dash : data.a dash.a
	echo "Building Dashboard application"
	echo $(CC) $(CFLAGS) `wx-config --cxxflags` dash.a data.a -pthread -I/usr/include/openssl -L/usr/local/aarch64-linux-gnu -lssl -lcrypto -lboost_system -Wall `wx-config --libs std,aui` -o DashboardApp
	$(CC) $(CFLAGS) `wx-config --cxxflags` dash.a data.a -pthread -I/usr/include/openssl -L/usr/local/aarch64-linux-gnu -lssl -lcrypto -lboost_system -Wall `wx-config --libs std,aui` -o DashboardApp
	echo "Finished build command."
	
app.a : 
	# Make all for CVGDNH to make the static link 
	# object for the server application.
	cd CVGDNH && make all

data.a : 
	# Make all for the CVGData to make the static link 
	# objects for the basic datastructure and utils.
	cd CVGData && make all
	
dash.a :
	cd Dashboard && make all

.PHONY: clean
clean :
	rm *.a
	rm DNH
	rm DashboardApp
	cd CVGDNH && make clean
	cd CVGData && make clean
	cd Dashboard && make clean
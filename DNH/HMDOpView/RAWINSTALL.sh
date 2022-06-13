##################################################
#
#	RAW INSTALLER
#
# This script attempts to setup the entire HMDOpView application
# on a new installation of RaspberryPi with as little
# human intervention as possible.
#
# The entire process is expected to take a few hours, with
# the compilation of wxWidgets taking the longest part.
#
# THIS SCRIPT IS ONLY OCCASIONALLY UPDATED, AS TESTING IT
# REQUIRES RECREATING A BLANK RPI IMAGE AND LETTING IT
# INSTALL FROM SCRATCH - WHICH IS A TIME CONSUMING PROCESS.
#
##################################################

# The commit that was last tested to successfully rebuild from.
SUPPORTED_COMMIT=bfe140668919684bffeb798a9b496e07d2abdaf9

# MAINTENENCE, INIT MACHINE
sudo apt update

# OPTIONAL, ENABLE SSH
sudo apt install openssh-server
sudo systemctl status ssh
sudo apt-get install ufw
sudo ufw allow ssh

##################################################
#
#	PULL DEPENDENCIES FROM APT
#
##################################################
sudo apt-get install libssl-dev
sudo apt-get install libopencv-dev
sudo apt-get install mesa-common-dev freeglut3-dev
sudo apt-get install libboost1.67-all
sudo apt-get install g++
sudo apt-get install make
sudo apt-get install libftgl-dev
sudo apt-get install libfreetype6-dev
sudo apt-get install cmake

##################################################
#
#	WXWIDGETS
#
##################################################

# Install 7-zip to extract downloaded snapshot
sudo apt-get install p7zip-full
sudo apt-get install libgtk-3-dev build-essential 

if [ -z $(command -v wx-config) ]
then

	# Make the wxWidgets directory if it doesn't exist.
	if [ ! -d wxWidgets-3.1.5.7 ]
	then 
		mkdir wxWidgets-3.1.5.7
	fi

	# Get and extract wxwidgets if needed
	pushd wxWidgets-3.1.5.7		# Enter new directory

	if [ ! -d wxWidgets-3.1.5.7z ]
	then
		# Download 7zip of specific version
		wget https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.5/wxWidgets-3.1.5.7z
		# unzip the 7zip file we just downloaded
		7z x wxWidgets-3.1.5.7z
	fi

	# Build and install wxWidgets if stuff if missing from an install
	if [ ! -d gtk-build ]
	then 
		mkdir gtk-build
	fi

	pushd gtk-build
	../configure --with-opengl --disable-shared --enable-unicode
	make
	sudo make install
	popd #Pop gtk-build

	popd # Pop wxWidgets-3.1.5.7
fi
##################################################
#
#	PULL AND BUILD MMAL
#
##################################################

if [ ! -f/opt/vc/lib/libmmal.so ]
then
	if [ ! -d userland ]
	then 
		git clone https://github.com/raspberrypi/userland.git
	fi

	pushd userland
	sudo ./buildme
	popd # Pop userland
fi

##################################################
#
#	PULL AND BUILD CVG REPO AND HMDOpView
#
##################################################
if [ ! -d cancer-goggles ]
then
	git clone https://github.com/Achilefu-Lab/cancer-goggles.git
fi

pushd cancer-goggles
git fetch
git checkout Tie $SUPPORTED_COMMIT

cd DNH/HMDOpView
if [ ! -f /lib/libmmal.so ]
then
	sudo make moves
fi
make clean
make all

popd


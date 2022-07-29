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
SUPPORTED_COMMIT=84b79af9900a94d9dd8a706faeeae24b8424e3f2

sudo apt update

# Make sure all apt-get install commands have a `-y` to 
# automatically accept the "yes" option and avoid the 
# installer needing to respond to queries.

# OPTIONAL, ENABLE SSH
sudo apt-get -y install openssh-server
sudo systemctl status ssh
sudo apt-get -y install ufw
sudo ufw allow ssh

##################################################
#
#	PULL DEPENDENCIES FROM APT
#
##################################################
sudo apt-get -y install libssl-dev
sudo apt-get -y install libopencv-dev
sudo apt-get -y install mesa-common-dev freeglut3-dev
sudo apt-get -y install libboost1.67-all
sudo apt-get -y install g++
sudo apt-get -y install make
sudo apt-get -y install libftgl-dev
sudo apt-get -y install libfreetype6-dev
sudo apt-get -y install cmake

##################################################
#
#	WXWIDGETS
#
##################################################

# Install 7-zip to extract downloaded snapshot
sudo apt-get -y install p7zip-full
sudo apt-get -y install libgtk-3-dev build-essential 

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
git checkout $SUPPORTED_COMMIT

cd DNH/HMDOpView
if [ ! -f /lib/libmmal.so ]
then
	sudo make moves
fi
make clean
make all

popd


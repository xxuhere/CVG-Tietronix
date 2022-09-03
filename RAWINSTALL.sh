#!/usr/bin/env bash

# To download this file via wget, use 
# wget https://raw.githubusercontent.com/Achilefu-Lab/CVG-Tietronix/main/RAWINSTALL.sh
#
# This can be used to get the latest file from the repository without cloning
# the repository. You should not manually clone the repository when using
# this script, because the script will do that for you at the right time.

##################################################
#
#	RAW INSTALLER
#
# This script attempts to setup the entire HMDOpView application
# on a new installation of RaspberryPi with as little
# human intervention as possible.
#
# The entire process is expected to take a few hours, with
# the compilation of wxWidgets taking the longest part. The 
# compilation of DCMTK is also expected to take some time.
#
# Note that wherever you run this is where the application repo,
# as well as the dependency repos, will be installed.
#
# THIS SCRIPT IS ONLY OCCASIONALLY UPDATED, AS TESTING IT
# REQUIRES RECREATING A BLANK RPI IMAGE AND LETTING IT
# INSTALL FROM SCRATCH - WHICH IS A TIME CONSUMING PROCESS.
#
##################################################

##################################################
#
#	VARIABLES
#
##################################################

# The Repo project to pull from
PROJECT_REPO=https://github.com/Achilefu-Lab/CVG-Tietronix.git
# The directory created when cloning the repo. Should be the Git 
# path destination without the .git.
PROJECT_DIR=CVG-Tietronix

# The commit that was last tested to successfully rebuild from.
SUPPORTED_COMMIT=a4686d17b0ef804e0a0f6cf3b10802d4fb5bbacc

# The version of DCMTK to pull
DCMTK_TAG=DCMTK-3.6.7 
# The text expected inside of the dcmtk's VERSION file. Should be
# the semantic version contain in the tag.
DCMTK_VER="3.6.7"

##################################################
#
#	UTILITY FUNCTIONS
#
##################################################

render_separator () {
	echo "##################################################"
	echo "##"
	echo "##     $1"
	echo "##"
	echo "##################################################"
}

##################################################
#
#	PULL AND BUILD CVG REPO AND HMDOpView
#
##################################################

# We do this first because if the repo is private, it may require 
# logging in, which may require the user to type something at the 
# keyboard.
#
# Afterwards, they should be able to leave the script alone and
# walk away for ~2 hours for the script to complete.

render_separator "Pulling repo"

if [ ! -d "${PROJECT_DIR}" ]
then
	git clone $PROJECT_REPO
fi

pushd "${PROJECT_DIR}"
git fetch
git checkout $SUPPORTED_COMMIT
popd

##################################################
#
#	INITIALIZATION
#
##################################################
render_separator "Initializing"

sudo apt update

# Make sure all apt-get install commands have a `-y` to 
# automatically accept the "yes" option and avoid the 
# installer needing to respond to queries.

# OPTIONAL, ENABLE SSH
sudo apt-get -y install openssh-server
# sudo systemctl status ssh
sudo apt-get -y install ufw
sudo ufw allow ssh

##################################################
#
#	PULL DEPENDENCIES FROM APT
#
##################################################
render_separator "Getting Package Dependencies"

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
#	DCMTK
#
##################################################
render_separator "Installing DCMTK"
echo "With target tag $DCMTK_TAG"
echo "With target version $DCMTK_VER"

sudo apt-get -y install libxml2-dev

if [ ! -d "dcmtk" ]
then
	git clone https://github.com/DCMTK/dcmtk.git
	pushd dcmtk
	git checkout tags/$DCMTK_TAG
	
	# For this repo that's created and managed by the automation script, 
	# we're just going to build directly in the repo.
	mkdir build
	cd build
	cmake ..
	make -j8
	sudo make install
	
	popd
else
	# If the dcmtk folder already exists, check to make sure some of the
	# expected files from above are installed. If not, then chances
	# are dcmtk exists, but not in a proper way we can continue with.
	
	# Get installed location of libdcmxml, and ensure it's installed
	dcmxml_loc=$(whereis libdcmdata | cut -d: -f 2 | sed 's/^ *//g')
	if [ -z "$dcmxml_loc"]
	then
		echo "ISSUE: Detected a dcmtk folder, but dcmtk was not detected to be installed. Install DCMTK from commit tag $DCMTK_TAG manually."
		return
	fi
	
	# Check version file exists, and that version matches
	if [-r "dcmtk/VERSION"]
	then
		echo "ISSUE: Folder dcmtk is missing expected VERSION file. Please set the folder to a DCMTK repo of version $DCMTK_VER."
		return
	fi
	
	if [ "$dcm_ver" != $(cat dcmtk/VERSION)]
	then
		echo "ISSUE: Detected the wrong version of DCMTK in folder dcmtk. Make sure it is $DCMTK_VER."
		return
	fi
fi

##################################################
#
#	PULL AND BUILD MMAL
#
##################################################
render_separator "Installing MMAL"

if [ ! -f /opt/vc/lib/libmmal.so ]
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
#	WXWIDGETS
#
##################################################
render_separator "Installing wxWidgets"

# Install 7-zip to extract downloaded snapshot
sudo apt-get -y install p7zip-full
sudo apt-get -y install libgtk-3-dev build-essential 

# wx-config only exists if wxWidgets is already installed - 
# Note that this doesn't currently guard against an
# incorrect version of wxWidgets.
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
#	PULL AND BUILD CVG REPO AND HMDOpView
#
##################################################
render_separator "Finishing install"

pushd "${PROJECT_DIR}"

# Move installed MMAL into /usr regions
render_separator "Installing MMAL"
sudo make moves

render_separator "Making Project"
# Sanity clean
make clean
# Make the actual application
make all

popd

render_separator "FINISHED"


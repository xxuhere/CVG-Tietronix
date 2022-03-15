# The script runs the basic server-side systems needed to test
# the DNH with a web video streams.
#
# It also has a few utilities built in, some which leverage
# making a symlink of the script (outside the repo) for 
# convenience.
#
# FOR LINUX ONLY

# Absolute path of this script that's running,
# needed for installation and execution.
scr_abs="$(readlink -f "${BASH_SOURCE}")"
# The absolute folder of this script.
scr_dir="$(dirname "${scr_abs}")/"

echo ${scr_dir}
##################################################
#
# 	HELP
#
##################################################
if [ ! -z $1 ] && [ $1 == "--help" ];
then
	echo "A CVG utility script that boots up the DNH bus and test web cameras."
	echo "Usage:"
	echo "     cvgserverboot.sh [num]"
	echo "     cvgserverboot.sh --help"
	echo "     cvgserverboot.sh --install"
	echo "     cvgserverboot.sh --visit"
	echo "     cvgserverboot.sh --pvisit"
	echo "     cvgserverboot.sh --where"
	echo ""
	echo "Params:"
	echo "     num : Either the number 0, 1 or 2 - specifying how many v4l2rtspserver instances to run."
	echo "     --help    : Display script help"
	echo "     --install : Make a symlink of this scripts at the user's home."
	echo "     --visit   : cd into the script's directory (the CVG DNH home directory)."
	echo "     --pvisit  : pushd into the script's directory (the CVG DNH home directory)."
	echo "     --where   : print the directory of the script - which is also the dir for DNH."
	return 0
fi

##################################################
#
# 	INSTALL
#
##################################################
if [ ! -z $1 ] && [ $1 == "--install" ];
then
	x=5
	if [ ${OSTYPE} == "msys" ]
	then
		echo "Cannot execute install. Detected bash running on native Windows. Script must be run from linux, or from a ssh shell into linux."
		return 1
	fi
	
	echo ${scr_abs}
	sym_dst="/home/pi/${BASH_SOURCE}"
	echo "Installing, making a symlink to $sym_dst"
	sudo ln -sf ${scr_abs} ${sym_dst}
	return 0
fi

##################################################
#
# 	VISIT
#
##################################################
# This doesn't quite belong with this scripts purpose,
# but it's too useful not to scope creep.
#
# These add utilities that leverage the fact that the
# script is expected to be in the same directory where
# the DNH is built.
if [ ! -z $1 ] && [ $1 == "--visit" ];
then
	cd ${scr_dir}
	return
fi

if [ ! -z $1 ] && [ $1 == "--pvisit" ];
then
	pushd ${scr_dir}
	return
fi

if [ ! -z $1 ] && [ $1 == "--where" ];
then
	echo ${scr_dir}
	return
fi

##################################################
#
#	EXECUTION
#
##################################################

echo "========================================"
echo "="
echo "=     Booting DNH and related services..."
echo "="
echo "========================================"
echo ""
echo ""

# 	Check if the user has prerequisites installed
#
##################################################

echo ""
echo "Checking dependencies exist..."
echo "========================================"

# CHECK python3
echo "     python3"
has_py3=$(command -v python3)
if [ -z "$has_py3" ];
then
	echo "Did not find python3 installed."
	echo "run sudo apt-get install python3"
	return 1
fi


# CHECK v4l2rtspserver
echo "     v4l2rtspserver"
has_v4l2rtsp=$(command -v v4l2rtspserver)
if [ -z "$has_v4l2rtsp" ];
then
	echo "Did not find v4l2rtspserver installed."
	echo "Please install, see https://github.com/mpromonet/v4l2rtspserver/wiki/Setup-on-Pi"
	return 1
fi

# CHECK PYTHON MODULE websocket-client
echo "     pywsc"
has_pywsc=$(pip3 list | grep websocket-client)
if [ -z "$has_pywsc" ];
then
	echo "Did not find python3 module 'websocket-client' installed."
	echo "run sudo pip3 install websocket-client first"
	return 1
fi

# CHECK DNH
echo "     DNH"
has_dnh=$(find ${scr_dir} -maxdepth 1 -name DNH)
if [ -z "$has_dnh" ];
then
	echo "Did not find DNH where expected."
	echo "Is DNH compiled?"
	return 1
fi

echo "All dependencies successfully found!"

pushd ${scr_dir}

# 	Kill all DNH instances running
#
##################################################

echo ""
echo "Killing any existing DNH instances"
echo "========================================"
x=`ps | grep DNH | awk '{print $1 }'`
if [ ! -z "$x" ];
then
	kill "$x"
fi

# 	Kill all v4l2rtspservers running
#
##################################################

echo ""
echo "Killing any existing v4l2rtspserver instances"
echo "========================================"
x=`ps | grep v4l2rtspserver | awk '{print $1 }'`
if [ ! -z "$x" ];
then
	kill "$x"
fi


#	Kill all python3 instances
#
# While it would be nice to surgically kill only 
# the Python instances we need to, we don't get 
# that luxury. 
##################################################
echo ""
echo "Killing any existing python3 instances"
echo "========================================"
x=`ps | grep python3 | awk '{print $1 }'`
if [ ! -z "$x" ];
then
	kill "$x"
fi

# Startup services
##################################################

echo ""
echo "Starting DNH Server"
echo "========================================"
./DNH&

echo "Alloting time for startup"
sleep 2s

# Only a parameter of 2 will spawn dual webcams in 
# the same equipment.
if [ ! -z $1 ] && [ $1 = "2" ];
then
	echo ""
	echo "Starting dual RTSP Webcams"
	echo "Running command ./WebCamera2.py &"
	echo "========================================"
	pushd ./SampleEquipment
	python3 ./WebCamera2.py &
else
	echo ""
	echo "Starting single RTSP Webcam"
	echo "========================================"
	echo "Running command ./WebCamera.py &"
	pushd ./SampleEquipment
	python3 ./WebCamera.py &
fi
popd # Popping the directory when running webcams
popd # Popping the directory at the start to account for running from other CWDs
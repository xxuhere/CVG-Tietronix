# The script clears all software related to streaming cameras
# an the DNH. Essentially making the computer a blank slate
# for running DNH software without need a reboot, or manually
# tracking all the individual programs.
#
# FOR LINUX ONLY

# Absolute path of this script that's running,
# needed for installation and execution.
scr_abs="$(readlink -f "${BASH_SOURCE}")"
# The absolute folder of this script.
scr_dir="$(dirname "${scr_path}")/"

##################################################
#
# 	HELP
#
##################################################
if [ ! -z $1 ] && [ $1 == "--help" ];
then
	echo "A CVG utility script shuts down all Python and v4l2rtspserver instances"
	echo "     cvgclearsession.sh"
	echo "     cvgserverboot.sh --help"
	echo "     cvgserverboot.sh --install"
	echo ""
	echo "Params:"
	echo "     --help : Display script help"
	echo "     --install : Make a symlink of this scripts at the user's home."
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
	if [ ${OSTYPE} == "msys" ];
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
#	EXECUTION
#
##################################################

# 	Kill all DNH instances running
#
##################################################

echo "Killing all DNH instances"

x=`ps | grep DNH | awk '{print $1 }'`
if [ ! -z "$x" ];
then
	kill "$x"
fi

# 	Kill all v4l2rtspservers running
#
##################################################

echo "Killing all v2l2rtspserver instances"

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

echo "Killing all python instances"

x=`ps | grep python3 | awk '{print $1 }'`
if [ ! -z "$x" ];
then
	kill "$x"
fi

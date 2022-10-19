#!/bin/bash

# This is an automation shell script to open hdmopapp with the USB2BT connection process automated.
#
# It handles:
# 1) Disconnecting/resetting the USB2BT data in the RPi
# 2) Reconnecting the USB2BT properly
# 3) Runing the CVG application afterwards
# with a single command.

# INTRODUCTION MAST
##################################################

echo "CVG Startup script w\ Bluetooth connection to USB2BT"
echo "=================================================="
echo "This program will run the CVG application but will run"
echo "through a setup process to reset the connection to"
echo "The USB2BT device that is expected to be providing"
echo "Bluetooth capability to the Lemo 1640835 foot pedal."
echo ""

# CHECK IF hmdopapp IS AVAILABLE
##################################################
# This should come with the RPi
echo "Checking if CVG application (hmdopapp) is available..."

if ! command -v ./hmdopapp /dev/null
then
	echo ""
	echo "ERROR!: THE hmdopapp APPLICATION WAS NOT FOUND - this should exist before automating its execution with Bluetooth support." 
	echo "Make sure the hmdopapp program is ready to be run before using this script."
	echo "Exiting script."
	return
fi


# CHECK IF expect IS AVAILABLE
##################################################
echo "Checking if expect is available..."

if ! command -v expect /dev/null
then
	echo "This script requires the program 'expect' which is not found, getting with apt-get" 
	sudo apt-get install expect
else
	echo "Found program 'expect'."
	echo ""
fi

# CHECK IF bluetoothctl IS AVAILABLE
##################################################
# This should come with the RPi
echo "Checking if bluetoothctl is available..."

if ! command -v bluetoothctl /dev/null
then
	echo "This script requires the machine to have bluetoothctl, which was not found." 
	echo "Make sure it is available and then restart the script."
	echo "Exiting script."
	return
fi

# SEARCH FOR USB2BT
##################################################

while true
do
	echo "Searching if a USB2BT is connected."
	
	findinsts=`bluetoothctl -- devices | grep USB2BT`
	if [ ! -z "$findinsts" ]
	then
		dev_id=`echo $findinsts | awk -F '[ \t]' '{print $2}'`
		dev_name=`echo $findinsts | awk -F '[ \t]' '{print $3}'`
		
		echo "Found an instance of USB2BT with the full name $dev_name"
		echo "Untrusting..."
		bluetoothctl -- untrust $dev_id
		echo "Removing ..."
		bluetoothctl -- remove $dev_id
		echo ""
	else
		echo "No instances currently found, continuing"
		echo ""
		break
	fi
done

# SEARCH FOR USB2BT TO CONNECT TO IT
##################################################

echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
echo "!!"
echo "!! AT THIS TIME, SET THE USB2BT IN PAIRING MODE IF IT IS NOT ALREADY..."
echo "!!"
echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
echo ""
sleep 2s # Give the user a short chance to see things have stopped, and the last thing echoed.

# https://bbs.archlinux.org/viewtopic.php?id=270044
# https://forum.endeavouros.com/t/how-to-script-bluetoothctl-commands/18225/9
# "bluetooth -- scan on" will keep the connection open indefinitely, so 
# we need to use something more complicated to analyze the stream and figure
# out when the device is connected and then let go of the scanning session.
echo "Scanning for USB2BT as a recognized device by the RPi - leave device in pairing mode until the script finds it."
sleep 2s # Give the user a short chance to see things have stopped, and the last thing echoed.

/usr/bin/expect -f ./expect_connect.sh

# There may be a way to make this more robust, but for simplicity we'll assume
# the USB2BT that we just saw, will still be around this next moment - and 
# skip error handling that will probably never be needed. (wleu 10/19/2022)
findinsts=`bluetoothctl -- devices | grep USB2BT`

dev_id=`echo $findinsts | awk -F '[ \t]' '{print $2}'`
dev_name=`echo $findinsts | awk -F '[ \t]' '{print $3}'`
echo ""
echo "Found device $dev_name, connecting now."
sleep 2s # Give the user a short chance to see things have stopped, and the last thing echoed.
bluetoothctl -- connect $dev_id

echo "DONE!"

# SEARCH FOR USB2BT TO CONNECT TO IT
##################################################
echo "The device is now paired - getting ready to start application."
echo "Press any key to continue, preferrably with the newly paired device to confirm the wireless input connection is valid."

# https://stackoverflow.com/a/24016147/2680066
read -rsn1

./hmdopapp
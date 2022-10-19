#!/usr/bin/expect -f

# This function is used in StartWBluetooth.sh. Because of how bash and 
# expect work, this script file needs to be separate.
set timeout 60

send -- "echo Running program to wait until USB2BT is scanned and found in an expect shell"

spawn bluetoothctl
expect "Agent registered"
send -- "scan on\rdevices\r"
expect "*USB2BT*"
send -- "exit\r"
expect eof
# Script to close all v4lrtspservers currently running.

kill ` ps | grep v4l2rtspserver | awk '{print $1}'`

# It's assumed the developers of the repos are not deeply familiar 
# with bash and standard Linux shell functions, so a brief breakdown.
# 
#
# 	ps 		-> Get all running processes
#	grep	-> Find all lines mentioning v412rtspserver
#	awk		-> Text manipulation program. In this case, treats
#		the input as table entries and gets the 2nd column where
#		the process id is.
#	kill	-> Force process IDs to close.
#
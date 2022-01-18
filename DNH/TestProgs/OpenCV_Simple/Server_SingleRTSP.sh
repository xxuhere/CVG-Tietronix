
# Runs a single RTSP server using v4l2 RTSP server.
#
# See the repo's root README file for information on
# installing 

CAMPATH1="/dev/video0"

v4l2rtspserver -F 25 -H 480 -W 640 -P 8555 ${CAMPATH1}
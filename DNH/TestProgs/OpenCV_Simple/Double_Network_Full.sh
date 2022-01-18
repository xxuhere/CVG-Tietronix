
# Bash script to run OCVSimple with two V4L2 streams.
# 
# Requires installing v4l2rtspserver, see the repo's root
# README for instructions on how to install it. See
# Server_DoubleRTSP.sh for a reference on how to run
# two V4L2 servers simultaneously.
#

CAMPATH1="/dev/video0"
CAMPATH1="/dev/video2"

# Assumed to run on the same machine as the V4L2 RTSP server.
# The assumed ports should match the ones used in Server_DoubleRTSP.sh

./OCVSimple --v --api CAP_FFMPEG \
	--c1 rtsp://localhost:8555/unicast \
	--c2 rtsp://localhost:8556/unicast
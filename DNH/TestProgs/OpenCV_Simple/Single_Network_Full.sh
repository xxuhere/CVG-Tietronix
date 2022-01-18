
# Bash script to run OCVSimple with a V4L2 stream.
# 
# Requires installing v4l2rtspserver, see the repo's root
# README for instructions on how to install it. See 
#Server_SingleRTSP.sh or Server_DoubleRTSP.sh for a reference 
# on how to run the V4L2 server.

# Assumed to run on the same machine as the V4L2 RTSP server.
# The assumed port should match the one used in Server_SingleRTSP.sh

./OCVSimple --v --api CAP_FFMPEG --c1 rtsp://localhost:8555/unicast

# Bash script to run OCVSimple with a two cameras 
# on RPi Linux. The camera dev paths are assumed
# to be /dev/video0 and /dev/video2

CAMPATH1="/dev/video0"
CAMPATH2="/dev/video2"

./OCVSimple --v --count 2 --c1 ${CAMPATH1} --c2 ${CAMPATH2}
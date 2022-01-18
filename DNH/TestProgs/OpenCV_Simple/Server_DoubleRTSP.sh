CAMPATH1="/dev/video0"
CAMPATH2="/dev/video2"

echo "Opening Server 1 on port 8555"
echo "#####################################"
v4l2rtspserver -F 25 -H 480 -W 640 -P 8555 ${CAMPATH1} &

# Buy some time for the server to initialize and spit out text to the
# terminal.
sleep 2s

echo "Opening Server 2 on port 8556"
echo "#####################################"
v4l2rtspserver -F 25 -H 480 -W 640 -P 8556 ${CAMPATH2} &

# Buy some time for the server to initialize and spit out text to the
# terminal.
sleep 2s

echo "#####################################"
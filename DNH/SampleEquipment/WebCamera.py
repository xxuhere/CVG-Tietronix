
#!/usr/bin/env python

"""
WebCamera.py: An example for registering an equipment
with web camera support.

The webcamera is provided via v4l2rtspserver, which means
this script is for a linux machine (specifically a RaspberryPi)
with v4l2rtspserver installed.
"""

import time
from RegisterUtils import *
import sys
import os

HOSTNAME        = None          # The hostname of the DNH
PORT            = None          # The port of the DNH
WSCONNECTION    = None          # Websocket connection
DEVICE          = "/dev/video0" # The device port
STREAMPORT      = "8555"        # The port for the camera server to host the stream
PREKILLPORT     = True          # If true, kill whatever is using the STREAMPORT (if used) before trying to use it

def Init():
    """
    Initialize the app. Including getting param line
    parameters for data on how to connect to the DNH.
    This may be something we want to generalize by
    moving into RegisterUtils for everything else to
    use in a standard way.
    """
    global HOSTNAME
    global PORT
    global WSCONNECTION
    global DEVICE
    global STREAMPORT
    global PREKILLPORT
    
    # See if there are command line options
    argc = len(sys.argv)
    i = 1
    while i < argc:
        if sys.argv[i] == "--host":
            i += 1
            HOSTNAME = sys.argv[i]
        elif sys.argv[i] == "--port":
            i += 1
            PORT = sys.argv[i]
        elif sys.argv[i] == "--device":
            i += 1
            DEVICE = sys.argv[i]
        elif sys.argv[i] == "--strport":
            i += 1
            STREAMPORT = sys.argv[i]
            
        i += 1

    # Default values for unspecified parameters
    if not PORT:
        PORT = GetDNHWSPort()
        
    if not HOSTNAME:
        HOSTNAME = GetDNHHostname()
        
    # Create final connection path
    WSCONNECTION = f"ws://{HOSTNAME}:{PORT}/realtime"
    print("\t\thostname : " + HOSTNAME )
    print("\t\tport : " + str(PORT) )
    print("\t\turi : " + WSCONNECTION )


if __name__ == "__main__":    
    print("STARTING")
    
    print("\tInit() Start")
    Init()
    print("\tInit End")
    
    if PREKILLPORT:
        killcmd = "kill `lsof -i | grep " + STREAMPORT + " | awk '{print $2}'`"
        print("\tPrekilling port " + STREAMPORT)
        print("\t\t" + killcmd)
        os.system(killcmd)
        
    # Initialize the camera before the server. If we can't get
    # the server up, there no point in registering the camera.
    # Plus if a broadcast is sent and processed by other Equipment
    # before we can get the server up, they'll connect to nothing.
    print("\tInitializing webcam server")
    os.system(f'v4l2rtspserver -F 25 -H 480 -W 640 -P {STREAMPORT} {DEVICE} &')
    
    print("\tRegistering")
    reg = CreateEqRegistration("WebcamStream", "WUSTL/TIE", "camera")
    reg["channels"] = [{"label":"RGB", "port":STREAMPORT, "type":"RGB", "proto":"rtsp", "endpoint":"unicast"}]
    success, ws, guid = ConnectWS(WSCONNECTION, reg)
    if not success:
        print("\tRegistering failed")
        exit()
    else:
        print("\tRegistered")
        
    # The program is just going to stay connected and do nothing.
    # We keep it around on the DNH to keep advertising about the
    # camera feed.
    print("Idling")
    while True:
        time.sleep(1.0)
        result = ws.recv()
        print(result)
        
    # Potential TODOS:
    #   - End the loop (and application) when any kind of disconnect
    #   is detected.
    #   - End the instance of the v4l2rtspserver created from
    #   running the server

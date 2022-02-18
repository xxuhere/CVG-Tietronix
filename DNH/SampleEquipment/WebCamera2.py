
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
DEVICE1         = "/dev/video0" # The device port
DEVICE2         = "/dev/video2" # The device port
STREAMPORT1     = "8555"        # The port for the camera server to host the stream
STREAMPORT2     = "8556"        # The port for the camera server to host the stream
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
    global DEVICE1
    global DEVICE2
    global STREAMPORT1
    global STREAMPORT2
    global PREKILLPORT
    
    # Default values. We set them ahead of parsing params
    # in case we need to display them for --help.
    PORT = GetDNHWSPort()
    HOSTNAME = GetDNHHostname()
    
    argc = len(sys.argv)
    
    #Check if there's a --help
    # This is a separate parsed option from the others because after it
    # displays the help, the application exits.
    i = 1
    while i < argc:
        if sys.argv[i] == "--help":
            print("WebCamera.py, opens a test webcamera equipment. It both opens a streaming camera and registers to a DNH bus.")
            print("")
            print("usage: python[3] WebCamera.py [options]");
            print("");
            print("options:");
            print("   --host");
            print(f"      Specify the DNH host. Defaulted to {HOSTNAME}.");
            print("   --port");
            print(f"      Specify the DNH port. Defaulted to {PORT}.");
            print("   --device1");
            print(f"      Specify path of the first camera to host a video stream for. Defaulted to {DEVICE1}.");
            print("   --device2");
            print(f"      Specify path of the second camera to host a video stream for. Defaulted to {DEVICE2}.");
            print("   --strport1");
            print(f"      Specify RTSP port of the first camera to host a video stream for. Defaulted to {STREAMPORT1}.");
            print("   --strport2");
            print(f"      Specify RTSP port of the second camera to host a video stream for. Defaulted to {STREAMPORT2}.");
            exit()
        i += 1
    
    # See if there are command line options
    i = 1;
    while i < argc:
        if sys.argv[i] == "--host":
            i += 1
            HOSTNAME = sys.argv[i]
        elif sys.argv[i] == "--port":
            i += 1
            PORT = sys.argv[i]
        elif sys.argv[i] == "--device1":
            i += 1
            DEVICE1 = sys.argv[i]
        elif sys.argv[i] == "--device2":
            i += 1
            DEVICE2 = sys.argv[i]
        elif sys.argv[i] == "--strport1":
            i += 1
            STREAMPORT1 = sys.argv[i]
        elif sys.argv[i] == "--strport2":
            i += 1
            STREAMPORT2 = sys.argv[i]
            
        i += 1
        
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
        killcmd = "kill `lsof -i | grep " + STREAMPORT1 + " | awk '{print $2}'`"
        print("\tPrekilling port " + STREAMPORT1)
        print("\t\t" + killcmd)
        os.system(killcmd)
        
        killcmd = "kill `lsof -i | grep " + STREAMPORT2 + " | awk '{print $2}'`"
        print("\tPrekilling port " + STREAMPORT2)
        print("\t\t" + killcmd)
        os.system(killcmd)
        
    # Initialize the camera before the server. If we can't get
    # the server up, there no point in registering the camera.
    # Plus if a broadcast is sent and processed by other Equipment
    # before we can get the server up, they'll connect to nothing.
    print("\tInitializing webcam servers")
    os.system(f'v4l2rtspserver -F 25 -H 480 -W 640 -P {STREAMPORT1} {DEVICE1} &')
    os.system(f'v4l2rtspserver -F 25 -H 480 -W 640 -P {STREAMPORT2} {DEVICE2} &')
    
    print("\tRegistering")
    reg = CreateEqRegistration("WebcamStream", "WUSTL/TIE", "camera")
    reg["channels"] = [ {"label":"RGB", "port":STREAMPORT1, "type":"RGB", "proto":"rtsp", "endpoint":"unicast"}, 
                        {"label":"RGB", "port":STREAMPORT2, "type":"RGB", "proto":"rtsp", "endpoint":"unicast"}]
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

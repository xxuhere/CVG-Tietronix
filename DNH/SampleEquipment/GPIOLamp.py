#!/usr/bin/env python

"""
GPIOLamp.py: An example of locally running a Python script on the
RasberryPi to be a liason to an LED connected via GPIO pins.

The script will register as an Equipment for the DNH bus.
"""

# Note that RPi.GPIO must be manually installed if not executing on
# Raspbian (such as Ubuntu for Raspberry Pi).
#
# This can be done with sudo apt-get install python3-RPi.GPIO
# See https://github.com/gpiozero/gpiozero/issues/837#issuecomment-765863132
# for more information.

import time
from RegisterUtils import *
import RPi.GPIO as GPIO

# GPIO Pin
LEDPIN = 2

# Local copy of toggle param
toggleVal = False
# Enforce GPIO (LED) or toggleVal
GPIO.setmode(GPIO.BCM)
GPIO.setup(LEDPIN, GPIO.OUT)
GPIO.output(LEDPIN, toggleVal)

#   CREATING REGISRATION REQUEST FOR EQUIPMENT
#
##################################################

# The DNH to connect to
#WSCONNECTION = "ws://"192.168.1.84:5001/realtime"
WSCONNECTION = f"ws://{GetDNHHostname()}:{GetDNHWSPort()}/realtime"

# Create registration
reg = CreateEqRegistration("RPi GPIO LED", "Tietronix Software Inc", "lamp")
# Add "toggle" Param to control LED
AddEqParam(reg, "toggle", "Toggle", "bool", "onoff",
           GPIO.HIGH if toggleVal else GPIO.LOW)

# Register
success, ws, guid = ConnectWS(WSCONNECTION, reg)
print("Logged in with GUID " + guid)

#   APPLICATION LOOP
#
##################################################
while True:
    # Block application until a command is received.
    result = ws.recv()
    # Convert payload to JSON
    apiObj = json.loads(result)

    # Throw everything else away that isn't "changedval"
    # for ourself.
    if apiObj["apity"] != "changedval":
        continue
    if apiObj["guid"] != guid:
        continue

    # We only have one parameter to check for.
    if "toggle" in apiObj["sets"]:
        print("Detected toggle change")
        toggleVal = apiObj["sets"]["toggle"]["val"]
        print("Listing value")
        print(toggleVal)
        # reify the LED change request
        GPIO.output(LEDPIN, GPIO.HIGH if toggleVal else GPIO.LOW)

GPIO.cleanup()

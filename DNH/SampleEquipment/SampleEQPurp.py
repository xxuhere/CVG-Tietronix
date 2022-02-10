#!/usr/bin/env python

"""
SampleEQ.py: An example for registering an equipment
with various types of parameters - and a purpose.
"""

import time
from RegisterUtils import *

#WSCONNECTION = "ws://"192.168.1.84:5001/realtime"
WSCONNECTION = f"ws://{GetDNHHostname()}:{GetDNHWSPort()}/realtime"

# %% Registration

print("STARTING")

#   CREATING REGISRATION REQUEST FOR EQUIPMENT
#
##################################################

# The registration for the Equipment
reg = CreateEqRegistration("Stellar NIR", "Europa Optics GMBH", "camera")

# The exposed parameters of the Equipment
AddEqParam(reg, "toggle", "Toggle", "bool", "onoff", True)
AddEqParam(reg, "zoom", "Zoom", "float",
           "Multiplier", 1.0, 1.0, None, 0.5, 2.0)
AddEqParam(reg, "incr", "Increments", "int", "mm", 1, 1)
AddEqParam(reg, "str", "String Test", "string", "", "String Value")
AddEqParam(reg, "mode", "Mode", "enum", "", "Thing_1",
           possibs=["Thing_1", "Thing_2", "Thing_3"])
AddEqParam(reg, "evt", "Dummy Event", "event", "")
reg["channels"] = ["NIR"]
reg["topics"] = []
print(reg)

success, ws, guid = ConnectWS(WSCONNECTION, reg)


#   REGISTERING
#
##################################################
print("\nQuering Equipment Enumeration")
reqeq = {"apity": "equipment", "postage": "__MYPOSTAGE__"}
s = json.dumps(reqeq)
print(s)
# Send registration
ws.send(s)
# Receive registration success response
result = ws.recv()
print(result)

#   RANDOM API TESTS
#
##################################################

# Test valset to change a variable with the "self" guid.
print("\nTesting changing bool")
setv = {}
setv["apity"] = "valset"
setv["guid"] = "self"
setv["sets"] = {"toggle": "False"}
print("\tSending")
ws.send(json.dumps(setv))
result = ws.recv()
print("\tReceived:")
print(result)

# Testing valget
print("\nTesting retrieving bool")
setv = {}
setv["apity"] = "valget"
setv["guid"] = "self"
setv["gets"] = ["toggle"]
print("\tSending")
ws.send(json.dumps(setv))
result = ws.recv()
print("\tReceived:")
print(result)
# %%

#   APP LOOP
#
##################################################
while True:
    time.sleep(1.0)
    result = ws.recv()
    print(result)

ws.close()
print("Closed")

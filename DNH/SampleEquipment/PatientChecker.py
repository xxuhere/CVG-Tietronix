#!/usr/bin/env python

"""
Validates if the patient has a record in the database of 
having the flourescent dye of the correct type injected 
at the appropriate time in the past relative to the current
time for the FGS surgery.

The example requires the sample patient database server
contained in subfolder PatientDBServer to be running.
"""

from RegisterUtils import *
import datetime
import requests
import sys
import time


EXPECTED_DYE = "LS301"

SECONDS_IN_HOURS = 60 * 60

# How much time is required for the dye to settle in the patient
MIN_TIME_SECONDS = SECONDS_IN_HOURS * 15
# How much time until the dye is processed out of the patient's body
MAX_TIME_SECONDS = SECONDS_IN_HOURS * 30


def SendLS301SystemStatus(dnhSocket, patname, status, checktime=None, lasttime=None):
    """
    Send an updated status on the patient into the DNH bus. This function
    expects the DNH to have certain datacache variables and that it is not
    fighting other Equipment to use them:
        - patname
        - timing
        - checktime
        - injecttime

    Parameters
    ----------
    dnhSocket : websocket
        The websocket that's connected to the DNH bus.
    patname : String
        The patient name. This should be the DNH patname value that we 
        queried the patient DB history with.
    status : String
        This needs to match one of the status values,
        ["unchecked", "mismatch", "match", "unknown", "missing", "checking"]

    Returns
    -------
    None.

    """
    # https://www.codegrepper.com/code-examples/python/convert+from+epoch+to+utc+python
    setdata = {}
    setdata["apity"] = "valset"
    setdata["guid"] = "system"
    setdata["sets"] = {}
    setdata["sets"]["patname"] = patname
    setdata["sets"]["timing"] = status

    if checktime != None:
        setdata["sets"]["checktime"] = datetime.datetime.utcfromtimestamp(
            int(checktime)).strftime('%Y-%m-%d %H:%M:%S')
    else:
        setdata["sets"]["checktime"] = ""

    if lasttime != None:
        setdata["sets"]["injecttime"] = datetime.datetime.utcfromtimestamp(
            int(lasttime)).strftime('%Y-%m-%d %H:%M:%S')
    else:
        setdata["sets"]["injecttime"] = ""

    dnhSocket.send(json.dumps(setdata))


def VerifyPatientTime(dnhSocket, patname, databaseHost):
    """
    Check a patient's DB history to see if they were given a dye injection at
    the appropriate time to coincide with a surgery at the time the function
    is called.

    Parameters
    ----------
    dnhSocket : websocket
        The websocket that's connected to the DNH bus.
    patname : String
        The patient name. This should be teh DNH patname value that we query
        the patient DB history with.
    databaseHost : String
        Hostname of the database server. This should either be a dns, 
        localhost, or an api.

    Returns
    -------
    None.

    """
    #  Since we're checking the status, don't risk some previous lingering
    # status to remain while we do our check.
    curTime = time.time()
    SendLS301SystemStatus(dnhSocket, patname, "checking", curTime)

    print("Checking patient preop history with database")
    r = requests.get(
        f"http://{databaseHost}:5000/patient?action=info&name={patname}")
    if r.status_code != 200:
        print("Could not contact database server")
        return

    print(r.text)
    respObj = json.loads(r.text)
    lastDye = None

    if "events" not in respObj:
        print("Patient not in database")
        SendLS301SystemStatus(dnhSocket, patname, "unknown", curTime)
        return

    for evt in respObj["events"]:
        # If the event a relevant dye injection event?
        if evt["event"] == "preop" and evt["type"] == "injection" and evt["info"] == EXPECTED_DYE:
            if lastDye == None:
                lastDye = evt["entered"]
            else:
                lastDye = max(evt["entered"], lastDye)

    curTime = time.time()
    # Did they not even get injected with a dye?
    if not lastDye:
        print("Detected patient never got a " + EXPECTED_DYE)
        SendLS301SystemStatus(dnhSocket, patname, "missing", curTime)
        return

    # Did the dye get injected at the wrong time, to where the wrong amount of
    # time has passed at the current moment?
    timeSinceDye = curTime - lastDye
    if timeSinceDye < MIN_TIME_SECONDS or timeSinceDye > MAX_TIME_SECONDS:
        print("Detected patient's most recent dye is outside required time window.")
        SendLS301SystemStatus(dnhSocket, patname, "mismatch", curTime, lastDye)
        return

    # If they got inject and it's not too late or too recent, we've hit
    # the Goldilocks time window.
    print("Patient is within the proper time window.")
    SendLS301SystemStatus(dnhSocket, patname, "match", curTime, lastDye)


def main():
    """ Program entry point. """

    # WSCONNECTION = "ws://"192.168.1.84:5001/realtime"
    WSCONNECTION = f"ws://{GetDNHHostname()}:{GetDNHWSPort()}/realtime"

    reg = CreateEqRegistration("Patient Check", "", "spectator")
    success, ws, guid = ConnectWS(WSCONNECTION, reg)

    if not success:
        print("Could not connect Patient dye checker to DNH")
        sys.exit()

    # Query equipment so we can handle the patname if it's available.
    ws.send(json.dumps({"apity": "equipment"}))

    # Message listening loop
    #
    ##################################################
    while True:
        # Block application until a command is received.
        result = ws.recv()
        apiObj = json.loads(result)  # Convert payload to JSON

        # If equipment, check if patname if available - if so, process it.
        if apiObj["apity"] == "equipment":

            # We'll handle patname at this level if we find it, theres
            # a lot of nesting just to find it.
            patname = None

            eqs = apiObj["equipment"]
            for x in eqs:
                # The datacache is in the system Equipment
                if x["guid"] != "system":
                    continue
                if "params" not in x:  # Sanity check
                    break
                for e in x["params"]:
                    if e["id"] == "patname":
                        patname = e["current"]
                        break
                break

            # If we found it, verify it. If not, post a notice about how we
            # don't have it.
            if patname == None:
                print("Did not find patient name registered in the system")
                SendLS301SystemStatus(ws, patname, "unknown")
                continue
            elif len(patname) == 0:
                print("Patient name has not been set yet")
                SendLS301SystemStatus(ws, patname, "unknown")
                continue
            else:
                VerifyPatientTime(ws, patname, GetDNHHostname())

        # If changedval, check if it's the system patname - if so, process it
        if apiObj["apity"] == "changedval":
            # We only care about the system datacache
            if apiObj["guid"] != "system":
                continue
            if "patname" in apiObj["sets"]:
                patname = apiObj["sets"]["patname"]["val"]
                VerifyPatientTime(ws, patname, GetDNHHostname())


if __name__ == "__main__":
    main()

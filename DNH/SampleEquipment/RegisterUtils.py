#!/usr/bin/env python

"""
RegisterUtils.py: A utility library for Python scripts to 
register and use the DNH bus.
"""

from websocket import create_connection
import json
import sys

#%% Utilities

def GetDNHHostname():
    """
    A standard way to get the server host name. Either using an agreed upon 
    hard-coded value, or through a parameter via command line.

    Returns
    -------
    ret : String
        The host name.

    """
    ret = "localhost"
    
    # Check if the host is defined as commandline arguments
    n = len(sys.argv)
    for i in range(1, n-1):
        if sys.argv[i] == "--host":
            ret = sys.argv[i + 1]
            break
    
    return ret

def GetDNHHTTPPort():
    """
    A standard way to get the server port. Either using an agreed upon
    hard-coded value, or through a parameter via command line.

    Returns
    -------
    ret : Int
        The port number for HTTP REST requests.

    """
    ret = 5700
    
    # Check if the port is defined as commandline arguments
    n = len(sys.argv)
    for i in range(1, n-1):
        if sys.argv[i] == "--port":
            ret = int(sys.argv[i+1])
            break
    
    return ret
    
def GetDNHWSPort():
    return 5701
    
def CreateEqRegistration(name, manufacturer, ty):
    """
    Create a registration object that can be used to
    register the Equipment on a DNH bus.
    
    The return value will be a Python object. It must
    be converted to a JSON string in order to use.
    
    Parameters
    ----------
    name : String
        The name of the Equipment
    manufacturer : String
        Optional. The manufacturer of the Equipment
    ty : String
        The type of the equipment.

    Returns
    -------
    req : Object
        A Python object that has the basics of a DNH registration

    """
    req = {"apity" : "register"}
    req["name"] = name
    req["manufacturer"] = manufacturer
    req["type"] = ty
    #
    # Params is sent back empty - but they can be added
    # with a call to AddEqParam.
    req["params"] = []
    #
    return req

def AddEqParam(eq, idv, label, ty, unit, default = None, val = None, fail = None, minv = None, maxv = None, possibs = None):
    """
    Create and add an Equipment parameter to an 
    Equipment object.

    Parameters
    ----------
    eq : Equipment. This will be a return value from CreateEqRegistration().
        DESCRIPTION.
    idv : String
        The API ID of the parameter.
    label : String
        The human readable name of the parameter.
    ty : String
        The parameter type. This should be "bool", "string", "int", "float" or "enum".
    unit : String
        Optional. The type of units the parameter is.
    default : Variable, optional
        The default value. The type depends on the parameter type. The default is None.
    val : Variable, optional
        The current value. The type depends on the parameter type. The default 
        is None. It not specified, the server will use the default value - thus, if 
        not specified the default value must be specified.
    fail : Variable, optional
        The fail value. The default is None.
    minv : Variable, optional
        The minimum allowed value. The type depends on the parameter type. 
        Only valid for int and float parameters.The default is None.
    maxv : Variable, optional
        The maximum allowed value. The type depends on the parameter type.
        Only valid for int and float parameters. The default is None.
    possibs : String array, optional
        Only used for enum types. It specifies the possible string values for
        an enum parameter. The default is None.

    Returns
    -------
    param : Param
        The Param object that was added to the Equipment's parameters.

    """
    print(default)
    print(val)
    param = {}
    param["id"] = idv
    param["type"] = ty
    if label:
        param["label"] = label
    if unit:
        param["unit"] = unit
    if val != None:
        param["current"] = val
        print("Set current")
    if default != None:
        param["default"] = default
        print("Set default")
        print(param["default"])
    if fail != None:
        param["fail"] = fail
    if minv != None:
        param["min"] = minv
    if maxv != None:
        param["max"] = maxv
    if possibs:
        param["possible"] = possibs
        
    eq["params"].append(param)
    return param
    
def ConnectWS(loc, reg):
    """
    Connect to a WebSocket and attempt to register
    an Equipment.

    Parameters
    ----------
    loc : String
        DESCRIPTION.
    reg : Equipment
        The Python Equipment registration object.

    Returns
    -------
    bool
        True if the registration was sent succesfully.
    ws : WebSocket
        The WebSocket connection if connected successfully. Else Null.
    String
        The GUID if registered successfully. Else, an empty string.

    """
    print("CONNECTING")
    ws = create_connection(loc)
    if ws == None:
        return False, ws, None
    
    print("CONNECTED")
    # The Equipment registration is converted to a JSON string
    jsonReg = json.dumps(reg, indent=4)
    print("Sending regstration JSON:\n----------")
    print(jsonReg)
    ws.send(jsonReg)
    result = ws.recv()
    print("\nReceived response:\n")
    print(result)

    retobj = json.loads(result)
    if( retobj["apity"] == "error"):
        return False, ws, None
    
    print(result)
        
    selfGUID = retobj["guid"]
    print("GUID: " + selfGUID)
    return True, ws, selfGUID
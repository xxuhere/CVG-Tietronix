# -*- coding: utf-8 -*-

"""
A simple placeholder server to simulate a patient information server.

The idea is that there could be a network store of patient data - most
importantly to confirm that the patient was injected with fluorescent dye
in a certain time window previous to the operation (e.g., ~24 hrs ago).

Some extra patient information is also included, but the important part is
the patient's "events" member which will contain an array of relevant
events, including pre-ops.

NOTE THAT THIS IS A SIMPLE IMPLEMENTATION FOR FUNCTION PROOF OF CONCEPT.
- THIS SERVER SHOULD NOT BE USED IN PRACTICE
- THE EXACT SERVER INTERFACE/API WILL BE DIFFERENT
- A DEPLOYED API MAY NOT USE HTTP
- A DEPLOYED HTTP IMPLEMENTATION SHOULD BE HTTPS <- note the 'S'
- THIS HAS NO HIPPA CONSIDERATIONS

We mimic a database by simply storing filenames under the format of 
{firstname} {lastname}.json

    CREATING A PATIENT:
==================================================
Before a patient can have their information accessed or have an inejction 
event, they must first have an existing database entry (JSON file).

An example patient return with http://*/patient?action=create&name={patient name}
Example: http://*/patient?action=create&name=Jane Doe


    ACCESSING PATIENT INFORMATION:
==================================================
An example patient return with http://*/patient?action=info&name={patient name}
Example: http://*/patient?action=info&name=Jane Doe
{
     "name" : "Linda St.Clair",
     "age" : 30,
     "birthday" : Jan-5-1995
     "events" : 
         [
             {
                 "event": "schedule",
                 "type": "surgery",
                 "info": "lumpectomy",
                 "entered" : 1635957324
                 "schedule" : 1636790400
             },
             {
                 "event": "preop",
                 "type": "injection"
                 "info": "LS301",
                 "entered" :  1636712104
             }
         ]
}
    

    CREATE AN INJECTION EVENT:
==================================================
The only type of event is an injection event. When the injection endpoint
is called, it will add an entry at the current server timestamp.

An example patient return with http://*/patient?action=logprep&name={patient name}
Example: http://*/patient?action=logprep&name=Jane Doe


    RESETTING THE "DATABASE"
==================================================
To reset the patient information, simply delete all user patient JSON files 
on the filesystem.

"""


# conda
from flask import Flask, request
import json
from os.path import exists
import time

# %% Flask server implementations

app = Flask(__name__)


@app.route('/patient')
def Endpoint_Patient():
    act = request.args.get('action')
    pname = request.args.get('name')

    # Error checking for patient
    if not pname:
        return json.dumps({"status": "error", "reason": "missing patient"})

    # Create a new patient - as simple as creating a new
    # JSON file in the python server directory.
    if act == "create":
        if CreatePatient(pname):
            return json.dumps({"status": "success"})
        else:
            return json.dumps({"status": "error", "reason": "Could not create new patient file."})

    # Retrieve the history of a patient (as JSON)
    if act == "info":
        patobj = GetPatient(pname)
        if not patobj:
            return json.dumps({"status": "error", "reason": "Could not retrieve patient information."})
        else:
            return json.dumps(patobj)

    # Insert a dye entry at the current time.
    if act == "logprep":
        patobj = GetPatient(pname)
        if not patobj:
            return json.dumps({"status": "error", "reason": "Could not retrieve patient information."})

        InjectPatientObject(patobj)
        SavePatient(patobj)

        return json.dumps({"status": "success"})

    return json.dumps({"status": "error", "reason": "Unknown action."})

# %% Business utility functions


def CreatePatientFilename(name):
    """
    Generate a patient filename based off a patient's name.

    This should be the ONLY thing generating these filenames. If anything needs
    a filename, they MUST use this function instead of generating their own.

    Parameters
    ----------
    name : String
        The patient's name. It's expected to be in the form 
        {firstname} {lastname}
        Without the curly braces. For simplicity, all names testing with this
        proof of concept will omit the possibility of a middle name.

    Returns
    -------
    String
        The canonical filename of where the json file should be for the 
        patient.

    """
    return "Patient_" + name + ".json"


def InjectPatientObject(patobj, ovrrdTime=None):
    """
    Add a new dye injection event into a patient's history.

    Parameters
    ----------
    patobj : Dictionary
        Patient's data
        
    ovrrdTime: None or int
        It not none, it's a timestamp override.

    Returns
    -------
    None.

    """
    newEvt = {}
    newEvt["event"] = "preop"
    newEvt["type"] = "injection"
    newEvt["info"] = "LS301"
    if not ovrrdTime:
        newEvt["entered"] = int(time.time())
    else:
        newEvt["entered"] = ovrrdTime
    patobj["events"].append(newEvt)
    

def CreatePatient(name):
    """
    Create a patient file.

    This will fail if the patient name converts to an invalid filename, or if
    the patient file already exists.

    Parameters
    ----------
    name : String
        The patient's name.

    Returns
    -------
    A Python dictionary representing the created patient, or None if the 
    creation failed.

    """
    patientFile = CreatePatientFilename(name)
    if exists(patientFile):
        return None

    patient = {}
    patient["name"] = name
    patient["events"] = []
    file = open(patientFile, 'w')
    json.dump(patient, file)
    file.close()
    return patient


def GetPatient(name):
    """
    Get the patient's information as a Python dictionary.

    This will fail if the patient does not have an existing file.

    Parameters
    ----------
    name : String
        The patient's name.

    Returns
    -------
    ret : Dictionary
        Python dictionary. Or None if the retrieval failed.

    """
    patientFile = CreatePatientFilename(name)
    if not exists(patientFile):
        return None

    file = open(patientFile, 'r')
    ret = json.load(file)
    file.close()
    return ret


def SavePatient(patient):
    """
    Save a patient's data.

    Parameters
    ----------
    patient : Dictionary
        The patient's data in the form of a Python dictionary.

    Returns
    -------
    True if the data was saved. Else, false.

    """
    patientFile = CreatePatientFilename(patient["name"])
    if not exists(patientFile):
        return False

    file = open(patientFile, 'w')
    json.dump(patient, file)
    file.close()


# %% Main application

if __name__ == '__main__':
    print("Connecting to patient's database.")
    app.run()

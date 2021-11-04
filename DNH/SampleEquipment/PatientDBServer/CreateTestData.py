"""
Create dummy data for Server.py
"""

import PatServer
import time
import sys

# %% Create dummy patient data

# This assumes these patient files don't exist. If they do, they should be 
# removed before executing this script.

p0 = PatServer.CreatePatient("Jane Doe")
p1 = PatServer.CreatePatient("John Doe")
p2 = PatServer.CreatePatient("Linda St.Clair")
p3 = PatServer.CreatePatient("Jonus Moynahan")

# CreatePatient doesn't allow existing patient to be created. To run this
# dummy data generation script, none of them can currently exist.
#
# The user is instructed to delete the existing JSONs - and while tedious,
# this also includes any JSONs included before the issue was detected.
for p in [p0, p1, p2, p3]:
    if p == None:
        print("PATIENT FILES ALREADY EXISTED; DELETE THEM ALL FIRST!")
        sys.exit()

# %% Insert dummy event data

# Note that because the time of the events for InjectPatientObject is the 
# current server time, the exact times will never be the same each time the
# dummy events are added.

PatServer.InjectPatientObject(p1)

# For variation, we'll add a patient will multiple events.
dayInSecs = 60 * 60 * 24
PatServer.InjectPatientObject(p2, int(time.time()) - dayInSecs)
PatServer.InjectPatientObject(p2)

# %% Save patients

PatServer.SavePatient(p0)
PatServer.SavePatient(p1)
PatServer.SavePatient(p2)
PatServer.SavePatient(p3)

print("Finished creating dummy patient data.")

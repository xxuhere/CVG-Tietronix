A low complexity patient information server for proof-of-concept testing. It is used for functional examples of pulling patient information from a network location.

# Requirements

Implemented in Python3 and flask. 

# Running

 When running the `PatServer.py` script, make sure the working directory is set to the script's direction. When running the script from a terminal, this can be done by making cure the terminal's current directory is set to `ProxyPatientServer` before executing `PatServer.py`.

The current working directory requirement also applied to `CreateTestData.py`.

Documentation for how to use the Flask server, `PatServer.py` can be found at the top of the script.

# Setup

Patient JSON data should not be committed to the repo.

To create sample data which can be used as a baseline, delete all (if any) *.json files in the directory and then run `CreateTestData.py`.




#!/usr/bin/env python2

import subprocess
import os

def main():
	print("Creating the versioning files")
	
	# https://stackoverflow.com/a/9350788/2680066
	# For Windows, we need to specify the script output directory explicitly because
	# visual studio has its own mind on what the work directory will be.
	script_dir = os.path.dirname(os.path.realpath(__file__))
	print("Ouput script directory: " + script_dir)
	
	shortVer = get_cmd_string_result(["git", "rev-parse", "--short", "HEAD"])
	#
	# https://stackoverflow.com/a/3815007/2680066
	commitDateTime = get_cmd_string_result(["git", "show", "-s", "--format=%ci", "HEAD"])
    
	print("\tShort SHA1 hash of the current HEAD commit: " + shortVer)
	print("\tCommit date of the current HEAD commit: " + commitDateTime)
	print("\tCreating src/GenVer.h")
	
	entire_out_path = script_dir + "/src/GenVer.h"
	with open(entire_out_path, 'w') as generated_file:
		generated_file.write(" // This file was automatically generated, DO NOT EDIT!\n")
		generated_file.write(" // Instead, see contents of {REPO_ROOT}/create_version_src.py\n")
		generated_file.write( "#define GENVER_SHORTSHA \"" + shortVer + "\"\n")
		generated_file.write( "#define GENVER_DATETIME \"" + commitDateTime + "\"\n")
		generated_file.write( "\n" )
	print(entire_out_path + " created!")
	
def get_cmd_string_result(args):
	p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	out, err = p.communicate()
	return out.strip()
    
if __name__ == "__main__":
    main()
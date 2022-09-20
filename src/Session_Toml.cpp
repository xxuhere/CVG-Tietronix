 // This file was automatically generated, DO NOT EDIT!
 // Instead, see contents of {REPO_ROOT}/_SrcAssets/TOML_Template


const char* szSessionTomlString = 
"###################################################\n"
"##\n"
"##	Session.toml file\n"
"##\n"
"## This contains per-session data that should be modified\n"
"## for each patient and operation instance.\n"
"##\n"
"###################################################\n"
"\n"
"[patient]\n"
"	# https://dicom.innolitics.com/ciods/us-image/patient/00100020\n"
"	upn = \"Replace this with patient's unique patient number\"\n"
"\n"
"	# The patient comments is expected to have the surgeon name and more information\n"
"	# about the operation being performed.\n"
"	# https://dicom.innolitics.com/ciods/mr-image/patient/00104000\n"
"	comments = \"\"\n"
"\n"
"# Information about the FGS contrast agent\n"
"#\n"
"# If any of these items are unknown, they can be commented out to omit them.\n"
"[contrast]\n"
"	# https://dicom.innolitics.com/ciods/mr-image/contrast-bolus/00180010\n"
"	agent = \"\"\n"
"	\n"
"	# https://dicom.innolitics.com/ciods/mr-image/contrast-bolus/00181041\n"
"	milliliters = 0\n"
"	\n"
"	# This will be treated as a generalized time, and be used for both \n"
"	# the start and end time of the injection process.\n"
"	# https://dicom.innolitics.com/ciods/mr-image/contrast-bolus/00181042\n"
"	# https://dicom.innolitics.com/ciods/mr-image/contrast-bolus/00181043\n"
"	time = \"\"\n"
"\n"
"# Clerical information about the surgery\n"
"[surgery]\n"
"\n"
"	# This will be used for both study and series timestamps,\n"
"	# https://dicom.innolitics.com/ciods/segmentation/general-series/00080021\n"
"	# https://dicom.innolitics.com/ciods/segmentation/general-study/00080020\n"
"	date = \"\"\n"
"	\n"
"	# https://dicom.innolitics.com/ciods/segmentation/general-series/00080031\n"
"	time = \"\"\n"
"	\n"
"	# https://dicom.innolitics.com/ciods/segmentation/general-study/00200010\n"
"	study_id = \"\"\n"
"	\n"
"	study_descr = \"\"\n"
;

const char* GetSessionTOMLString(){ return szSessionTomlString;}
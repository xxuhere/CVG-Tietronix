
template_data = ""
print("Opening Session_toml_template.txt for reading")
with open('Session_toml_template.txt', 'r') as template_file:
    template_data = template_file.read()
    
print("\n")
print("Converting content:")
print("Converting content:" + template_data)

print("Opening output file Session_Toml.h for writing")
with open('Session_Toml.h', 'w') as generated_file:
    generated_file.write(" // This file was automatically generated, DO NOT EDIT!\n")
    generated_file.write(" // Instead, see contents of {REPO_ROOT}/_SrcAssets/TOML_Template\n")
    generated_file.write("#pragma once\n")
    generated_file.write("\n\n")
    generated_file.write("const char* GetSessionTOMLString();")

print("Completed .h\n")

print("Opening output file Session_Toml.cpp for writing")
with open('Session_Toml.cpp', 'w') as generated_file:
    generated_file.write(" // This file was automatically generated, DO NOT EDIT!\n")
    generated_file.write(" // Instead, see contents of {REPO_ROOT}/_SrcAssets/TOML_Template\n")
    generated_file.write("\n\n")
    generated_file.write("const char* szSessionTomlString = \n")
    
    template_lines = template_data.split('\n')
    for tline in template_lines:
        # TODO: Escape here if needed
        tline = tline.replace("\"", "\\\"") 
        tline = "\"" + tline + "\\n\"\n"
        generated_file.write(tline)
    
    generated_file.write(";")
    generated_file.write("\n\n")
    generated_file.write("const char* GetSessionTOMLString(){ return szSessionTomlString;}")
    
print("Completed .cpp\n")
    
print("FINISHED!\n")
        

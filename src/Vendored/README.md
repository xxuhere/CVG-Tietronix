# Vendored Libraries

Vendored libraries are versions of the library that are contained
within the repo. This means they are version controlled with the 
rest of the main code instead of their most up-to-date version.

# Libraries

All libraries mentioned shall have their official retrieval location
listed here, regardless of if its latest version is the same that's
currently vendored.

Only relevant files need to be included (not the entire repository).
This includes the library version's README file and LICENSE file.

## glm

https://github.com/g-truc/glm
**Name:** OpenGL Mathematics
**Version:** 0.9.9.8
**License:** MIT License
**Justification:**
A vector math library designed for use with OpenGL.

Currently unused. It is currently being kept in case 3D math is required
for future features.

## lodePNG

https://github.com/lvandeve/lodepng
**Name:** LodePNG
**Version** 
**License:** zLib
**Justification:** 
A PNG library. While OpenCV also supports PNGs, LodePNG tends to be more
robust. LodePNG is preferred when loading PNG for GUI assets.

## json

https://github.com/nlohmann/json
**Name:** JSON for modern C++
**Version:** Archived zip, 3.10.4
**License:** MIT License
**Justification:** 
The CVG system uses JSON to send and receive messages. A library is
needed to effectively perform these jobs.

Only relevant files are taken. The repo also contains demo code
and supporting files that were not vendored.

# tomlplusplus

https://github.com/marzer/tomlplusplus
**Name:** tomlplusplus
**Version: ** v3.1.0 (Single header)
**License:** MIT License
**Justification:** 
Support for TOML option files, to support configuration files that can have 
comments and are more human readable than JSON.

The single header version is used, and only the single header and license
files are vendored.

# Dependencies

These libraries are not expected to have any library dependencies.

Anything Vendored shall compile with a C++ 17 compliant compiler.

Anything Vendored shall be portable enough to work on all
development and deployment platforms.
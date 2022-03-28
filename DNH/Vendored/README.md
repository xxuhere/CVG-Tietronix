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

## json

https://github.com/nlohmann/json
**Name:** JSON for modern C++
**Version:** Archived zip, 3.10.4
**License:** 
**Justification:** 
The CVG system uses JSON to send and receive messages. A library is
needed to effectively perform these jobs.

Only relevant files are taken. The repo also contains demo code
and supporting files that were not vendored.

## Simple-Web-Server

https://github.com/eidheim/Simple-Web-Server
**Name:** Simple Web Server
**Version:** Archived zip, v2.1.1
**License:** MIT
**Justification:** 
The library is used to host HTTP content, specifically for the REST
API.

Only relevant files are taken. The repo also contains demo code
and supporting files that were not vendored.

## Simple-WebSocket-Server

https://github.com/eidheim/Simple-WebSocket-Server
**Name:** Simple Web Socket Server
**Version:** Archived zip, v2.0.0-rc3
**License:** MIT
**Justification:**
The library is used to manage WebSockets functionality.

Only relevant files are taken. The repo also contains demo code
and supporting files that were not vendored.

# Dependencies

These libraries are not expected to have any library dependencies.

Anything Vendored shall compile with a C++ 14 compliant compiler.

Anything Vendored shall be portable enough to work on all
development and deployment platforms.
// Get rid of a caveat error that the code will not be supported
// on WindowsXP, Windows Server 2003, or earlier. This is because
// of socket code brought in from server_ws.hpp and (specific versions
// of) boost.
//
// Include where needed to avoid errorC4996 breaking the build.
// https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-3-c4996?view=msvc-170
#pragma warning(disable : 4996)
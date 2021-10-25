#include <string>
#include "Core/CoreDNH.h"
#include "Core/LogSys.h"
#include <chrono>
#include <ctime>

using namespace CVG;
const double TimeBetweenPings = 10.0;

int main() 
{
    LogSys::Log("Initializing core DNH Systems.");
    CoreDNH dnhSys;

    LogSys::Log("Starting DNH Network.");
    dnhSys.StartNetwork();

    LogSys::Log("Networks initialized.");

    LogSys::Log("System is up, idling in app loop.");

    std::chrono::time_point<std::chrono::system_clock> lastPing =
        std::chrono::system_clock::now();

    // Background app loop
    while (true)
    {
        Sleep(100);
        dnhSys.Tick();

        // Every so often, we need to send a ping. It's the 
        // only way to force the system to detect unexpected 
        // dropout connections.
        std::chrono::time_point<std::chrono::system_clock> curTime =
            std::chrono::system_clock::now();
        //
        double timeSincePing =
            std::chrono::duration_cast<std::chrono::seconds>(curTime - lastPing).count();
        //
        if (timeSincePing >= TimeBetweenPings)
        {
            dnhSys.Ping();
            // Reset timer
            lastPing = std::chrono::system_clock::now();
        }
    }
    
    return 0;
}
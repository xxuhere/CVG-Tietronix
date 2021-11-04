#include <string>
#include "Core/CoreDNH.h"
#include "Core/LogSys.h"
#include <boost/filesystem.hpp>
#include <chrono>
#include <ctime>

#ifdef __linux__
#include <unistd.h> // For getcwd()
#endif

using namespace CVG;
const double TimeBetweenPings = 10.0;

#define REQUIRE_CONFIG false

int main(int argc, const char* argv[])
{
    //      STARTUP MESSAGE
    //////////////////////////////////////////////////

    // Before the DNH is create, it's acceptable to use std::cout or printf
    std::cout << "==================================================" << std::endl;
    std::cout << "Cancer Vision Goggles (CVG)" << std::endl;
    std::cout << "Device Network Hub (DNH)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Build date: " << __DATE__ << " - " << __TIME__ << std::endl;
    std::cout << "" << std::endl;
    std::cout << "                                        WUSTL 2021" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;

    LogSys::Log("Initializing core DNH Systems.");
    LogSys::Log(std::string("\tExecuted as ") + argv[0]);
    CoreDNH dnhSys;

    std::string strCurrentDir;

#if _WIN32
    // This currently isn't implemented in boost 1.67 that the 
    // Linux builds are expected to use. If the Linux version of
    // Boost is upgraded to 1.68 or greater, then this branching
    // can be removed and both implementations could use Boost.
    //
    //(Boost 1.67 is used on Linux because that's the highest version
    // that could be automatically installed with apt-get)
    boost::filesystem::path pathCWD = boost::filesystem::current_path().generic_string();
    strCurrentDir = pathCWD.string();
#elif __linux__
    #define CWDBUFFERSIZE 1024
    { 
        // It's put in a scope to explicitly help it not continue 
        // using function stack space after its temporary use.
        char szCwdBuffer[CWDBUFFERSIZE];
        if (getcwd(szCwdBuffer, CWDBUFFERSIZE) == nullptr)
        {
            LogSys::Log("\tEncountered error when querying current directory.");
            strCurrentDir = "";
        }
        else
            strCurrentDir = szCwdBuffer;
    }
#else
#error "UNSUPPORTED PLATFORM: No implementation to query current working directory found."
#endif
    LogSys::Log("\tStarting in directory " + strCurrentDir);

    //      CONFIGURATION
    //////////////////////////////////////////////////

    // The DNH can run different configurations for different
    // tasks or 
    std::string configPath = "config.json";
    for (int i = 0; i < argc - 1; ++i)
    {
        if (strcmp(argv[i], "--config") == 0)
        {
            configPath = argv[i + 1];
            LogSys::Log(std::string("Changing config file to ") + configPath);
        }
    }

    std::ifstream configFile(configPath);
    if (!configFile.is_open())
        LogSys::Log(std::string("Config file ") + configPath + " not found; skipping config load.");
    else
    {
        LogSys::Log(std::string("Found config ") + configPath + ", initializing.");
        LogSys::Log("\tLoading file");

        try
        {
            // https://stackoverflow.com/a/2602258/2680066
            std::stringstream buffer;
            buffer << configFile.rdbuf();

            json jsConfig = json::parse(buffer.str());

            dnhSys.ProcessConfigurationJSON(jsConfig);
        }
        catch (const std::exception & ex)
        {
            LogSys::Log(std::string("Error processing config : ") + ex.what());
            if (REQUIRE_CONFIG)
            {
                LogSys::Log("This build of DNH require a config. Aborting session.");
                abort();
            }
        }

        configFile.close();
    }

    //      NETWORK INIT
    //////////////////////////////////////////////////
    LogSys::Log("Starting DNH Network...");
    dnhSys.StartNetwork();
    LogSys::Log("\tNetworks initialized.");
    
    //      MISC POST-NETWORK INITS
    //////////////////////////////////////////////////
    LogSys::Log("Finalizing initialization...");
    dnhSys.FinalizeInitialization();
    LogSys::Log("System is up, idling in app loop\n--------------------------------------------------");

    //      APPLICATION LOOP
    //////////////////////////////////////////////////
    std::chrono::time_point<std::chrono::system_clock> lastPing =
        std::chrono::system_clock::now();

    // Background app loop
    while (!dnhSys.IsShutdown())
    {
#ifdef __linux__
        sleep(100);
#elif _WIN32
        Sleep(100);
#else
        #error "UNSUPPORTED PLATFORM: No sleep implementation found or used."
#endif
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

    dnhSys.FinalizeShutdown();
    
    return 0;
}
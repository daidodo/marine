#include <configuration.hh>
#include <logger.hh>

using namespace marine;
using namespace reaper;

static int g_clients;

static bool init(const char * exe)
{
    //read conf
    CConfiguration config;
    if(!config.load("logger.conf")){
        FATAL("cannot open 'logger.conf'");
        return false;
    }
    //init logger
    std::string level = config.getString("LEVEL");
    if(!LOGGER_INIT(std::string(exe) + ".log", level, 10 << 20, 5)){
        FATAL("cannot init logger");
        return false;
    }
    //other
    g_clients = config.getInt("CLIENTS", 1);
    return true;
}


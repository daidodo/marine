#include "logger.h"

void func()
{
    using namespace im_logger;
    LOGGER_INIT("1.log", "INFO");

    METHOD("method");
    TRACE("trace");
    DEBUG("debug");
    INFO("info");
    NOTICE("notice");
    WARN("warn");
    ERROR("error");
    EVENT("event");
    FATAL("fatal");

    LOGGER_LOG(kInfo, __FILE__, __LINE__, __FUNCTION__, "tag", "this is msg with tag");
}

int main()
{
    func();
    return 0;
}

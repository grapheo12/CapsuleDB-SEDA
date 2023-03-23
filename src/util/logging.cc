#include <iostream>
#include "logging.hpp"
#include "../dc_config.h"

namespace Logger
{
    void log(LogLevel level, const std::string &message)
    {
        switch (level)
        {
        case DEBUG:
            if (!DEBUG_MODE)
                return;
            std::cerr << "[ DEBUG ] ";
            std::cerr << message << std::endl;
            break;
        case INFO:
            std::cout << "[ INFO ] ";
            std::cout << message << std::endl;
            break;
            break;
        case WARNING:
            std::cerr << "[ WARNING ] ";
            std::cerr << message << std::endl;
            break;
            break;
        case ERROR:
            std::cout << "[ ERROR ] ";
            std::cout << message << std::endl;
            break;
        default:
            return;
        }
    }
}

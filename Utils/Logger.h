#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H

#include <string>

// Provides simple console logging for application messages.
class Logger {
public:
    Logger();

    void log(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
};

#endif

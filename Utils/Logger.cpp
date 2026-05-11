#include "Logger.h"
#include <iostream>

Logger::Logger() {
    // TODO: Add initialization logic if needed.
}

void Logger::log(const std::string& message) {
    // Output informational message.
    std::cout << "[LOG]   " << message << std::endl;
}

void Logger::warn(const std::string& message) {
    // Output warning message.
    std::cout << "[WARN]  " << message << std::endl;
}

void Logger::error(const std::string& message) {
    // Output error message.
    std::cout << "[ERROR] " << message << std::endl;
}

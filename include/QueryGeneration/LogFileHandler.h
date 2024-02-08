#ifndef LOG_FILE_HANDLER_H
#define LOG_FILE_HANDLER_H

#include <string>
#include <fstream>
#include <chrono>
#include <iostream>

namespace driver{
    class LogFileHandler {
    private:
        static std::ofstream logFile;
        static std::chrono::high_resolution_clock::time_point startTime;
        static std::chrono::high_resolution_clock::time_point stopTime;

    public:
        static bool createLogFile(const std::string& fileName);
        static bool log(const std::string& fileName, const std::string& message);
        static bool clearLogFile(const std::string& fileName);
        static bool closeLogFile(const std::string& fileName);
    };
}

#endif // LOG_FILE_HANDLER_H

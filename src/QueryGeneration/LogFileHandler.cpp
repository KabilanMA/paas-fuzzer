#include "LogFileHandler.h"

namespace driver {
    std::ofstream LogFileHandler::logFile;
    std::chrono::high_resolution_clock::time_point LogFileHandler::startTime;
    std::chrono::high_resolution_clock::time_point LogFileHandler::stopTime;

    bool LogFileHandler::createLogFile(const std::string& fileName) {
        std::ofstream logfile(fileName);
        if (logfile.is_open()) {
            return true;
        } else {
            std::cerr << "Error: Could not create file." << std::endl;
            return false;
        }
    }

    bool LogFileHandler::log(const std::string& fileName, const std::string& message) {
        std::ofstream logFile(fileName, std::ios_base::app);
        if (logFile.is_open()) {
            logFile << message << std::endl;
            return true;
        } else {
            std::cerr << "Error: Log file is not open." << std::endl;
            return false;
        }
    }

    bool LogFileHandler::clearLogFile(const std::string& fileName) {
        std::ofstream logFile(fileName, std::ios::out | std::ios::trunc);
        if (logFile.is_open()) {
            logFile.close();
            return true;
        } else {
            std::cerr << "Error: Could not clear log file." << std::endl;
            return false;
        }
    }

    bool LogFileHandler::closeLogFile(const std::string& fileName) {
        // Log file is closed automatically when ofstream object goes out of scope
        return true;
    }
}

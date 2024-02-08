#ifndef QUERY_EXECUTOR_H
#define QUERY_EXECUTOR_H

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <thread>
#include <chrono>

#include "DatabaseHandler.h"
#include "QueryGenerator.h"
#include "LogFileHandler.h"
#include "QueryFileHandler.h"

namespace driver {
    class QueryExecutor {
    public:
        static void setupAndExecuteQueries();
        static void executeQueries(const std::string& queryFileName, driver::DatabaseHandler& dbHandler);
        static std::map<std::string, std::string> readCredentials(const std::string& filename);
    };
}

#endif // QUERY_EXECUTOR_H

#ifndef QUERY_EXECUTOR_H
#define QUERY_EXECUTOR_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <filesystem>

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
        
        static void processData(const std::string& selectQuery, const std::string& outputCsvFilename, const std::string& queriesFile, const std::string& executionMessage, DatabaseHandler& dbHandler);
        static void compareCSVFiles(const std::string& localFilePath, const std::string& clusterFilePath, const std::string& outputFilePath);
        static void writeToCSV(const std::vector<std::vector<std::string>> data, const std::string filename);

        static void generateQueries(const std::string& queryFileName);
        
        //static sql::ResultSet* executeSelectQuery(sql::Connection *con, std::string query);

    };
}

#endif // QUERY_EXECUTOR_H
#include "QueryExecutor.h"

namespace driver {
    // Function to read credentials from the file
    std::map<std::string, std::string> QueryExecutor::readCredentials(const std::string& filename) {
        std::map<std::string, std::string> credentials;
        std::ifstream file(filename);

        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string key, value;
                if (iss >> key >> value) {
                    credentials[key] = value;
                }
            }
            file.close();
        }

        return credentials;
    }

    // Function to execute queries
    void QueryExecutor::executeQueries(const std::string& queryFileName,driver::DatabaseHandler& dbHandler, const std::string& connection) {

        std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait for 5 seconds
        //dbHandler.enableQueryProfiling();
        dbHandler.executeQueriesFromFile(queryFileName);  // Read and execute queries from the file
        double totalExecutionTime = dbHandler.getTotalExecutionTime();

        // Find the position of the last '/'
        std::string countStr = "";
        size_t lastSlashPos = queryFileName.find_last_of('/');
        if (lastSlashPos != std::string::npos) {
            // Extract the substring after the last '/'
            countStr = queryFileName.substr(lastSlashPos + 1);

            // Find the positions of underscores
            size_t underscorePos = countStr.find('_');
            if (underscorePos != std::string::npos) {
                // Extract the substring after the first underscore
                countStr = countStr.substr(underscorePos + 1);

                // Find the position of the dot (.)
                size_t dotPos = countStr.find('.');
                if (dotPos != std::string::npos) {
                    // Extract the substring before the dot
                    countStr = countStr.substr(0, dotPos);
                }
            }
        }

        std::string logFileName = "output/output_" + countStr + "/log_" + countStr + ".txt";

        //driver::LogFileHandler::createLogFile(logFileName);
        driver::QueryFileHandler::createQueryFile(logFileName);
        driver::LogFileHandler::log(logFileName, "Total execution time in " + connection + " : " + std::to_string(totalExecutionTime) + " seconds \n");
        driver::LogFileHandler::closeLogFile(logFileName);
    }

    void QueryExecutor::generateQueries(const std::string& queryFileName){
        std::vector<std::string> queries;
        driver::QueryFileHandler::createQueryFile(queryFileName);

        std::string dbName = "mydatabase";
        queries.push_back("SET profiling = 1");
        queries.push_back(driver::QueryGenerator::generateDropDatabaseQuery(dbName));

        driver::QueryFileHandler::clearQueryFile(queryFileName);
        driver::QueryFileHandler::createQueryFile(queryFileName);

        // Generate queries and add them to the vector
        queries.push_back(driver::QueryGenerator::generateCreateDatabaseQuery(dbName));
        queries.push_back(driver::QueryGenerator::generateUseDatabaseQuery(dbName));

        for (int j = 0; j < 5; ++j) {
            std::string tableName = "table_" + std::to_string(j);
            queries.push_back(driver::QueryGenerator::generateCreateTableQuery(tableName));

            for (int k = 0; k < 5; ++k) {
                int randomId = rand() % 1000;
                std::string randomName = "Name_" + std::to_string(randomId);
                queries.push_back(driver::QueryGenerator::generateInsertQuery(tableName, randomId, randomName));
            }

            // Add SELECT, WHERE, ORDER BY, and JOIN queries
            // std::vector<std::string> columnsToSelect = {"field1", "field2", "field3"};
            // queries.push_back(driver::QueryGenerator::generateSelectQuery(tableName, columnsToSelect));

            // std::string whereCondition = "field2 > 50";
            // queries.push_back(driver::QueryGenerator::generateWhereQuery(tableName, whereCondition));

            // std::string orderByColumn = "field3 DESC";
            // queries.push_back(driver::QueryGenerator::generateOrderByQuery(tableName, orderByColumn));

            // if (j != 0){
            //     std::string joinCondition = "table_0.id = table_" + std::to_string(j) + ".id";
            //     queries.push_back(driver::QueryGenerator::generateJoinQuery("table_0", tableName, joinCondition));
            // }
        }

        // Write the queries to the file
        driver::QueryFileHandler::writeQueriesToFile(queryFileName, queries);
    }

    void QueryExecutor::processData(const std::string &selectQuery, const std::string &outputCsvFilename, const std::string &queriesFile, const std::string& connection, const std::string &executionMessage, driver::DatabaseHandler& dbHandler) {
        
        driver::QueryFileHandler::createQueryFile(outputCsvFilename);
        driver::QueryExecutor::executeQueries(queriesFile, dbHandler, connection);
        std::cout << executionMessage << std::endl;
        std::ofstream csvFile(outputCsvFilename);

        for (int a = 0; a < 5; a++) {
            std::string query = selectQuery + std::to_string(a); // Create a new query for each table
            sql::Connection* connection = dbHandler.getConnection();
            sql::Statement *stmt = connection->createStatement();
            sql::ResultSet *res = stmt->executeQuery(query);

            int columnCount = res->getMetaData()->getColumnCount();

            csvFile << "Table_" << a << " results" << std::endl; // Heading for table results

            while (res->next()) {
                for (int i = 1; i <= columnCount; ++i) {
                    csvFile << res->getString(i);
                    if (i < columnCount) {
                        csvFile << ",";
                    }
                }
                csvFile << std::endl;
            }

            delete res; // Delete the ResultSet after usage
            delete stmt; // Delete the Statement after usage

            csvFile << std::endl; // Add a blank line to separate tables visually
        }

        csvFile.close();
    }


    void QueryExecutor::compareCSVFiles(const std::string &localFilePath, const std::string &clusterFilePath, const std::string &outputFilePath) {
        std::ifstream localFile(localFilePath);
        std::ifstream clusterFile(clusterFilePath);
        std::ofstream outputFile(outputFilePath);

        if (!localFile.is_open() || !clusterFile.is_open() || !outputFile.is_open()) {
            std::cerr << "Error opening files." << std::endl;
            return;
        }

        std::string localLine, clusterLine;
        std::vector<std::string> localValues, clusterValues;

        // Create output folder
        std::string outputFolder = "output";
        std::filesystem::create_directory(outputFolder);

        // Read and compare each line
        int lineNumber = 1;
        while (getline(localFile, localLine) && getline(clusterFile, clusterLine)) {
            localValues.clear();
            clusterValues.clear();

            // Tokenize lines based on comma
            std::istringstream localStream(localLine);
            std::string localToken;
            while (getline(localStream, localToken, ',')) {
                localValues.push_back(localToken);
            }

            std::istringstream clusterStream(clusterLine);
            std::string clusterToken;
            while (getline(clusterStream, clusterToken, ',')) {
                clusterValues.push_back(clusterToken);
            }

            // Compare values
            bool match = true;
            for (size_t i = 0; i < localValues.size(); ++i) {
                if (localValues[i] != clusterValues[i]) {
                    match = false;
                    break;
                }
            }

            // If values don't match, save the lines to the output file
            if (!match) {
                outputFile << "Line " << lineNumber << " - Local: " << localLine << std::endl;
                outputFile << "Line " << lineNumber << " - Cluster: " << clusterLine << std::endl;
                outputFile << "----------------------------------" << std::endl;
            }

            lineNumber++;
        }

        localFile.close();
        clusterFile.close();
        outputFile.close();
    }
}

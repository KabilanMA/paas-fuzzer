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
    void QueryExecutor::executeQueries(const std::string& queryFileName,driver::DatabaseHandler& dbHandler) {
        // std::vector<std::string> queries;

        // std::string dbName = "mydatabase";
        // queries.push_back("SET profiling = 1");
        // queries.push_back(driver::QueryGenerator::generateDropDatabaseQuery(dbName));

        // driver::QueryFileHandler::clearQueryFile(queryFileName);
        // driver::QueryFileHandler::createQueryFile(queryFileName);

        // // Generate queries and add them to the vector
        // queries.push_back(driver::QueryGenerator::generateCreateDatabaseQuery(dbName));
        // queries.push_back(driver::QueryGenerator::generateUseDatabaseQuery(dbName));

        // for (int j = 0; j < 5; ++j) {
        //     std::string tableName = "table_" + std::to_string(j);
        //     queries.push_back(driver::QueryGenerator::generateCreateTableQuery(tableName));

        //     for (int k = 0; k < 5; ++k) {
        //         int randomId = rand() % 1000;
        //         std::string randomName = "Name_" + std::to_string(randomId);
        //         queries.push_back(driver::QueryGenerator::generateInsertQuery(tableName, randomId, randomName));
        //     }

        //     // Add SELECT, WHERE, ORDER BY, and JOIN queries
        //     // std::vector<std::string> columnsToSelect = {"field1", "field2", "field3"};
        //     // queries.push_back(driver::QueryGenerator::generateSelectQuery(tableName, columnsToSelect));

        //     // std::string whereCondition = "field2 > 50";
        //     // queries.push_back(driver::QueryGenerator::generateWhereQuery(tableName, whereCondition));

        //     // std::string orderByColumn = "field3 DESC";
        //     // queries.push_back(driver::QueryGenerator::generateOrderByQuery(tableName, orderByColumn));

        //     // if (j != 0){
        //     //     std::string joinCondition = "table_0.id = table_" + std::to_string(j) + ".id";
        //     //     queries.push_back(driver::QueryGenerator::generateJoinQuery("table_0", tableName, joinCondition));
        //     // }
        // }

        // // Write the queries to the file
        // driver::QueryFileHandler::writeQueriesToFile(queryFileName, queries);

        std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait for 5 seconds
        //dbHandler.enableQueryProfiling();
        dbHandler.executeQueriesFromFile(queryFileName);  // Read and execute queries from the file
        double totalExecutionTime = dbHandler.getTotalExecutionTime();

        driver::LogFileHandler::log("log.txt", "Total execution time: " + std::to_string(totalExecutionTime) + " seconds \n");
        driver::LogFileHandler::closeLogFile("log.txt");
    }

    void QueryExecutor::generateQueries(const std::string& queryFileName){
        std::vector<std::string> queries;

        driver::LogFileHandler::createLogFile("log.txt");
        //std::string queryFileName = "output/queries.txt";
        driver::QueryFileHandler::clearQueryFile(queryFileName);
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

    void QueryExecutor::processData(const std::string &selectQuery, const std::string &outputCsvFilename, const std::string &queriesFile, const std::string &executionMessage, driver::DatabaseHandler& dbHandler) {

        // driver::LogFileHandler::createLogFile("log.txt");
        // //std::string queryFileName = "output/queries.txt";
        // driver::QueryFileHandler::clearQueryFile(queriesFile);
        // driver::QueryFileHandler::createQueryFile(queriesFile);

        driver::QueryExecutor::executeQueries(queriesFile, dbHandler);

        // Select data from table
        std::vector<std::vector<std::string>> result;


        // Execute selectQuery
        sql::Connection* connection = dbHandler.getConnection();
        sql::Statement *stmt = connection->createStatement();
        //sql::ResultSet *res = QueryExecutor::executeSelectQuery(connection, selectQuery); 
        sql::ResultSet *res = stmt->executeQuery(selectQuery); // Use executeQuery instead of execute
        delete stmt;

        int columnCount = res->getMetaData()->getColumnCount();

        while (res->next()) {
            std::vector<std::string> row;
            for (int i = 1; i <= columnCount; ++i) {
                row.push_back(res->getString(i));
            }
            result.push_back(row);
            // for (const auto& row : result) {
            //     for (const auto& value : row) {
            //         std::cout << value << " ";
            //     }
            // }
        }

        // delete res;
        // delete stmt;

        // Write result to CSV file
        //writeToCSV(result, outputCsvFilename);

        std::ofstream csvFile(outputCsvFilename);

        for (const auto &row : result) {
            for (size_t i = 0; i < row.size(); ++i) {
                csvFile << row[i];
                if (i < row.size() - 1) {
                    csvFile << ",";
                }
            }
            csvFile << std::endl;
        }

        csvFile.close();
    }

    // sql::ResultSet* QueryExecutor::executeSelectQuery(sql::Connection *con, const std::string &query) {
    //     try {
    //         sql::Statement *stmt = con->createStatement();
    //         sql::ResultSet *res = stmt->executeQuery(query); // Use executeQuery instead of execute
    //         delete stmt;
    //         return res; // Return the result set
    //     } catch (sql::SQLException &e) {
    //         std::cerr << "Error executing query: " << query << "\nSQL Exception: " << e.what() << std::endl;
    //         throw; // Re-throw the exception to be caught in the main function
    //     }
    // }

    // void writeToCSV(const std::vector<std::vector<std::string>> &data, const std::string &filename) {
    //     std::ofstream csvFile(filename);

    //     for (const auto &row : data) {
    //         for (size_t i = 0; i < row.size(); ++i) {
    //             csvFile << row[i];
    //             if (i < row.size() - 1) {
    //                 csvFile << ",";
    //             }
    //         }
    //         csvFile << std::endl;
    //     }

    //     csvFile.close();
    // }


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


    // // Function to set up the database and execute queries
    // void setupAndExecuteQueries() {
    //     //Read credentials from the file
    //     std::map<std::string, std::string> credentials = driver::QueryExecutor::readCredentials("credentials.txt");

    //     // Check if all required credentials are present
    //     if (credentials.find("host") == credentials.end() ||
    //         credentials.find("port") == credentials.end() ||
    //         credentials.find("user") == credentials.end() ||
    //         credentials.find("password") == credentials.end()) {
    //         std::cerr << "Invalid or incomplete credentials in the file." << std::endl;
    //         return;
    //     }

    //     const std::string host = credentials["host"];
    //     const std::string port = credentials["port"];
    //     const std::string user = credentials["user"];
    //     const std::string password = credentials["password"];

    //     driver::DatabaseHandler dbHandler(host, port, user, password);

    //     if (dbHandler.connect()) {
    //         driver::LogFileHandler::clearLogFile("output/log.txt");
    //         driver::QueryFileHandler::clearQueryFile("output/queries.txt");

    //         if (driver::LogFileHandler::createLogFile("log.txt")) {
    //             std::string queryFileName = "output/queries.txt";
    //             driver::QueryFileHandler::clearQueryFile(queryFileName);
    //             driver::QueryFileHandler::createQueryFile(queryFileName);

    //             // Execute queries
    //             driver::QueryExecutor::executeQueries(queryFileName, dbHandler);
    //         }
    //     }
    // }
}

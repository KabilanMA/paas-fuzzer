#include "QueryFileHandler.h"

namespace driver {

    bool QueryFileHandler::createQueryFile(const std::string& fileName) {
        std::ofstream queryFile(fileName);
        if (queryFile.is_open()) {
            queryFile.close();
            return true;
        } else {
            std::cerr << "Error: Could not create query file." << std::endl;
            return false;
        }
    }

    bool QueryFileHandler::clearQueryFile(const std::string& fileName) {

        std::ofstream queryFile(fileName, std::ios::out | std::ios::trunc);
        if (queryFile.is_open()) {
            queryFile.close();
            return true;
        } else {
            std::cerr << "Error: Could not clear query file." << std::endl;
            return false;
        }
    }

    bool QueryFileHandler::writeQueriesToFile(const std::string& fileName, const std::vector<std::string>& queries) {

        std::ofstream queryFile(fileName, std::ios::app);  // Open in append mode
        if (queryFile.is_open()) {
            for (const std::string& query : queries) {
                queryFile << query << std::endl;
            }
            queryFile.close();
            std::cout << "Queries written to file successfully." << std::endl;
            return true;
        } else {
            std::cerr << "Error: Could not write queries to file." << std::endl;
            return false;
        }
    }

    bool QueryFileHandler::executeQueriesFromFile(const std::vector<std::string>& queries, sql::Connection* connection) {

        try {
            sql::Statement* statement = connection->createStatement();

            for (const auto& query : queries) {
                statement->execute(query);

                // Ensure any remaining result sets are closed
                do {
                    sql::ResultSet *res = statement->getResultSet();
                    if (res != nullptr) {
                        res->close();
                    }
                } while (statement->getMoreResults());
            }

            delete statement;
            return true;
        } catch (sql::SQLException& e) {
            std::cerr << "SQL Exception: " << e.what() << std::endl;
            std::cerr << "MySQL error code: " << e.getErrorCode() << std::endl;
            std::cerr << "SQL State: " << e.getSQLState() << std::endl;

            std::cout << "Failed to execute queries." << std::endl;
            return false;
        }
    }
}
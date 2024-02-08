#include "DatabaseHandler.h"

namespace driver {

    DatabaseHandler::DatabaseHandler(const std::string& host, const std::string& port, const std::string& user, const std::string& password)
        : driver(nullptr), con(nullptr), host_(host), port_(port), user_(user), password_(password) {}

    DatabaseHandler::~DatabaseHandler() {
        delete con;
    }

    bool DatabaseHandler::connect() {
        try {
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect("tcp://" + host_ + ":" + port_ , user_, password_);
            std::cout << "Connection successful!" << std::endl;
            return true;
        } catch (sql::SQLException& e) {
            std::cerr << "SQL Exception: " << e.what() << std::endl;
            std::cerr << "MySQL error code: " << e.getErrorCode() << std::endl;
            std::cerr << "SQL State: " << e.getSQLState() << std::endl;

            std::cout << "Connection unsuccessful!" << std::endl;
            return false;
        }
    }

    sql::Connection* DatabaseHandler::getConnection() const {
        return con;
    }

    void DatabaseHandler::executeQueriesFromFile(const std::string& fileName) {
        std::ifstream queryFile(fileName);
        if (queryFile.is_open()) {
            std::vector<std::string> queries;
            std::string query;

            while (std::getline(queryFile, query)) {
                queries.push_back(query);
            }

            queryFile.close();

            // Execute queries using QueryFileHandler
            if (QueryFileHandler::executeQueriesFromFile(queries, con)) {
                std::cout << "All queries executed successfully." << std::endl;
            } else {
                std::cerr << "Some queries failed to execute." << std::endl;
            }
        } else {
            std::cerr << "Error: Could not open query file." << std::endl;
        }
    }

    void DatabaseHandler::enableQueryProfiling() {
        sql::Statement *stmt = con->createStatement();
        if (stmt) {
            try {
                stmt->execute("SET profiling = 1");
            } catch (sql::SQLException& e) {
                std::cerr << "Error setting query profiling: " << e.what() << std::endl;
                std::cerr << "MySQL error code: " << e.getErrorCode() << std::endl;
                std::cerr << "SQL State: " << e.getSQLState() << std::endl;
            }

            delete stmt;
        } else {
            std::cerr << "Error creating statement for enabling query profiling." << std::endl;
        }
    } 

    double DatabaseHandler::getTotalExecutionTime() {
        double totalExecutionTime = 0;

        // Show profiles to get the duration of each query
        sql::Statement *stmt = con->createStatement();
        if (stmt) {
            try {
                sql::ResultSet *profiles = stmt->executeQuery("SHOW PROFILES");

                while (profiles->next()) {
                    totalExecutionTime += profiles->getDouble("Duration");
                }

                delete profiles;
            } catch (sql::SQLException& e) {
                std::cerr << "Error executing SHOW PROFILES: " << e.what() << std::endl;
                std::cerr << "MySQL error code: " << e.getErrorCode() << std::endl;
                std::cerr << "SQL State: " << e.getSQLState() << std::endl;
            }

            delete stmt;
        } else {
            std::cerr << "Error creating statement for SHOW PROFILES." << std::endl;
        }

        return totalExecutionTime;
    }   
}
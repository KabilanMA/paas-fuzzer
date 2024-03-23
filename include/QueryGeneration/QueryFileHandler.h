#ifndef QUERY_FILE_HANDLER_H
#define QUERY_FILE_HANDLER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread> 

#include <mysql_driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

namespace driver {
    class QueryFileHandler {
    public:
        static bool createQueryFile(const std::string& fileName);
        static bool clearQueryFile(const std::string& fileName);
        static bool writeQueriesToFile(const std::string& fileName, const std::vector<std::string>& queries);
        static bool executeQueriesFromFile(const std::vector<std::string>& queries, sql::Connection* connection);
    };
}

#endif // QUERY_FILE_HANDLER_H
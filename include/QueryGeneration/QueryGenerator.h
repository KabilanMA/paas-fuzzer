#ifndef QUERY_GENERATOR_H
#define QUERY_GENERATOR_H

#include <string>
#include <vector>

namespace  driver{

    class QueryGenerator {
    public:
        static std::string generateDropDatabaseQuery(const std::string& dbName);
        static std::string generateCreateTableQuery(const std::string& tableName);
        static std::string generateInsertQuery(const std::string& tableName, int id, const std::string& name);
        static std::string generateSelectQuery(const std::string& tableName, const std::vector<std::string>& columns = {});
        static std::string generateWhereQuery(const std::string& tableName, const std::string& condition);
        static std::string generateOrderByQuery(const std::string& tableName, const std::string& orderByColumn);
        static std::string generateJoinQuery(const std::string& table1, const std::string& table2, const std::string& onCondition);
        static std::string generateAdvancedQuery(const std::string& tableName);
        static std::string generateCreateDatabaseQuery(const std::string& dbName);
        static std::string generateUseDatabaseQuery(const std::string& dbName);
    };
}
#endif // QUERY_GENERATOR_H


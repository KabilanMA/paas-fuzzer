#include "QueryGenerator.h"

namespace  driver{
    std::string QueryGenerator::generateDropDatabaseQuery(const std::string& dbName) {
        return "DROP DATABASE IF EXISTS " + dbName + ";";
    }

    std::string QueryGenerator::generateCreateTableQuery(const std::string& tableName) {
        return "CREATE TABLE IF NOT EXISTS " + tableName + " ("
            "id INT PRIMARY KEY AUTO_INCREMENT, "
            "field1 VARCHAR(255), "
            "field2 INT, "
            "field3 DOUBLE, "
            "field4 DATE, "
            "field5 BOOLEAN, "
            "field6 TEXT);";
    }

    std::string QueryGenerator::generateInsertQuery(const std::string& tableName, int id, const std::string& name) {
        return "INSERT INTO " + tableName + " (id, field1, field2, field3, field4, field5, field6) "
            "VALUES (" + std::to_string(id) + ", 'Name_" + std::to_string(id) + "', "
            + std::to_string(rand() % 100) + ", " + std::to_string((double)rand() / RAND_MAX * 100.0) + ", "
            "'2022-10-01', " + std::to_string(rand() % 2) + ", 'SomeText');";
    }

    std::string QueryGenerator::generateAdvancedQuery(const std::string& tableName) {
        return "UPDATE " + tableName + " SET field1 = 'NewName' WHERE id = 1;";
    }

    std::string QueryGenerator::generateCreateDatabaseQuery(const std::string& dbName) {
        return "CREATE DATABASE IF NOT EXISTS " + dbName + ";";
    }

    std::string QueryGenerator::generateUseDatabaseQuery(const std::string& dbName) {
        return "USE " + dbName + ";";
    } 

    std::string QueryGenerator::generateSelectQuery(const std::string& tableName, const std::vector<std::string>& columns) {
        std::string columnList;
        for (const auto& column : columns) {
            if (!columnList.empty()) {
                columnList += ", ";
            }
            columnList += column;
        }

        return "SELECT " + (columnList.empty() ? "*" : columnList) + " FROM " + tableName + ";";
    }

    std::string QueryGenerator::generateWhereQuery(const std::string& tableName, const std::string& condition) {
        return "SELECT * FROM " + tableName + " WHERE " + condition + ";";
    }

    std::string QueryGenerator::generateOrderByQuery(const std::string& tableName, const std::string& orderByColumn) {
        return "SELECT * FROM " + tableName + " ORDER BY " + orderByColumn + ";";
    }

    std::string QueryGenerator::generateJoinQuery(const std::string& table1, const std::string& table2, const std::string& onCondition) {
        return "SELECT * FROM " + table1 + " INNER JOIN " + table2 + " ON " + onCondition + ";";
    }
}
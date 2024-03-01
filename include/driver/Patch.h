#pragma once

#include <iostream>
#include <json/json.h>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

namespace driver
{
    class Patch
    {
    public:
        static int id;

        Patch(std::vector<std::string> keys, double score, std::string value);

        // getters
        std::vector<std::string> getKeys() const;
        double getScore() const;
        std::string getValue() const;
        std::string getPatchPathString() const;
        std::string getPatchValue() const;
        std::string getId() const;

        // setters
        bool setScore(double newScore);

        friend std::ostream &operator<<(std::ostream &os, const Patch &obj);

    private:
        bool isInteger(const std::string &s) const;
        std::pair<double, double> calculateStats(std::vector<double> values) const;

        // members
        std::string stringId;
        double score;
        std::string value;
        std::vector<std::string> keys;
        std::vector<double> times;
    };
}
#include "Patch.h"
#include <valarray>

namespace driver
{
    Patch::Patch(std::vector<std::string> keys, double score, std::string value) : keys(keys), score(score), value(value)
    {
        this->times = {};
    }

    std::vector<std::string> Patch::getKeys() const
    {
        return keys;
    }

    bool Patch::isInteger(const std::string &s) const
    {
        for (char c : s)
        {
            if (!std::isdigit(c))
            {
                return false;
            }
        }
        return true;
    }

    std::string Patch::getPatchValue() const
    {
        if (isInteger(value))
        {
            return value;
        }
        else
        {
            return "\"" + value + "\"";
        }
    }

    std::string Patch::getPatchPathString() const
    {
        std::vector<std::string> keys = this->getKeys();
        std::string finalValue = this->getValue();
        std::string outputPath = "";
        for (const std::string &pathKey : keys)
        {
            outputPath += ("/" + pathKey);
        }
        return outputPath;
    }

    double Patch::getScore() const
    {
        return score;
    }

    std::string Patch::getValue() const
    {
        return value;
    }

    bool Patch::setScore(double newScore)
    {
        this->times.push_back(newScore);
        auto [mean, stdDev] = this->calculateStats(this->times);
        score += ((newScore - mean) / stdDev);
        return true;
    }

    std::pair<double, double> Patch::calculateStats(std::vector<double> values) const
    {
        double sum = 0.0;
        double sumSquare = 0.0;

        for (const double &value : values)
        {
            sum += value;
            sumSquare += value * value;
        }

        double mean = sum / values.size();
        double variance = (sumSquare / values.size()) - (mean * mean);
        double stdDev = std::sqrt(variance);

        return {mean, stdDev};
    }

}

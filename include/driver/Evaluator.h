#pragma once

#include <json/json.h>
#include <iostream>
#include <regex>
#include <random>
#include <fstream>
#include <iomanip>
#include <filesystem>

#include "Patch.h"

extern "C"
{
#include <external/cJSON.h>
#include <model/object.h>
}

namespace fs = std::filesystem;

namespace driver
{
    class Evaluator
    {
    public:
        void logPatches(std::vector<Patch> &patchArray, std::string LogFileName, int suffix);
        void logPatchInfo(std::vector<Patch> &patchArray);
    };

}
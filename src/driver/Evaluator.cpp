#include "Evaluator.h"

using namespace driver;

namespace driver
{

    void writeToFile(const std::vector<Patch> &arr, const std::string &filename)
    {
        std::ofstream outfile(filename, std::ios::app);
        if (outfile.is_open())
        {
            for (Patch patch : arr)
            {
                outfile << patch.getId() << " ";
            }
            outfile << std::endl;
            outfile.close();
            std::cout << "Array appended to file successfully." << std::endl;
        }
        else
        {
            std::cout << "Unable to open file: " << filename << std::endl;
        }
    }

    void Evaluator::logPatches(std::vector<Patch> &patchArray, std::string LogFileName, int suffix)
    {
        if (LogFileName == "" || LogFileName == "empty")
        {
            LogFileName = "./Report_" + std::to_string(suffix);
        }

        // if the directory and file does not exists, create it.
        if (!fs::exists(LogFileName))
        {
            std::cout << "Created the Report Directory" << std::endl;
            fs::create_directory(LogFileName);
        }

        LogFileName += "/patchPermutation_" + std::to_string(suffix) + ".txt";
        std::ifstream infile(LogFileName);
        if (infile.good())
        {
            std::cout << "File already exists." << std::endl;
        }
        else
        {
            std::ofstream outputFile(LogFileName);
            if (outputFile.is_open())
            {
                std::cout << "Empty file created successfully." << std::endl;
                outputFile.close();
            }
            else
            {
                std::cerr << "Error: Unable to create file." << std::endl;
            }
        }

        writeToFile(patchArray, LogFileName);
    }

    void Evaluator::logPatchInfo(std::vector<Patch> &patchArray)
    {
    }
}

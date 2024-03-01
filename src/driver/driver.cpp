#include "driver.h"

#include <unordered_map>

using namespace driver;

bool compareByFloatValue(const driver::Patch &obj1, const driver::Patch &obj2)
{
    return obj1.getScore() > obj2.getScore();
}

void updatePatches(std::vector<driver::Patch> &patches, scheduler::ClusterAccess &clusterAccess, cJSON *initialConfig, Evaluator &logger, int verbose)
{
    // Use a random number for uniquen patch paermutaion file
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 9999);
    int random = distribution(gen);

    std::string logDirectory = "./Report_" + std::to_string(random);
    // if the directory and file does not exists, create it.
    if (!fs::exists(logDirectory))
    {
        std::cout << "Created the Report Directory" << std::endl;
        fs::create_directory(logDirectory);
    }

    logDirectory += "/patchList" + std::to_string(random) + ".txt";
    std::ifstream infile(logDirectory);
    if (infile.good())
    {
        std::cout << "File already exists." << std::endl;
    }
    else
    {
        std::ofstream outputFile(logDirectory);
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

    std::ofstream outfile(logDirectory, std::ios::app);
    if (outfile.is_open())
    {
        for (Patch &patch : patches)
        {
            outfile << patch.getId() << " : \n\t";
            outfile << patch << std::endl;
        }
        outfile.close();
    }
    else
    {
        std::cout << "Unable to open file: " << logDirectory << std::endl;
    }

    if (verbose > 10)
    {
        printf("-------------------------------\nAvailable patch count: %li\n", patches.size());
        printf("-------------------------------\nList of Patches\n-------------------------------\n");
        for (const auto &patch : patches)
        {
            std::cout << patch << std::endl;
        }
        printf("-------------------------------\n");
    }

    driver::JsonFileHandler jsonFH;
    apiClient_t *apiClient;
    clusterAccess.createAPI_client(&apiClient);
    // int a = 5;
    while (true)
    {
        for (driver::Patch &patch : patches)
        {
            // std::cout << patch.getValue() << std::endl;
            cJSON *previousConfig = cJSON_Parse(cJSON_Print(initialConfig));
            auto startTime = std::chrono::high_resolution_clock::now();
            clusterAccess.patch(&initialConfig, apiClient, "default", patch, verbose);
            bool isBuggyPatch = false;
            while (!clusterAccess.isPropagationComplete(initialConfig, patch, verbose))
            {
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                if (duration.count() > driver::WAIT_TIME_MILLI_SEC)
                {
                    // bug identified
                    jsonFH.reportBug(previousConfig, initialConfig, patches, patch, verbose);
                    clusterAccess.reset(previousConfig, apiClient, "default", verbose);
                    goto exitOuterLoop;
                }

                if (clusterAccess.bugExists(initialConfig, apiClient, "default", verbose))
                {
                    jsonFH.reportBug(previousConfig, initialConfig, patches, patch, verbose);
                    clusterAccess.reset(previousConfig, NULL, "default", verbose);
                    goto exitOuterLoop;
                }
            }

            if (clusterAccess.bugExists(initialConfig, apiClient, "default", verbose))
            {
                jsonFH.reportBug(previousConfig, initialConfig, patches, patch, verbose);
                clusterAccess.reset(previousConfig, NULL, "default", verbose);
                break;
            }

        exitOuterLoop:

            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            patch.setScore(duration.count());
            // std::cout << patch.getScore() << std::endl;
            std::cout << "Time duration: " << duration.count() << " milliseconds" << std::endl;
        }
        logger.logPatches(patches, "empty", random);
        std::sort(patches.begin(), patches.end(), compareByFloatValue);
        // break;
        // a--;
    }
}

void extractOptionalArgument(std::unordered_map<std::string, std::string> &arguments, int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        // Check if the argument has the format "--key=value"
        size_t pos = arg.find('=');
        if (pos != std::string::npos && pos > 2 && arg.substr(0, 2) == "--")
        {
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);

            // Store key-value pair in the map
            arguments[key] = value;
        }
    }
}

// Function to set up the database and execute queries
void setupAndExecuteQueries() {
    //Read credentials from the file for local machine
    std::map<std::string, std::string> credentials_local = driver::QueryExecutor::readCredentials("credentials_local.txt");

    // Check if all required credentials are present
    if (credentials_local.find("host") == credentials_local.end() ||
        credentials_local.find("port") == credentials_local.end() ||
        credentials_local.find("user") == credentials_local.end() ||
        credentials_local.find("password") == credentials_local.end()) {
        std::cerr << "Invalid or incomplete credentials in the file." << std::endl;
        return;
    }

    const std::string host_local = credentials_local["host"];
    const std::string port_local = credentials_local["port"];
    const std::string user_local = credentials_local["user"];
    const std::string password_local = credentials_local["password"];

    driver::DatabaseHandler dbHandler(host_local, port_local, user_local, password_local);

    // //Read credentials from the file for cluster
    // std::map<std::string, std::string> credentials_cluster = driver::QueryExecutor::readCredentials("credentials_cluster.txt");

    // // Check if all required credentials are present
    // if (credentials_cluster.find("host") == credentials_cluster.end() ||
    //     credentials_cluster.find("port") == credentials_cluster.end() ||
    //     credentials_cluster.find("user") == credentials_cluster.end() ||
    //     credentials_cluster.find("password") == credentials_cluster.end()) {
    //     std::cerr << "Invalid or incomplete credentials in the file." << std::endl;
    //     return;
    // }

    // const std::string host_cluster = credentials_cluster["host"];
    // const std::string port_cluster = credentials_cluster["port"];
    // const std::string user_cluster = credentials_cluster["user"];
    // const std::string password_cluster = credentials_cluster["password"];

    // driver::DatabaseHandler dbHandler(host_cluster, port_cluster, user_cluster, password_cluster);

    if (dbHandler.connect()) {
        driver::LogFileHandler::clearLogFile("output/log.txt");
        driver::QueryFileHandler::clearQueryFile("output/queries.txt");

        if (driver::LogFileHandler::createLogFile("log.txt")) {
            std::string queryFileName = "output/queries.txt";
            driver::QueryFileHandler::clearQueryFile(queryFileName);
            driver::QueryFileHandler::createQueryFile(queryFileName);

            // Execute queries
            driver::QueryExecutor::executeQueries(queryFileName, dbHandler);
        }
    }
}


int main(int argc, char *argv[])
{
    char *filePath = argv[1];
    std::unordered_map<std::string, std::string> arguments;
    extractOptionalArgument(arguments, argc, argv);

    std::string verboseKey = "verbose";
    int verbose = 0;
    auto it = arguments.find(verboseKey);
    if (it != arguments.end())
    {
        verbose = std::stoi(it->second);
    }

    driver::JsonFileHandler fileHandler;
    driver::ConfigGenerator configGenerator;
    printf("-----------------------------------------\n");

    cJSON *originalJson = fileHandler.createJSONObject(filePath);

    // cJSON *checkJson = fileHandler.createJSONObject(argv[2]);
    // std::vector<std::string> keys = {};
    // std::cout << fileHandler.compareCJSONObjects(originalJson, checkJson, keys) << std::endl;

    // cJSON_Delete(checkJson);

    if (verbose > 10)
    {
        printf("Original JSON Input File\n-----------------------------------------\n");
        std::cout << cJSON_Print(originalJson) << std::endl;
    }

    cJSON *inputJson = cJSON_Duplicate(originalJson, 1);
    std::vector<driver::Patch> patches = {};

    std::vector<std::vector<std::string>> patchableKeys = {};
    fileHandler.createConfigFile(inputJson, {}, patchableKeys);
    // std::cout << "AS" << std::endl;
    configGenerator.createPatches(inputJson, patchableKeys, patches, verbose);

    if (verbose > 10)
    {
        printf("-----------------------------------------\nInitial Deployment Configuration\n-----------------------------------------\n");
        std::cout << cJSON_Print(inputJson) << std::endl;
        printf("-----------------------------------------\n");
    }

    scheduler::ClusterAccess clusterAccess;
    if (clusterAccess.create(
            inputJson,
            NULL,
            "default",
            verbose))
    {
        std::cout << "Initial Application Setup Completed" << std::endl;
    }
    else
    {
        printf("Initial Deployment Failed\n");
        return 1;
        // std::cout << "Initial Deployment Failed\n" << std::endl;
    }

    Evaluator evaluator;
    std::thread myThread(updatePatches, std::ref(patches), std::ref(clusterAccess), inputJson, std::ref(evaluator), verbose);
    // query generator and kubernetes service connector
    std::thread queryThread(setupAndExecuteQueries);

    myThread.join();
    queryThread.join();

    cJSON_Delete(inputJson);
    cJSON_Delete(originalJson);
    return 0;
}